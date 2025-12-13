# Inet-chat

TCP chat client, similar to telnet. This project was built for learning purposes.

## Table of Contents

- [Requirements](#requirements)
- [Installation](#quick-start)
- [Features](#features)
- [Changelog](#changelog)

## Requirements

- gcc, or any C compiler
-  System with POSIX API
- Makefile (optional)

## Quick Start
Use localhost for "server-addr" if you're testing this on local machine
```console
gcc ./server -o server server.
gcc ./main -o main main.c

or simply "make all"

./server <port>
./main <server-addr> <port>
```
### Features
- [x] Tcp client with simple TUI
- [x] Tcp server that broadcasts messages
- [ ] Semaphore queue
- [x] IPv4 support
- [ ] IPv6 support
- [ ] UNIX sockets support
- [x] Makefile
- [x] Many memory leaks


## Changelog

### [1.2.1] - 2025 Fri Dec 12

#### Changed
- Append buffer now stores an initial capacity, making it more effective by doubling the size when full and limits calls to realloc

#### To fix
- [ ] Apparently there's a bug, when I append too many messages, program crashes

### [1.2.0] - 2025 Sun Dec 7

#### Added
- Makefile
- Temporary sync.c for testing

#### To fix
- [ ] Implement a queue using semaphores to ensure that server won't run out of resources

### [1.1.0] - 2025 Sun Nov 30

#### Added
- Message broadcasting

#### Changed
- Socket reads are now non blocking

#### To fix
- [ ] Client socket should be in shared memory for proper cleanup
- [ ] Let have more than 5 connections per server runtime

### [1.0.0] - 2025 Sat Nov 29

#### Added
- Initial release
- Socket API
- Hostname to address translation

#### Changed
- N/A

#### Fixed
- N/A
