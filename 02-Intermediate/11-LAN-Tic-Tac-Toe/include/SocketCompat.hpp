#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #if defined(_MSC_VER)
        #pragma comment(lib, "ws2_32.lib")
    #endif
    using socket_t = SOCKET;
    constexpr socket_t INVALID_SOCK = INVALID_SOCKET;
    constexpr int SOCK_ERR = SOCKET_ERROR;

    inline bool initNet() {
        WSADATA wsa;
        return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    }
    inline void cleanupNet() { WSACleanup(); }
    inline int closeNetSock(socket_t s) { return closesocket(s); }
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    using socket_t = int;
    constexpr socket_t INVALID_SOCK = -1;
    constexpr int SOCK_ERR = -1;

    inline bool initNet() { return true; }
    inline void cleanupNet() {}
    inline int closeNetSock(socket_t s) { return close(s); }
#endif
