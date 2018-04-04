#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 25565
#define MAX_CONNECTIONS 64
#define EVENT_BUFFER_SIZE 32
#define DATA_BUFFER_SIZE 1024

/**
 * Handles an incoming data packet
 *
 * @param fd the socket FD from which the packet originates
 * @param dataBuffer the data buffer used to store the data
 */
int handle(int fd, char *dataBuffer) {
    ssize_t readLength = read(fd, dataBuffer, DATA_BUFFER_SIZE);
    if (readLength < 1) {
        return 1;
    }

    printf("Received string length %ld: ", readLength);
    for (int i = 0; i < readLength; i++) {
        printf("%c", dataBuffer[i]);
    }
    printf("\n");

    ssize_t writeResult = write(fd, dataBuffer, (size_t) readLength);
    if (writeResult == -1) {
        puts("Could not write");
        return 1;
    }

    return 0;
}

int main() {
    // Create a new TCP socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        puts("Failed to open FD");
        return 1;
    }

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    int bindResult = bind(fd, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
    if (bindResult == -1) {
        puts("Failed to bind to port");
        return 1;
    }

    // Allow socket to accept connections
    int listenResult = listen(fd, MAX_CONNECTIONS);
    if (listenResult == -1) {
        puts("Failed to listen");
        return 1;
    }

    // Create an epoll instance
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        puts("Could not create epoll instance");
        return 1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;

    // Add the socket to epoll instance
    int ctlResult = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    if (ctlResult == -1) {
        puts("Could not update epoll");
        return 1;
    }

    // Buffer used to store events retrieved from epoll
    struct epoll_event *buffer = calloc(EVENT_BUFFER_SIZE, sizeof(struct epoll_event));
    // Buffer used to read data off the socket
    char *dataBuffer = calloc(DATA_BUFFER_SIZE, sizeof(char));
    while (1) {
        int events = epoll_wait(epfd, buffer, EVENT_BUFFER_SIZE, -1);
        for (int i = 0; i < events; i++) {
            struct epoll_event event = buffer[i];
            if (event.data.fd == fd) {
                int clientFd = accept(fd, NULL, NULL);
                ev.events = EPOLLIN;
                ev.data.fd = clientFd;

                int clientCtlResult = epoll_ctl(epfd, EPOLL_CTL_ADD, clientFd, &ev);
                if (clientCtlResult == -1) {
                    puts("Error adding client");
                    return 1;
                }
                puts("Connected client");
            } else {
                int clientFd = event.data.fd;
                if (handle(clientFd, dataBuffer)) {
                    int clientCtlResult = epoll_ctl(epfd, EPOLL_CTL_DEL, clientFd, &ev);
                    if (clientCtlResult == -1) {
                        puts("Error deleting client");
                        return 1;
                    }

                    close(clientFd);
                    puts("Disconnected client");
                }
            }
        }
    }
}