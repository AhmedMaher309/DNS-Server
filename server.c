#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DB_driver.h"
#include "get_ip_address.h"

#define PORT 8080
#define BUFFER_SIZE 1024
const char* dbName;

void 
handleError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int 
createSocket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        handleError("socket failed");
    return server_fd;
}

void 
setSocketOptions(int server_fd) {
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        handleError("setsockopt");
}

void 
bindSocket(int server_fd) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        handleError("bind failed");
}

void 
startListening(int server_fd) {
    if (listen(server_fd, 3) < 0)
        handleError("listen");
}

int 
acceptConnection(int server_fd, struct sockaddr_in *address, socklen_t *addrlen) {
    int new_socket = accept(server_fd, (struct sockaddr *)address, addrlen);
    if (new_socket < 0)
        handleError("accept");
    return new_socket;
}

void 
handleClient(int new_socket) {
    char hostname[BUFFER_SIZE] = {0};
    ssize_t valread = read(new_socket, hostname, BUFFER_SIZE - 1);
    printf("%s\n", hostname);
    char * ip = searchIPByName(dbName, hostname);
    if(ip == NULL){
        ip = getIPAddress(hostname);
    }
    if(ip == NULL){
        ip = "not found";
    }
    printf("ip is sent\n");
    send(new_socket, ip, strlen(ip), 0);
    close(new_socket);
}

int 
main(int argc, char const *argv[]) {
    dbName = createSQLiteDatabase("database");
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = createSocket();
    setSocketOptions(server_fd);
    bindSocket(server_fd);
    startListening(server_fd);

    while (1) {
        new_socket = acceptConnection(server_fd, &address, &addrlen);
        handleClient(new_socket);
    }

    return 0;
}