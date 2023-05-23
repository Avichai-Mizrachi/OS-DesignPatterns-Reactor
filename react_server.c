#include "reactor.h"

//The port we are listening on
#define PORT "9034" 
Reactor *gReactor;

// The sighandler function is intended to handle the SIGINT signal.
void sighandler(int sig)
{
   if(sig == SIGINT)
   {
      stopReactor(gReactor);
      deleteReactor(gReactor);
      exit(0);
   }
}


// The get_listener_socket function creates a listening socket,
// binds it to a specified port,
// and sets it up to listen for incoming connections.
int get_listener_socket(void)
{
    int listenfd;
    int rv;
    int yes = 1;

    struct addrinfo hints, *ai, *p;
    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        gotAnError();
    }
    for(p = ai; p != NULL; p = p->ai_next) {
        listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenfd < 0) { 
            continue;
        }
        // For "address already in use"
        
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) < 0) {
            close(listenfd);
            continue;
        }

        break;
    }

    freeaddrinfo(ai);
    printf("Server is up and running on port 9034\n");

    
    if (p == NULL) {
        printf("p == null\n");
        return -1;
    }
    if (listen(listenfd, 5120) == -1) {
        return -1;
    }

    return listenfd;
}


// This function is responsible for removing a file descriptor
// and its associated handler from the pfds and
// handlers arrays of a Reactor.
void del_from_pfds(Reactor* reactor, int fd)
{
    // Find the index (i) for the specified file descriptor (fd)
    int i;
    for (i = 0; i < reactor->count; i++) {
        if (reactor->pfds[i].fd == fd) {
            break;
        }
    }

    // If the file descriptor is not found, return
    if (i == reactor->count) {
        return;
    }

    // Copy the last entry over the entry at index i
    reactor->pfds[i] = reactor->pfds[reactor->count - 1];
    reactor->handlers[i] = reactor->handlers[reactor->count - 1];
   
    reactor->count--;
}


// The handleClientData function handles the processing of client data
// and manages the cleanup if there are any errors
// or the connection is closed.
void handleClientData(int fd, void *arg)
{
    Reactor* reactor = (Reactor*)arg;
    char buf[256];

    if (recv(fd, buf, sizeof buf, 0) <= 0) 
    {
        close(fd);
        del_from_pfds(reactor, fd);
    } 
    else {
        printf("MSG: %s\n", buf);
    }
}


// The newConnectionHandler function is a callback function
// used in the reactor pattern to handle new client connections.
void newConnectionHandler(int fd, void* arg) {
    Reactor* reactor = (Reactor*)arg;
    int listener = reactor->listenFd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    addrlen = sizeof remoteaddr;
    int newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
    if (newfd == -1) {
        perror("Error in accept");
    } else {
        addFd(reactor, newfd,handleClientData);
    }
}


int main(void)
{
    gReactor = createReactor();
    signal(SIGINT, sighandler);
    int listener;
    listener = get_listener_socket();
    if (listener == -1) {
        gotAnError();
    }
    addFd(gReactor, listener, newConnectionHandler);
    startReactor(gReactor);
    waitFor(gReactor);
    return 0;
}