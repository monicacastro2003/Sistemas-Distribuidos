// voter_client.cpp
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "voting.pb.h"
#include "common.h"

using namespace std;
using namespace voting;

int main(int argc, char** argv) {
    string server = "127.0.0.1";
    int port = 5555;
    string voterId = "eleitor1";
    if (argc > 1) voterId = argv[1];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, server.c_str(), &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("connect"); close(sock); return 1; }

    LoginRequest lr;
    lr.set_voterid(voterId);
    string out;
    lr.SerializeToString(&out);
    if (!send_message_prefixed(sock, out)) { cerr << "send failed\n"; close(sock); return 1; }

    string reply_str = recv_message_prefixed(sock);
    if (reply_str.empty()) { cerr << "no reply\n"; close(sock); return 1; }
    LoginReply lrep;
    lrep.ParseFromString(reply_str);
    cout << "Server: " << lrep.message() << "\n";
    cout << "Candidates:\n";
    for (const auto& c : lrep.candidates()) {
        cout << c.id() << ": " << c.name() << " (" << c.party() << ")\n";
    }

    cout << "Digite o id do candidato para votar: ";
    int choice;
    if (!(cin >> choice)) { close(sock); return 1; }

    VoteRequest vr;
    vr.set_voterid(voterId);
    vr.set_candidateid(choice);
    string vout;
    vr.SerializeToString(&vout);
    if (!send_message_prefixed(sock, vout)) { cerr << "send vote failed\n"; close(sock); return 1; }

    string vreply_str = recv_message_prefixed(sock);
    if (vreply_str.empty()) { cerr << "no vote reply\n"; close(sock); return 1; }
    VoteReply vrep;
    vrep.ParseFromString(vreply_str);
    cout << "Vote reply: " << vrep.message() << "\n";
    close(sock);
    return 0;
}
