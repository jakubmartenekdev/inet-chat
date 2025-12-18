# Inet-chat

TCP chat client, similar to telnet. This project was made for learning purposes.

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
> [!NOTE]
> Use make command or compile from source

```console
make all

./bin/server <port>
./bin/client <server-addr> <port>
```
### Features
- [x] Tcp client with simple TUI
- [x] Tcp server that broadcasts messages
- [ ] Uses threadpool
- [x] IPv4 support
- [ ] IPv6 support
- [ ] UNIX sockets support
- [x] Makefile
- [ ] Included testing framework
- [x] Many memory leaks


## Changelog

### [1.2.2] - 2025 Thu Dec 18

#### Changed
- Refactored Makefile, made use of built in functions like wildcard, patsubst and filter-out
- Changed project structure, including added sub-directories for binaries

### [1.2.1] - 2025 Fri Dec 12

#### Changed
- Append buffer now stores an initial capacity, making it more effective by doubling the size when full and limits calls to realloc

#### To fix
- [ ] Apparently there's a bug, when I append too many messages, program crashes

### [1.2.0] - 2025 Sun Dec 7

#### Added
- Makefile

#### To fix
- [ ] Implement a thread pool

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
