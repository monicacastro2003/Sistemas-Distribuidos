// multicast_listener.cpp
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "voting.pb.h"

using namespace std;
using namespace voting;

int main(int argc, char** argv) {
    string mcast = "230.0.0.1";
    int port = 4446;
    if (argc >= 2) mcast = argv[1];

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&local, sizeof(local)) < 0) { perror("bind"); close(sock); return 1; }

    ip_mreq mreq{};
    inet_pton(AF_INET, mcast.c_str(), &mreq.imr_multiaddr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt join");
        close(sock);
        return 1;
    }

    cout << "Listening to multicast " << mcast << ":" << port << " ...\n";
    while (true) {
        char buf[65536];
        ssize_t n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) { perror("recv"); break; }
        string s(buf, buf + n);
        AdminNotice notice;
        if (notice.ParseFromString(s)) {
            cout << "NOTICE: " << notice.title() << " - " << notice.body() << "\n";
        } else {
            cout << "Received non-protobuf or invalid notice.\n";
        }
    }
    close(sock);
    return 0;
}
