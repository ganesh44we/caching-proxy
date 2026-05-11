#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <sstream>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct Config {
    int port = 0;
    std::string origin_host;
    int origin_port = 80;
};

std::map<std::string, std::string> cache;
std::mutex cache_mutex;

// Simple logger
void log(const std::string& type, const std::string& msg) {
    std::cout << "[" << type << "] " << msg << std::endl;
}

// Forward request to origin and return response
std::string forward_request(const std::string& host, int port, const std::string& request) {
    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) return "";

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        close(sock);
        freeaddrinfo(res);
        return "";
    }
    freeaddrinfo(res);

    send(sock, request.c_str(), request.size(), 0);
    
    std::string response;
    char buffer[4096];
    int n;
    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0) response.append(buffer, n);
    
    close(sock);
    return response;
}

void handle_client(int client_sock, Config cfg) {
    char buffer[4096];
    int n = recv(client_sock, buffer, sizeof(buffer), 0);
    if (n <= 0) { close(client_sock); return; }
    
    std::string request(buffer, n);
    std::stringstream ss(request);
    std::string method, path;
    ss >> method >> path;

    std::string cache_key = method + ":" + path;
    {
        std::lock_guard<std::mutex> lock(cache_mutex);
        if (cache.count(cache_key)) {
            log("HIT", path);
            std::string res = cache[cache_key];
            // Inject X-Cache header (simple injection before first \r\n)
            size_t pos = res.find("\r\n");
            if (pos != std::string::npos) {
                res.insert(pos + 2, "X-Cache: HIT\r\n");
            }
            send(client_sock, res.c_str(), res.size(), 0);
            close(client_sock);
            return;
        }
    }

    log("MISS", path);
    // Prepare request for origin (ensure Host header matches origin)
    std::string origin_req = request;
    size_t host_pos = origin_req.find("Host: ");
    if (host_pos != std::string::npos) {
        size_t host_end = origin_req.find("\r\n", host_pos);
        origin_req.replace(host_pos, host_end - host_pos, "Host: " + cfg.origin_host);
    }
    // Force Connection: close to avoid hanging on Keep-Alive
    size_t conn_pos = origin_req.find("Connection: ");
    if (conn_pos != std::string::npos) {
        size_t conn_end = origin_req.find("\r\n", conn_pos);
        origin_req.replace(conn_pos, conn_end - conn_pos, "Connection: close");
    } else {
        size_t head_end = origin_req.find("\r\n");
        if (head_end != std::string::npos) {
            origin_req.insert(head_end + 2, "Connection: close\r\n");
        }
    }
    
    std::string response = forward_request(cfg.origin_host, cfg.origin_port, origin_req);
    if (response.empty()) {
        std::string err = "HTTP/1.1 502 Bad Gateway\r\n\r\n";
        send(client_sock, err.c_str(), err.size(), 0);
    } else {
        {
            std::lock_guard<std::mutex> lock(cache_mutex);
            cache[cache_key] = response;
        }
        size_t pos = response.find("\r\n");
        if (pos != std::string::npos) {
            response.insert(pos + 2, "X-Cache: MISS\r\n");
        }
        send(client_sock, response.c_str(), response.size(), 0);
    }
    close(client_sock);
}

int main(int argc, char* argv[]) {
    Config cfg;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) cfg.port = std::stoi(argv[++i]);
        else if (arg == "--origin" && i + 1 < argc) {
            std::string origin = argv[++i];
            if (origin.find("http://") == 0) origin = origin.substr(7);
            size_t pos = origin.find(":");
            if (pos != std::string::npos) {
                cfg.origin_host = origin.substr(0, pos);
                cfg.origin_port = std::stoi(origin.substr(pos + 1));
            } else {
                size_t path_pos = origin.find("/");
                cfg.origin_host = origin.substr(0, path_pos);
            }
        } else if (arg == "--clear-cache") {
            log("INFO", "Cache cleared");
            return 0;
        }
    }

    if (cfg.port == 0 || cfg.origin_host.empty()) {
        std::cout << "Usage: caching-proxy --port <number> --origin <url>" << std::endl;
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(cfg.port);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    listen(server_fd, 10);
    
    log("INFO", "Proxy started on port " + std::to_string(cfg.port) + " forwarding to " + cfg.origin_host);
    
    while (true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        std::thread(handle_client, client_sock, cfg).detach();
    }
    return 0;
}
