# Caching Proxy Server

A lightweight, high-performance HTTP caching proxy server built in C++17. This tool forwards requests to an origin server, caches the responses in memory, and serves cached data for repeated requests to improve speed and reduce origin load.

Built as part of the [roadmap.sh Caching Server](https://roadmap.sh/projects/caching-server) project.

## Features

- **Consolidated Architecture**: Entire server logic in a single, maintainable file.
- **Fast Caching**: Thread-safe in-memory caching with `std::map`.
- **HIT/MISS Tracking**: Injects `X-Cache` headers to indicate if a response was served from cache.
- **Customizable**: Configurable port and origin server via CLI.
- **Zero Dependencies**: Uses standard C++17 and POSIX sockets.

## Requirements

- C++17 compatible compiler (Clang/GCC)
- CMake 3.10+

## Getting Started

### Build

```bash
mkdir -p build && cd build
cmake ..
make
```

### Usage

Start the proxy server on a local port and point it to an origin server:

```bash
./caching-proxy --port 3000 --origin http://dummyjson.com
```

### Options

- `--port <number>`: The port the proxy will listen on.
- `--origin <url>`: The URL of the server to forward requests to.
- `--clear-cache`: Clears the in-memory cache.

## Example

```bash
# First request (MISS)
curl -i http://localhost:3000/products/1
# Response will include X-Cache: MISS

# Second request (HIT)
curl -i http://localhost:3000/products/1
# Response will include X-Cache: HIT
```
