// compile with
// gcc -shared -fPIC -ldl udp_packetdup.c -o udp_packetdup.so
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>

int dup_count = 1;

typedef ssize_t (*sendto_t)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
sendto_t real_sendto;

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    if (!real_sendto) {
        real_sendto = dlsym(RTLD_NEXT, "sendto");

        char* UDP_PACKETDUP_ENVVAR = secure_getenv("UDP_PACKETDUP");
        if (UDP_PACKETDUP_ENVVAR && strlen(UDP_PACKETDUP_ENVVAR) > 0) {
            dup_count = atoi(UDP_PACKETDUP_ENVVAR);
            if (dup_count < 1)
                dup_count = 1;
            if (dup_count > 10)
                dup_count = 10;
        }
    }

    for (int i = 0; i < dup_count; ++i)
        real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
    return real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}
