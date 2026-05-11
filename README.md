# Caching Proxy Server

A lightweight, high-performance HTTP caching proxy server built with Modern C++ (C++17). This tool allows you to position a local proxy between a client and an origin server, automatically caching responses to accelerate repeated requests and minimize origin server load.

## Project Usage and Application

The Caching Proxy Server is designed for developers and system administrators who need a simple yet effective way to optimize network traffic and reduce latency for frequently accessed web resources. By sitting in front of an origin server, the proxy captures incoming HTTP requests and checks its local in-memory store for identical prior interactions. If a match is found, the proxy serves the response instantly without reaching out to the primary server, significantly improving application performance. This tool is particularly useful during development to simulate production caching environments, reduce API rate-limiting issues, and perform basic load balancing tests by offloading redundant read requests from the main infrastructure.

---

## Features

- **High Performance**: Serves cached responses in microseconds by bypassing network round-trips to the origin.
- **Minimal Configuration**: Operates as a standalone CLI tool without the need for external configuration files.
- **Simplified Architecture**: The entire core logic is consolidated into a single source file for maximum maintainability.
- **HIT/MISS Tracking**: Injects X-Cache: HIT or X-Cache: MISS headers into every response for clear transparency.
- **Concurrency Support**: Utilizes a thread-per-connection model to handle multiple simultaneous client requests.
- **Zero External Dependencies**: Developed strictly using the C++17 standard library and standard POSIX sockets.

---

## How it Works

The proxy functions as an intermediary. Upon receiving a request:
1. It queries an internal In-Memory Cache for a matching METHOD:PATH key.
2. **HIT**: If the key exists, it returns the cached response with an added X-Cache: HIT header.
3. **MISS**: If the key is absent, it forwards the request to the Origin Server, records the response in the cache, and delivers it to the client with an X-Cache: MISS header.

---

## Installation

### Prerequisites
- **Compiler**: GCC 8+ or Clang 7+
- **Build System**: CMake 3.10+
- **OS**: Linux or macOS

### Build Instructions
```bash
mkdir -p build && cd build
cmake ..
make
```

---

## Usage Instructions

Start the server by specifying the desired local port and the target origin server URL:

```bash
./caching-proxy --port 3000 --origin http://dummyjson.com
```

### CLI Arguments

| Flag | Description |
| :--- | :--- |
| --port <N> | The port the proxy server will listen on locally. |
| --origin <URL> | The URL of the origin server to which requests are forwarded. |
| --clear-cache | Clears all stored responses from the in-memory cache and exits. |

---

## Testing the Proxy

You can verify the caching behavior using curl in your terminal:

```bash
# First Request (Cache MISS)
curl -i http://localhost:3000/products/1

# Second Request (Cache HIT)
curl -i http://localhost:3000/products/1
```

---

## Project Structure

- `src/main.cpp`: Contains the CLI parser, HTTP logic, Socket handling, and Cache store.
- `CMakeLists.txt`: Build configuration.
- `.gitignore`: Ensures the repository remains clean of build artifacts.



---

*Built as part of the roadmap.sh project series.*
https://roadmap.sh/projects/caching-server
