// admin_client.cpp
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "voting.pb.h"

using namespace std;
using namespace voting;

int main(int argc, char** argv) {
    string mcast = "230.0.0.1";
    int port = 4446;
    string title = "Admin";
    string body = "Aviso administrativo";

    if (argc >= 3) {
        title = argv[1];
        body = argv[2];
    } else {
        cout << "Usage: admin_client \"Title\" \"Body\"\nUsing default title/body.\n";
    }

    AdminNotice n;
    n.set_title(title);
    n.set_body(body);
    n.set_timestamp((google::protobuf::int64)0);

    string out;
    n.SerializeToString(&out);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    int ttl = 1;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, mcast.c_str(), &addr.sin_addr);

    if (sendto(sock, out.data(), out.size(), 0, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        close(sock);
        return 1;
    }
    cout << "Notice sent.\n";
    close(sock);
    return 0;
}
