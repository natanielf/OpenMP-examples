// HTTP Client for testing the OpenMP Web Server
// Build: gcc -O2 client.c -o client
// Run: ./client [host] [port] [path]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 4096
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8080
#define DEFAULT_PATH "/"

// Send HTTP request and receive response
static int send_request(const char* host, int port, const char* path) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    // Setup server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address: %s\n", host);
        close(sock);
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
    
    // Build HTTP request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "User-Agent: OpenMP-Client/1.0\r\n"
        "Connection: close\r\n"
        "\r\n", path, host, port);
    
    // Send request
    ssize_t bytes_sent = send(sock, request, strlen(request), 0);
    if (bytes_sent < 0) {
        perror("send");
        close(sock);
        return -1;
    }
    
    printf("Request sent (%zd bytes):\n%s\n", bytes_sent, request);
    
    // Receive response
    char response[BUFFER_SIZE];
    ssize_t total_bytes = 0;
    
    printf("Response received:\n");
    printf("==================\n");
    
    while (1) {
        ssize_t bytes_received = recv(sock, response, sizeof(response) - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        
        response[bytes_received] = '\0';
        printf("%s", response);
        total_bytes += bytes_received;
    }
    
    printf("==================\n");
    printf("Total response size: %zd bytes\n", total_bytes);
    
    close(sock);
    return 0;
}

// Interactive client mode
static void interactive_mode(const char* host, int port) {
    char line[256];
    char path[256];
    
    printf("Interactive HTTP Client\n");
    printf("Connected to %s:%d\n", host, port);
    printf("Enter paths to request (or 'quit' to exit):\n");
    printf("Examples: /, /status, /connections, /test\n\n");
    
    while (1) {
        printf("path> ");
        fflush(stdout);
        
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        
        // Remove newline
        line[strcspn(line, "\r\n")] = 0;
        
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0 || strcmp(line, "q") == 0) {
            break;
        }
        
        if (strlen(line) == 0) {
            strcpy(path, "/");
        } else {
            strncpy(path, line, sizeof(path) - 1);
            path[sizeof(path) - 1] = '\0';
        }
        
        printf("\n--- Requesting: %s ---\n", path);
        if (send_request(host, port, path) < 0) {
            printf("Request failed\n");
        }
        printf("\n");
    }
}

// Stress test mode - send multiple concurrent requests
static void stress_test(const char* host, int port, int num_requests) {
    printf("Stress test: sending %d requests to %s:%d\n", num_requests, host, port);
    
    const char* paths[] = {"/", "/status", "/connections", "/test"};
    int num_paths = sizeof(paths) / sizeof(paths[0]);
    
    for (int i = 0; i < num_requests; ++i) {
        const char* path = paths[i % num_paths];
        printf("\n--- Request %d/%d: %s ---\n", i + 1, num_requests, path);
        
        if (send_request(host, port, path) < 0) {
            printf("Request %d failed\n", i + 1);
        }
        
        // Small delay between requests
        usleep(100000); // 100ms
    }
    
    printf("\nStress test completed\n");
}

int main(int argc, char* argv[]) {
    const char* host = DEFAULT_HOST;
    int port = DEFAULT_PORT;
    const char* path = DEFAULT_PATH;
    int interactive = 0;
    int stress = 0;
    int num_requests = 10;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options] [host] [port] [path]\n", argv[0]);
            printf("Options:\n");
            printf("  -i, --interactive    Interactive mode\n");
            printf("  -s, --stress [num]   Stress test with num requests (default: 10)\n");
            printf("  -h, --help          Show this help\n");
            printf("\nExamples:\n");
            printf("  %s                           # Connect to localhost:8080/\n", argv[0]);
            printf("  %s localhost 8080 /status    # Get server status\n", argv[0]);
            printf("  %s -i                        # Interactive mode\n", argv[0]);
            printf("  %s -s 20                     # Stress test with 20 requests\n", argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
            interactive = 1;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stress") == 0) {
            stress = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                num_requests = atoi(argv[++i]);
                if (num_requests <= 0) num_requests = 10;
            }
        } else if (argv[i][0] != '-') {
            // Positional arguments: host, port, path
            if (strcmp(host, DEFAULT_HOST) == 0) {
                host = argv[i];
            } else if (port == DEFAULT_PORT) {
                port = atoi(argv[i]);
                if (port <= 0 || port > 65535) {
                    fprintf(stderr, "Invalid port: %s\n", argv[i]);
                    return 1;
                }
            } else {
                path = argv[i];
            }
        }
    }
    
    printf("HTTP Client for OpenMP Web Server\n");
    printf("Target: %s:%d%s\n\n", host, port, path);
    
    if (interactive) {
        interactive_mode(host, port);
    } else if (stress) {
        stress_test(host, port, num_requests);
    } else {
        // Single request
        if (send_request(host, port, path) < 0) {
            return 1;
        }
    }
    
    return 0;
}
