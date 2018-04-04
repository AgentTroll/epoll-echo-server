#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>

#define PORT 25565
#define DATA_BUFFER_SIZE 1024
#define MSG "Hello World!"

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

    // Connect socket to server 
    int connectResult = connect(fd, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
    if (connectResult == -1) {
        puts("Failed to bind to port");
        return 1;
    }

    size_t len = strlen(MSG);
    char *dataBuffer = calloc(DATA_BUFFER_SIZE, sizeof(char));
    while (1) {
        ssize_t writeResult = write(fd, MSG, len);
        printf("Wrote to server: %s\n", MSG);
        if (writeResult == -1) {
            printf("Error writing");
            return 1;
        }

        printf("Got back from server: ");
        ssize_t readLength = read(fd, dataBuffer, DATA_BUFFER_SIZE);
        for (int i = 0; i < readLength; ++i) {
            printf("%c", dataBuffer[i]);
        }
        printf("\n");

        sleep(1);
    }
}