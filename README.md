# 🚀 Caching Proxy Server

A lightweight, high-performance HTTP caching proxy server built with **Modern C++ (C++17)**. This tool allows you to sit a local proxy between your client and an origin server, automatically caching responses to speed up repeated requests and reduce origin bandwidth.

---

## 🌟 Features

- **⚡ Blazing Fast**: Consistently serves cached responses in microseconds.
- **🛠️ Zero Config**: Run with a single command; no complex configuration files needed.
- **🏗️ Simplified Architecture**: The entire core logic is consolidated into a single, optimized `main.cpp` for extreme maintainability.
- **🛰️ HIT/MISS Tracking**: Automatically injects `X-Cache: HIT` or `X-Cache: MISS` headers into every response.
- **🧵 Concurrent**: Handles multiple client connections simultaneously using a thread-per-connection model.
- **📦 Zero Dependencies**: Built strictly using the C++17 standard library and POSIX sockets.

---

## 🛠️ How it Works

The proxy acts as a middleman. When it receives a request:
1. It checks its internal **In-Memory Cache** for a matching `METHOD:PATH` key.
2. **HIT**: If found, it instantly returns the cached response with an added `X-Cache: HIT` header.
3. **MISS**: If not found, it forwards the request to the **Origin Server**, captures the response, saves it to the cache, and returns it to the client with `X-Cache: MISS`.

---

## ⚙️ Installation

### Prerequisites
- **Compiler**: GCC 8+ or Clang 7+
- **Build System**: CMake 3.10+
- **OS**: Linux or macOS

### Build Instructions
```bash
# Clone the repository (if applicable)
# git clone https://github.com/YOUR_USERNAME/caching-proxy.git
# cd caching-proxy

# Create build directory and compile
mkdir -p build && cd build
cmake ..
make
```

---

## 🚀 Usage

Start the server by specifying your local port and the target origin server:

```bash
./caching-proxy --port 3000 --origin http://dummyjson.com
```

### CLI Arguments

| Flag | Description |
| :--- | :--- |
| `--port <N>` | The port the proxy server will listen on locally. |
| `--origin <URL>` | The URL of the origin server to which requests are forwarded. |
| `--clear-cache` | Clears all stored responses from the in-memory cache and exits. |

---

## 🧪 Testing the Proxy

Open a terminal and use `curl` to see the caching in action:

```bash
# First Request (Cache MISS)
curl -i http://localhost:3000/products/1

# Expected Response Headers:
# HTTP/1.1 200 OK
# X-Cache: MISS
# ...

# Second Request (Cache HIT)
curl -i http://localhost:3000/products/1

# Expected Response Headers:
# HTTP/1.1 200 OK
# X-Cache: HIT
# ...
```

---

## 📁 Project Structure

- `src/main.cpp`: The "Heart" of the project. Contains the CLI parser, HTTP logic, Socket handling, and Cache store.
- `CMakeLists.txt`: Simple build configuration.
- `.gitignore`: Keeps your repository clean from build artifacts.

---

## 📜 License

This project is open-source and available under the [MIT License](LICENSE).

---

*Built as part of the [roadmap.sh](https://roadmap.sh/projects/caching-server) project series.*
