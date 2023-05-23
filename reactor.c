#include "reactor.h"

// The createReactor function allocates memory for a Reactor object,
// initializes its fields, and returns a pointer to the created object
void *createReactor()
{
    Reactor *reactor = malloc(sizeof(Reactor));
    reactor->count = 0;
    reactor->size = 5;
    reactor->pfds = malloc(sizeof(*reactor->pfds) * reactor->size);
    reactor->isRunning = 0;
    reactor->handlers = malloc(sizeof(*reactor->handlers) * reactor->size);
    reactor->listenFd = 0;
    reactor->remoteIP = malloc(INET6_ADDRSTRLEN);
    return reactor;
}

// The stopReactor function is responsible for stopping the reactor by
// terminating the reactor thread and cleaning up associated resources
void stopReactor(void *this)
{
    Reactor *reactor = (Reactor *)this;
    if (reactor->isRunning == 1)
    {
        reactor->isRunning = 0;
        pthread_cancel(reactor->thread);
        pthread_join(reactor->thread, NULL);
    }
}

// Start the reactor in a separate thread
void startReactor(void *this)
{
    Reactor *reactor = (Reactor *)this;

    // In case the reactor is already running
    if (reactor->isRunning == 1)
    {
        return;
    }
    reactor->isRunning = 1;
    if (pthread_create(&reactor->thread, NULL, reactorThread, reactor) != 0)
    {
        gotAnError();
    }
}

// This function checks if the reactor is running.
// In case he is alive it run through all the existing connections 
// and check if one of them is ready to read.
// If someone is ready , it handle new connection and if not we're a regular client.
void *reactorThread(void *arg)
{
    // Wait for events on file descriptors
    Reactor *reactor = (Reactor *)arg;
    while (reactor->isRunning == 1)
    {
        if (poll(reactor->pfds, reactor->count, -1) == -1)
        {
            gotAnError();
        }

        // Handle events on file descriptors
        for (int i = 0; i < reactor->count; i++)
        {
            if (reactor->pfds[i].revents & POLLIN)
            {
                // Check if the file descriptor is the listener
                if (reactor->pfds[i].fd == reactor->listenFd)
                {
                    // Accept new connection
                    reactor->handlers[i](reactor->pfds[i].fd, reactor);
                    break;
                }
                else
                {
                    // Handle data on regular client socket
                    reactor->handlers[i](reactor->pfds[i].fd, reactor);
                }
            }
        }
    }

    return NULL;
}



// The addFd function allows you to dynamically add file descriptors and
// their handlers to a reactor, 
// expanding the capacity of the reactor if needed.
void addFd(void *this, int newfd, handler_t handler)
{
    Reactor *reactor = (Reactor *)this;
    if (reactor->listenFd == 0){
        reactor->listenFd = newfd;
    }
    if (reactor->count >= reactor->size)
    {
        // Double the size of handlers and pfds arrays
        reactor->size *= 2;
        reactor->handlers = realloc(reactor->handlers, sizeof(handler_t) * reactor->size);
        reactor->pfds = realloc(reactor->pfds, sizeof(*reactor->pfds) * reactor->size);
    }

    reactor->handlers[reactor->count] = handler;
    reactor->pfds[reactor->count].fd = newfd;
    reactor->pfds[reactor->count].events = POLLIN;
    reactor->count++;
}

// The waitFor function allows you to wait for
// the reactor thread to finish its execution
void waitFor(void *this)
{
    Reactor *reactor = (Reactor *)this;

    if (reactor->isRunning)
    {
        // Wait for the reactor thread to finish
        if (pthread_join(reactor->thread, NULL) != 0)
        {
            gotAnError();
        }
    }
}

// The deleteReactor function provides a way to delete a Reactor object
// and release the memory it occupies.
// It also takes care of waiting for the reactor thread to finish
// its execution before deleting the reactor, ensuring proper cleanup.
void deleteReactor(void *this)
{
    if (this == NULL)
    {
        printf("Reactor is NULL\n");
        return;
    }
    Reactor *reactor = (Reactor *)this;

    if (reactor->isRunning)
    {
        waitFor(this);
    }
    free(reactor->handlers);
    free(reactor->pfds);
    free(reactor->remoteIP);
    free(reactor);
}