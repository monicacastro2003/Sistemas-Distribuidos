// server.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <signal.h>

#include "voting.pb.h"
#include "common.h"

using namespace std;
using namespace voting;

struct CandidateData {
    int id;
    string name;
    string party;
};

class VoteServer {
public:
    VoteServer(int port_tcp = 5555, const string& mcastAddr = "230.0.0.1", int mcastPort = 4446, int votingDurationSec = 120)
        : tcp_port(port_tcp), mcast_addr(mcastAddr), mcast_port(mcastPort),
          voting_deadline(chrono::steady_clock::now() + chrono::seconds(votingDurationSec)), running(true)
    {
        // seed candidates
        addCandidate(1, "Alice", "PartA");
        addCandidate(2, "Bob", "PartB");
    }

    void start() {
        // start TCP listener
        tcp_thread = thread(&VoteServer::tcpListenLoop, this);
        cout << "Server started (TCP port " << tcp_port << "), voting until deadline.\n";
    }

    void stop() {
        running = false;
        if (tcp_thread.joinable()) tcp_thread.join();
    }

    // send a multicast notice (protobuf AdminNotice serialized)
    void sendAdminNotice(const string& title, const string& body) {
        AdminNotice notice;
        notice.set_title(title);
        notice.set_body(body);
        notice.set_timestamp((google::protobuf::int64)chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()).count());

        string out;
        notice.SerializeToString(&out);

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("multicast socket");
            return;
        }
        // set TTL and allow loopback (optional)
        int ttl = 1;
        setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(mcast_port);
        inet_pton(AF_INET, mcast_addr.c_str(), &addr.sin_addr);

        ssize_t res = sendto(sock, out.data(), out.size(), 0, (sockaddr*)&addr, sizeof(addr));
        if (res < 0) perror("sendto multicast");
        close(sock);
    }

private:
    int tcp_port;
    string mcast_addr;
    int mcast_port;
    chrono::steady_clock::time_point voting_deadline;
    atomic<bool> running;
    thread tcp_thread;

    // data structures
    unordered_map<int, CandidateData> candidates;
    unordered_map<int, int> voteCounts;
    unordered_set<string> votersVoted;
    mutex mtx;

    void addCandidate(int id, const string& name, const string& party) {
        lock_guard<mutex> lg(mtx);
        candidates[id] = CandidateData{id, name, party};
        voteCounts[id] = 0;
    }

    void tcpListenLoop() {
        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0) { perror("socket"); return; }
        int opt = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons((uint16_t)tcp_port);

        if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(listen_fd); return; }
        if (listen(listen_fd, 16) < 0) { perror("listen"); close(listen_fd); return; }

        while (running) {
            sockaddr_in cli{};
            socklen_t len = sizeof(cli);
            int client_fd = accept(listen_fd, (sockaddr*)&cli, &len);
            if (client_fd < 0) { if (running) perror("accept"); break; }
            thread(&VoteServer::handleClient, this, client_fd).detach();
        }
        close(listen_fd);
    }

    void handleClient(int fd) {
        // read LoginRequest
        string in = recv_message_prefixed(fd);
        if (in.empty()) { close(fd); return; }

        LoginRequest lr;
        if (!lr.ParseFromString(in)) { close(fd); return; }
        string voterId = lr.voterid();

        // prepare LoginReply with candidates
        LoginReply reply;
        reply.set_ok(true);
        reply.set_message("Bem-vindo " + voterId);

        {
            lock_guard<mutex> lg(mtx);
            for (auto &p : candidates) {
                Candidate* c = reply.add_candidates();
                c->set_id(p.second.id);
                c->set_name(p.second.name);
                c->set_party(p.second.party);
            }
        }
        string out;
        reply.SerializeToString(&out);
        if (!send_message_prefixed(fd, out)) { close(fd); return; }

        // read next message (expect VoteRequest)
        string in2 = recv_message_prefixed(fd);
        if (in2.empty()) { close(fd); return; }

        VoteRequest vr;
        VoteReply vreply;
        if (!vr.ParseFromString(in2)) {
            // not a VoteRequest; ignore
            vreply.set_accepted(false);
            vreply.set_message("Formato de mensagem invalido");
        } else {
            // process vote
            lock_guard<mutex> lg(mtx);
            auto now = chrono::steady_clock::now();
            if (now > voting_deadline) {
                vreply.set_accepted(false);
                vreply.set_message("Votacao encerrada");
            } else if (votersVoted.find(voterId) != votersVoted.end()) {
                vreply.set_accepted(false);
                vreply.set_message("Eleitor ja votou");
            } else if (candidates.find(vr.candidateid()) == candidates.end()) {
                vreply.set_accepted(false);
                vreply.set_message("Candidato invalido");
            } else {
                votersVoted.insert(voterId);
                voteCounts[vr.candidateid()] += 1;
                vreply.set_accepted(true);
                vreply.set_message("Voto registrado");
            }
        }
        string out2;
        vreply.SerializeToString(&out2);
        send_message_prefixed(fd, out2);
        close(fd);
    }

public:
    // compute results (thread-safe)
    Results computeResults() {
        Results r;
        lock_guard<mutex> lg(mtx);
        int total = 0;
        for (auto &p : voteCounts) total += p.second;
        r.set_totalvotes(total);
        for (auto &p : candidates) {
            CandidateResult* cr = r.add_results();
            Candidate* c = cr->mutable_candidate();
            c->set_id(p.second.id);
            c->set_name(p.second.name);
            c->set_party(p.second.party);
            int votes = voteCounts[p.first];
            cr->set_votes(votes);
            cr->set_percent(total == 0 ? 0.0 : (100.0 * votes / total));
        }
        return r;
    }
};

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    int duration = 120;
    if (argc > 1) duration = atoi(argv[1]);
    VoteServer server(5555, "230.0.0.1", 4446, duration);
    server.start();

    // for demo we send a multicast "voting started"
    this_thread::sleep_for(chrono::milliseconds(200));
    server.sendAdminNotice("Votacao", "Votacao iniciada!");

    // wait until deadline + 1s
    this_thread::sleep_for(chrono::seconds(duration + 1));
    cout << "Deadline reached. Computing results...\n";
    Results res = server.computeResults();
    cout << "Total votes: " << res.totalvotes() << "\n";
    for (const auto& cr : res.results()) {
        cout << "Candidate " << cr.candidate().name() << " -> " << cr.votes() << " (" << cr.percent() << "%)\n";
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
