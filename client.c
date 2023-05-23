#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9034
#define MESSAGE "Got it."

//Error msg
void gotAnError(){
    perror("Error");
    exit(1);
}


// This code snippet defines a function named clientThread
// that is meant to be executed as a separate thread.
void* clientThread(void* arg) {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        gotAnError();
    }

    // Set up the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr)); // Initialize serverAddr to zero
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        gotAnError();
    }

    // Send message to the server
    if (send(clientSocket, MESSAGE, strlen(MESSAGE), 0) == -1) {
        gotAnError();
    }

    // Return the client socket file descriptor
    return (void*)(intptr_t)clientSocket;
}

int main() {
    int i = 0;
    pthread_t threads[100];
    int clientSockets[100];

    // Create 100 client threads
    while (i < 100) {
        if (pthread_create(&threads[i], NULL, clientThread, NULL) != 0) {
            perror("pthread_create");
            exit(1);
        }
        i++;
    }

    i = 0;
    bool allThreadsCompleted = false;
    // Wait for all client threads to complete and collect client sockets
    while (!allThreadsCompleted) {
        if (pthread_join(threads[i], (void**)&clientSockets[i]) != 0) {
            perror("pthread_join");
            exit(1);
        }
        i++;
        if (i == 100) {
            allThreadsCompleted = true;
        }
    }

    i = 0;
    // Close all client sockets
    while (i < 100) {
        close((int)(intptr_t)clientSockets[i]);
        i++;
    }

    return 0;
}
