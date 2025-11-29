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

## Quick Start
Use localhost for "server-addr" if you're testing this on local machine
```console
gcc ./server -o server server.c
gcc ./main -o main main.c

./server <port>
./main <server-addr> <port>
```
### Features
- [x] Tcp client with simple TUI
- [x] Tcp server
- [ ] Tcp server that broadcasts messages
- [x] IPv4 support
- [ ] IPv6 support
- [ ] UNIX sockets support


## Changelog

### [1.0.0] - 2025-Sat Nov 29

#### Added
- Initial release
- Core functionality
- Socket API

#### Changed
- N/A

#### Fixed
- N/A
