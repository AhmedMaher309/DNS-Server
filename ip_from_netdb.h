#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

// Function to retrieve the IPv4 address from hostname
char* getIPAddress(const char* hostname) {
    struct addrinfo hints, *res;
    char ipstr[INET_ADDRSTRLEN]; // Use INET_ADDRSTRLEN for IPv4

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // Use IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return NULL;
    }

    // Iterate through the results and return the first IPv4 address
    for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            const char* addr = inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
            if (addr != NULL) {
                freeaddrinfo(res); // Free the memory allocated by getaddrinfo
                return strdup(addr); // Return a dynamically allocated string
            }
        }
    }

    freeaddrinfo(res); // Free the memory allocated by getaddrinfo
    return NULL;
}
