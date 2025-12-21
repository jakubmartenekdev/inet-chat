# Inet-chat
A real-time TCP chat application

>[!WARN]
> This is a toy project I made to learn core systems programming concepts in C and such, is only intended to use for learning purposes only

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
> [!NOTE]
> Type "localhost" for \<server-addr\> if you're testing this on a local machine
```console
make all DEBUG=0

./build/release/bin/server <port>
./build/release/bin/client <server-addr> <port>
```

### Features
- [x] Tcp client with simple TUI
- [x] Tcp server that broadcasts messages
- [x] Uses threadpool
- [x] IPv4 support
- [ ] IPv6 support
- [ ] UNIX sockets support
- [x] Makefile
- [x] Criterion - testing framework
- [x] Unit tests
- [ ] Integrated tests
- [x] Many memory leaks


## Changelog

### [1.3.4] - 2025 Sun Dec 21

#### Changed
- Fixed bug where sent text appeard inline because of incorrect use of a new line character 

#### To fix
- [ ] Add scroll in the client tui, when the buffer reaches the end of window, scroll automatically

### [1.3.3] - 2025 Sat Dec 20

#### Changed
- Server uses thread pool instead of processes to handle client requests
- Server is capable to handle hundreds of connections

### [1.3.2] - 2025 Fri Dec 19

#### Added
- Criterion testing framework, controlled with Makefile
- Common header files for ll_queue.c and client.c

### [1.2.2] - 2025 Thu Dec 18

#### Changed
- Refactored Makefile, made use of built in functions like wildcard, patsubst and filter-out
- Changed project structure, including added sub-directories for binaries

### [1.2.1] - 2025 Fri Dec 12

#### Changed
- Append buffer now stores an initial capacity, making it more effective by doubling the size when it's full and limits calls to realloc

#### To fix
- [x] Apparently there's a bug, when I append too many messages, program crashes

### [1.2.0] - 2025 Sun Dec 7

#### Added
- Makefile

#### To fix
- [x] Implement a thread pool

### [1.1.0] - 2025 Sun Nov 30

#### Added
- Message broadcasting

#### Changed
- Socket reads are now non blocking

#### To fix
- ~~[ ] Client socket should be in shared memory for proper cleanup~~
- [x] Let have more than 5 connections per server runtime

### [1.0.0] - 2025 Sat Nov 29

#### Added
- Initial release
- Socket API
- Hostname to address translation

#### Changed
- N/A

#### Fixed
- N/A
