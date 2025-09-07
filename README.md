# Lightweight HTTP Server in C

A lightweight HTTP server written in C. This server accepts requests using HTTP 1.0/1.1 and serves back an HTML file.
This project was inspired by [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) and demonstrates how to handle TCP connections, parse HTTP requests, and send valid responses.

---

## Features
- Handles incoming TCP connections on a specified port.
- Parses basic HTTP requests (`GET` method).
- Responds with valid HTTP/1.1 headers and body.
- Supports multiple concurrent clients (via `poll()`).
- Tested using Docker containers for simulating **300+ concurrent requests**.

---

## Getting Started

### Prerequisites
- Linux-based distro (Windows works with WSL)
- Docker

### Build

Enter the main project folder `http-server-c/` and make all scripts in the `scripts/` folder executable.
```bash
sudo chmod +x scripts/launch_server.sh
sudo chmod +x scripts/load_test.sh
```

### Run

Once again, in the main project folder `http-server-c/`, execute the following script:
```bash
./scripts/launch_server.sh
```
Once successful, you will see output similar to this:
<img width="1107" height="477" alt="image" src="https://github.com/user-attachments/assets/4badcfe2-2180-49dc-a575-f8eb8941ef18" />
Keep in mind this is running on WSL.


Open your browser and go to:
```
http://127.0.0.1:3490/
```
3490 is the default port specified. You can change this via the source files and the Dockerfile. If successful, you will see something like:
<img width="789" height="138" alt="image" src="https://github.com/user-attachments/assets/0cbe602f-526d-4d9c-bcca-e75074fcfb03" />

## Testing
### Single request with curl
```bash
curl http://127.0.0.1:3490/
```

### Multiple concurrent requests with Docker
Load tests were also done via the `load_test.sh` script. Simply execute the file while the server is running in another terminal window to see many concurrent connections to the server.
```bash
./scripts/load_test.sh
```

## Future Improvements
- Add support for POST requests
- Serve multiple HTML files depending on request parameters
- Serve other types of static files (CSS, JS)
- Implement keep-alive connections

## License
This project is open-sourced software licensed under the MIT license.
