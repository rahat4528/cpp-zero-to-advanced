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
    constexpr socket_t INVALID_SOCK_FD = INVALID_SOCKET;
    constexpr int SOCK_ERROR_CODE = SOCKET_ERROR;

    inline bool initCloudSockets() {
        WSADATA wsa;
        return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    }
    inline void cleanupCloudSockets() { WSACleanup(); }
    inline int closeCloudSocket(socket_t s) { return closesocket(s); }
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    using socket_t = int;
    constexpr socket_t INVALID_SOCK_FD = -1;
    constexpr int SOCK_ERROR_CODE = -1;

    inline bool initCloudSockets() { return true; }
    inline void cleanupCloudSockets() {}
    inline int closeCloudSocket(socket_t s) { return close(s); }
#endif
