# Inet-chat
A real-time TCP chat application

>[!WARNING]
> This is a toy project I made to learn application programming in C and such, is only intended to use for learning and integration test purposes

## Table of Contents

- [Requirements](#requirements)
- [Installation](#quick-start)
- [Usage](#usage)
- [Features](#features)
- [Changelog](#changelog)

## Requirements

- gcc, or any C compiler
- POSIX-compliant operating system
- Makefile (optional but recommended)

## Quick Start
```
git clone https://github.com/jakubmartenekdev/inet-chat.git
cd inet-chat/
make all DEBUG=0
sudo make install DEBUG=0 # Optional, avoid typing full path ./build/debug/bin/<prog>
```
## Using build script
```
git clone https://github.com/jakubmartenekdev/inet-chat.git
cd inet-chat/
./build.sh
```
## Usage
> [!NOTE]
> Type "localhost" for \<server-addr\> if you're testing this on a local machine
```
server <port>
client <server-addr> <port>
```

### Features
- [x] Tcp client with simple TUI
- [x] Tcp server that broadcasts messages
- [x] Uses threadpool
- [x] IPv4 support
- [ ] IPv6 support
- [ ] UNIX sockets support
- [x] Makefile
- [x] Build script
- [x] Criterion - testing framework
- [x] Unit tests
- [ ] Integrated tests
- [x] Many memory leaks


## Changelog

### [1.7.0] - 2025 Sun Dec 28

#### Added
- Build script to further automate building process

### [1.6.0] - 2025 Sat Dec 27

#### Added
- Automatic scroll at the end of area buffer

#### Changed
- I changed append buffer to behave slightly differently, it stores char** instead od char* which is basically what vector<string> does, this allows me to have a better control of how I want to render the screen

### [1.5.0] - 2025 Fri Dec 26

#### Added
- Usage messages

### [1.4.1] - 2025 Mon Dec 22

#### Changed
- Many network programs send and receive buffers suffixed with newline character and without null terminator, so I fixed the code to satisfy this "convention?", anyway, client has better support with third party software like tcpbin, which I tested and it works fine

#### To fix
- [x] More descriptive usage messages
- [ ] Add metadata to messages

### [1.4.0] - 2025 Sun Dec 21

#### Changed
- Fixed bug where sent text appeard inline because of incorrect use of a new line character 

#### Added
- make install && make uninstall

#### To fix
- [x] Add scroll in the client tui, when the buffer reaches the end of window, scroll automatically

### [1.3.1] - 2025 Sat Dec 20

#### Changed
- Server uses thread pool instead of processes to handle client requests

### [1.3.0] - 2025 Fri Dec 19

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
