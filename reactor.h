#pragma once
#include <pthread.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>

// Function pointer type for event handlers
typedef void (*handler_t)(int, void* arg);

typedef struct {
    int listenFd; // Listener FD
    struct pollfd* pfds; // Array of fds
    int count;  // Number of fds
    int size; // Size of the pfds array
    int isRunning; // Flag for reactor
    pthread_t thread; // Thread for reactor
    handler_t* handlers; // Handlers array
    char *remoteIP; // Remote IP address
} Reactor;


typedef Reactor* (*CreateReactorFunc)();
typedef void (*StopReactorFunc)(void* this);
typedef void (*StartReactorFunc)(void* this);
typedef void (*AddFdFunc)(void* this, int fd, handler_t handler);
typedef void (*waitForFunc)(void* this);

//Error msg
void gotAnError(){
    perror("Error");
    exit(1);
}

void* createReactor();
void stopReactor(void* this);
void startReactor(void* this);
void* reactorThread(void* arg);
void addFd(void* this, int fd, handler_t handler);
void waitFor(void* this);
void deleteReactor(void* this);


int get_listener_socket(void);
void handleClientData(int fd, void* arg);
void handleNewConnection(Reactor* reactor);

void* get_in_addr(struct sockaddr *sa);


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}