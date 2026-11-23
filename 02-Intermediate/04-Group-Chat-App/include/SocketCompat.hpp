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
    constexpr socket_t INVALID_SOCKET_VAL = INVALID_SOCKET;
    constexpr int SOCKET_ERROR_VAL = SOCKET_ERROR;

    inline bool initSockets() {
        WSADATA wsa;
        return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    }

    inline void cleanupSockets() {
        WSACleanup();
    }

    inline int closeSocket(socket_t s) {
        return closesocket(s);
    }
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
    using socket_t = int;
    constexpr socket_t INVALID_SOCKET_VAL = -1;
    constexpr int SOCKET_ERROR_VAL = -1;

    inline bool initSockets() { return true; }
    inline void cleanupSockets() {}
    inline int closeSocket(socket_t s) { return close(s); }
#endif
