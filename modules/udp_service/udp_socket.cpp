#include "modules/udp_service/udp_socket.hpp"
#include <iostream>

UdpSocket::UdpSocket() : fd_(-1), inited_(false), localPort_(0) {}

bool UdpSocket::open(uint16_t localPort) {
#ifdef _WIN32
    if (!inited_) {
        WSADATA w;
        if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
        inited_ = true;
    }
#endif
    localPort_ = localPort;
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(localPort);

    if (::bind(fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Socket binding failed" << std::endl;
        close();
        return false;
    }

    return true;
}

bool UdpSocket::is_open() const {
    return fd_ >= 0;
}

bool UdpSocket::reopen() {
    close();
    return open(localPort_);
}

int UdpSocket::sendTo(const std::string& ip, uint16_t port, const void* data, int len) {
    if (fd_ < 0) return -1;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

#ifdef _WIN32
    // Windows 使用传统的 inet_addr 来解析 IP 地址
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid address: " << ip << std::endl;
        return -1;
    }
#else
    // 在其他平台使用 inet_pton 进行地址转换
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
#endif

    return ::sendto(fd_, (const char*)data, len, 0, (sockaddr*)&addr, sizeof(addr));
}

int UdpSocket::recvFrom(void* buf, int len, std::string& ip, uint16_t& port) {
    if (fd_ < 0) return -1;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int n = ::recvfrom(fd_, (char*)buf, len, 0, (sockaddr*)&addr, &addrlen);
    if (n >= 0) {
        char tmp[64] = { 0 };
#ifdef _WIN32
        // Windows 下我们不使用 InetNtopA，直接处理
        snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d",
                 (addr.sin_addr.s_addr & 0xFF),
                 ((addr.sin_addr.s_addr >> 8) & 0xFF),
                 ((addr.sin_addr.s_addr >> 16) & 0xFF),
                 ((addr.sin_addr.s_addr >> 24) & 0xFF));
#else
        inet_ntop(AF_INET, &addr.sin_addr, tmp, sizeof(tmp));
#endif
        ip = tmp;
        port = ntohs(addr.sin_port);
    }
    return n;
}

void UdpSocket::close() {
#ifdef _WIN32
    if (fd_ >= 0) ::closesocket(fd_);
#else
    if (fd_ >= 0) ::close(fd_);
#endif
    fd_ = -1;
}

UdpSocket::~UdpSocket() {
    close();
#ifdef _WIN32
    if (inited_) WSACleanup();
#endif
}
