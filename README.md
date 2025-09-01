# Minimal HTTP Server in C

A lightweight HTTP server written in C, built from scratch using BSD sockets.  
This project was inspired by [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) and demonstrates how to handle TCP connections, parse HTTP requests, and send valid responses.

---

## Features
- Handles incoming TCP connections on a specified port.
- Parses basic HTTP requests (`GET` method).
- Responds with valid HTTP/1.1 headers and body.
- Supports multiple concurrent clients (via `fork()` or `select()`).
- Tested using Docker containers for simulating **50+ concurrent requests**.

---

## Getting Started

### Prerequisites
- GCC or Clang (any C99-compatible compiler)
- Linux or macOS (Windows works with WSL or MinGW)
- Docker (optional, for testing multiple clients)

### Build
```bash
gcc -o server server.c
```

## Run
```bash
./server 8080
```
This starts the server on port 8080.

Open your browser and go to:
```cpp
http://127.0.0.1:8080/
```

## Testing
### Single request with curl
```bash
curl http://127.0.0.1:8080/
```

### Multiple concurrent requests with Docker
```bash
docker run --rm williamyeh/wrk -t4 -c50 -d10s http://host.docker.internal:8080/
```
### Manual request with telnet
```bash
telnet 127.0.0.1 8080
GET / HTTP/1.1
Host: localhost
```

## Project Structure
```bash
.
├── server.c     # HTTP server implementation
├── README.md    # Project documentation
└── Makefile     # (optional) build automation
```

## Future Improvements
Add support for POST requests.

Serve static files (HTML, CSS, JS).

Implement keep-alive connections.

Add MIME type detection.

## License
This project is open source under the MIT License.
