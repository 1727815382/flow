#pragma once
#include <string>
#ifdef _WIN32
    // 确保在 windows.h 之前包含 winsock2.h
    #include <winsock2.h>
    #include <ws2tcpip.h>  // 包含 InetPtonA 和 InetNtopA
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib") // 链接 ws2_32.lib
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

class UdpSocket {
public:
    UdpSocket();
    bool open(uint16_t localPort);
    bool is_open() const;
    bool reopen();
    int sendTo(const std::string& ip, uint16_t port, const void* data, int len);
    int recvFrom(void* buf, int len, std::string& ip, uint16_t& port);
    void close();
    ~UdpSocket();

private:
    int fd_;
    bool inited_;
    uint16_t localPort_;
};
