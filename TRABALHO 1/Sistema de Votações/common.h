// common.h
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdexcept>

inline ssize_t send_all(int fd, const void* buf, size_t len) {
    size_t total = 0;
    const char* p = (const char*)buf;
    while (total < len) {
        ssize_t sent = send(fd, p + total, len - total, 0);
        if (sent <= 0) return sent;
        total += sent;
    }
    return total;
}

inline ssize_t recv_all(int fd, void* buf, size_t len) {
    size_t total = 0;
    char* p = (char*)buf;
    while (total < len) {
        ssize_t got = recv(fd, p + total, len - total, 0);
        if (got <= 0) return got;
        total += got;
    }
    return total;
}

// send protobuf message with 4-byte length prefix (network order)
inline bool send_message_prefixed(int fd, const std::string& data) {
    uint32_t n = htonl((uint32_t)data.size());
    if (send_all(fd, &n, sizeof(n)) <= 0) return false;
    if (data.size() > 0 && send_all(fd, data.data(), data.size()) <= 0) return false;
    return true;
}

// receive protobuf message with 4-byte length prefix; return empty string on error/close
inline std::string recv_message_prefixed(int fd) {
    uint32_t n_net;
    if (recv_all(fd, &n_net, sizeof(n_net)) <= 0) return {};
    uint32_t n = ntohl(n_net);
    std::string buf;
    buf.resize(n);
    if (n > 0) {
        if (recv_all(fd, &buf[0], n) <= 0) return {};
    }
    return buf;
}
