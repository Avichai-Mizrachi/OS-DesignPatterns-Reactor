# Reactor Chat Application

This is an implementation of a chat application that supports an unlimited number of customers using the reactor design pattern and either the `poll()` or `select()` system calls.

## Introduction

The reactor chat application is designed to handle multiple client connections efficiently. It utilizes the reactor design pattern, where a reactor mechanism receives client file descriptors and associated functions to execute when the file descriptors become "hot" or ready for reading.

## Implementation Details

The implementation of the reactor chat application involves the following components:

1. Reactor Library (`st_reactor.so`):
   - The reactor library provides the core functionality for the reactor design pattern.
   - It allows registration of file descriptors and associated handler functions.
   - The library uses `poll()` or `select()` to listen to multiple file descriptors simultaneously.
   - The reactor runs in a single thread, processing messages as they arrive.

2. Reactor API:
   - `void* createReactor()`: Creates a reactor and returns a pointer to the reactor structure.
   - `void stopReactor(void* reactor)`: Stops the reactor if it is active.
   - `void startReactor(void* reactor)`: Starts the reactor.
   - `void addFd(void* reactor, int fd, handler_t handler)`: Adds a file descriptor to the reactor and associates it with a handler function.
   - `void waitFor(void* reactor)`: Waits for the reactor thread to finish using `pthread_join()`.

3. Application:
   - The chat server application follows the design of Big's chat server, such as `selectserver`.
   - The client application interacts with the chat server as per the requirements of the assignment.
   - The server application runs the reactor in a separate thread and waits for it to finish.


## How to Run

To run the React Server, follow these steps:

1. Open a terminal or command prompt.
2. Navigate to the directory where the `react_server` executable is located.
3. Run the following command:

   ```
   ./react_server
   ```

This command will execute the React Server and start the application.

## Authors

Avichai Mizrachi
Maya Rom
