// Real HTTP Web Server with OpenMP Task Management
// Build: gcc -O2 -fopenmp webserver.c -o webserver
// Run: OMP_NUM_THREADS=4 ./webserver [port]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <omp.h>

#define MAX_CONN 128
#define BUFFER_SIZE 4096
#define DEFAULT_PORT 8080

// Global connection management arrays
static int conn_id[MAX_CONN];        // Connection IDs
static int conn_alive[MAX_CONN];     // Connection status: 1=alive, 0=stopping
static int conn_in_use[MAX_CONN];    // Slot allocation status
static int next_id = 1;              // Next connection ID
static int server_socket = -1;       // Main server socket
static volatile int server_running = 1; // Server shutdown flag

// HTTP response templates
static const char* HTTP_200_OK = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Connection: close\r\n"
    "\r\n";

static const char* HTTP_404_NOT_FOUND = 
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Connection: close\r\n"
    "\r\n";

static const char* HTTP_500_ERROR = 
    "HTTP/1.1 500 Internal Server Error\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Connection: close\r\n"
    "\r\n";

// Signal handler for graceful shutdown
static void signal_handler(int sig) {
    printf("\n[server] Received signal %d, shutting down...\n", sig);
    server_running = 0;
    if (server_socket != -1) {
        close(server_socket);
    }
}

// Simulate some processing work
static void simulate_work(int cycles) {
    volatile int s = 0;
    for (int i = 0; i < cycles; ++i) s += i;
}

// Generate HTML response based on request path
static void generate_response(const char* path, char* response, size_t max_size) {
    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        snprintf(response, max_size,
            "<!DOCTYPE html>\n"
            "<html><head><title>OpenMP Web Server</title></head>\n"
            "<body>\n"
            "<h1>Welcome to OpenMP Web Server</h1>\n"
            "<p>Server time: %ld</p>\n"
            "<p>Active threads: %d</p>\n"
            "<p>Max threads: %d</p>\n"
            "<ul>\n"
            "<li><a href=\"/status\">Server Status</a></li>\n"
            "<li><a href=\"/connections\">Active Connections</a></li>\n"
            "<li><a href=\"/test\">Test Page</a></li>\n"
            "</ul>\n"
            "</body></html>\n",
            time(NULL), omp_get_num_threads(), omp_get_max_threads());
    } else if (strcmp(path, "/status") == 0) {
        int active_conns = 0;
        // Count active connections using atomic operations for thread safety
        for (int i = 0; i < MAX_CONN; ++i) {
            int used, alive;
            #pragma omp atomic read
            used = conn_in_use[i];
            if (used) {
                #pragma omp atomic read
                alive = conn_alive[i];
                if (alive) active_conns++;
            }
        }
        
        snprintf(response, max_size,
            "<!DOCTYPE html>\n"
            "<html><head><title>Server Status</title></head>\n"
            "<body>\n"
            "<h1>Server Status</h1>\n"
            "<p>Server running: %s</p>\n"
            "<p>Active connections: %d</p>\n"
            "<p>Max connections: %d</p>\n"
            "<p>OpenMP threads: %d/%d</p>\n"
            "<p>Server time: %ld</p>\n"
            "<a href=\"/\">Back to Home</a>\n"
            "</body></html>\n",
            server_running ? "Yes" : "No", active_conns, MAX_CONN,
            omp_get_num_threads(), omp_get_max_threads(), time(NULL));
    } else if (strcmp(path, "/connections") == 0) {
        char conn_list[2048] = "";
        for (int i = 0; i < MAX_CONN; ++i) {
            int used, alive, id;
            #pragma omp atomic read
            used = conn_in_use[i];
            if (used) {
                #pragma omp atomic read
                id = conn_id[i];
                #pragma omp atomic read
                alive = conn_alive[i];
                char temp[64];
                snprintf(temp, sizeof(temp), "<li>Connection %d (slot %d) - %s</li>\n", 
                        id, i, alive ? "Active" : "Stopping");
                strcat(conn_list, temp);
            }
        }
        
        snprintf(response, max_size,
            "<!DOCTYPE html>\n"
            "<html><head><title>Active Connections</title></head>\n"
            "<body>\n"
            "<h1>Active Connections</h1>\n"
            "<ul>%s</ul>\n"
            "<a href=\"/\">Back to Home</a>\n"
            "</body></html>\n", conn_list);
    } else if (strcmp(path, "/test") == 0) {
        // Simulate some processing work
        simulate_work(1000000);
        snprintf(response, max_size,
            "<!DOCTYPE html>\n"
            "<html><head><title>Test Page</title></head>\n"
            "<body>\n"
            "<h1>Test Page</h1>\n"
            "<p>This page simulates some processing work.</p>\n"
            "<p>Processing completed at: %ld</p>\n"
            "<a href=\"/\">Back to Home</a>\n"
            "</body></html>\n", time(NULL));
    } else {
        // 404 Not Found
        snprintf(response, max_size,
            "<!DOCTYPE html>\n"
            "<html><head><title>404 Not Found</title></head>\n"
            "<body>\n"
            "<h1>404 - Page Not Found</h1>\n"
            "<p>The requested page '%s' was not found.</p>\n"
            "<a href=\"/\">Back to Home</a>\n"
            "</body></html>\n", path);
    }
}

// Handle HTTP request and send response
static void handle_http_request(int client_socket, int conn_id, int slot_idx) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE * 2];
    char http_response[BUFFER_SIZE * 3];
    
    printf("[conn %02d] Handling request (slot %d, tid %d)\n", 
           conn_id, slot_idx, omp_get_thread_num());
    
    // Read HTTP request
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        printf("[conn %02d] Failed to read request\n", conn_id);
        return;
    }
    
    buffer[bytes_read] = '\0';
    
    // Parse HTTP request to extract path
    char* method = strtok(buffer, " ");
    char* path = strtok(NULL, " ");
    
    if (!method || !path) {
        printf("[conn %02d] Invalid HTTP request\n", conn_id);
        return;
    }
    
    printf("[conn %02d] %s %s\n", conn_id, method, path);
    
    // Generate response content
    generate_response(path, response, sizeof(response));
    
    // Determine HTTP status and build full response
    const char* status_line;
    if (strstr(path, "/") == path) {  // Valid path
        status_line = HTTP_200_OK;
    } else {
        status_line = HTTP_404_NOT_FOUND;
    }
    
    // Build complete HTTP response
    snprintf(http_response, sizeof(http_response), "%s%s", status_line, response);
    
    // Send response
    ssize_t bytes_sent = send(client_socket, http_response, strlen(http_response), 0);
    if (bytes_sent < 0) {
        printf("[conn %02d] Failed to send response\n", conn_id);
    } else {
        printf("[conn %02d] Response sent (%zd bytes)\n", conn_id, bytes_sent);
    }
}

// Main connection handler - runs as OpenMP task
static void handle_connection(int client_socket, int slot_idx, int conn_id) {
    printf("[conn %02d] Connection started (slot %d, tid %d)\n", 
           conn_id, slot_idx, omp_get_thread_num());
    
    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 30;  // 30 second timeout
    timeout.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    // Handle the HTTP request
    handle_http_request(client_socket, conn_id, slot_idx);
    
    // Close connection
    close(client_socket);
    
    // Mark slot as free using atomic operation for thread safety
    #pragma omp atomic write
    conn_in_use[slot_idx] = 0;
    
    printf("[conn %02d] Connection closed\n", conn_id);
}

// Find free connection slot using atomic operations
static int find_free_slot(void) {
    for (int i = 0; i < MAX_CONN; ++i) {
        int used;
        #pragma omp atomic read
        used = conn_in_use[i];
        if (!used) return i;
    }
    return -1;
}

// Setup server socket
static int setup_server_socket(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    // Allow address reuse
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }
    
    if (listen(sock, 10) < 0) {
        perror("listen");
        close(sock);
        return -1;
    }
    
    return sock;
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    
    // Parse command line arguments
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number: %s\n", argv[1]);
            return 1;
        }
    }
    
    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Configure OpenMP for nested parallelism
    // NOTE: omp_set_dynamic(0) disables dynamic thread adjustment
    // This ensures we get exactly the number of threads we request
    omp_set_dynamic(0);
    
    // NOTE: omp_set_max_active_levels(2) allows nested parallel regions
    // This enables the server to use nested parallelism if needed
    omp_set_max_active_levels(2);
    
    // Initialize connection arrays
    memset(conn_id, 0, sizeof(conn_id));
    memset(conn_alive, 0, sizeof(conn_alive));
    memset(conn_in_use, 0, sizeof(conn_in_use));
    
    // Setup server socket
    server_socket = setup_server_socket(port);
    if (server_socket < 0) {
        return 1;
    }
    
    printf("OpenMP Web Server starting on port %d\n", port);
    printf("Threads: %d (max: %d), Max connections: %d\n", 
           omp_get_num_threads(), omp_get_max_threads(), MAX_CONN);
    printf("Press Ctrl+C to stop the server\n\n");
    
    // NOTE: Main parallel region - creates a team of threads
    // One thread will handle the main accept loop (single directive)
    // Other threads will execute connection tasks
    #pragma omp parallel
    {
        // NOTE: single directive ensures only one thread runs the accept loop
        // This prevents multiple threads from trying to accept connections simultaneously
        #pragma omp single
        {
            while (server_running) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                
                // Accept new connection
                int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
                if (client_socket < 0) {
                    if (server_running) {
                        perror("accept");
                    }
                    continue;
                }
                
                // Find free slot for this connection
                int slot = find_free_slot();
                if (slot < 0) {
                    printf("No free connection slots, rejecting connection\n");
                    close(client_socket);
                    continue;
                }
                
                // Allocate connection ID and mark slot as used
                int id = next_id++;
                
                // NOTE: Atomic operations ensure thread-safe updates to shared arrays
                // Multiple threads may be accessing these arrays simultaneously
                #pragma omp atomic write
                conn_in_use[slot] = 1;
                #pragma omp atomic write
                conn_id[slot] = id;
                #pragma omp atomic write
                conn_alive[slot] = 1;
                
                printf("Accepted connection %02d from %s:%d (slot %d)\n", 
                       id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), slot);
                
                // NOTE: task directive creates an asynchronous task
                // This allows the connection to be handled by any available thread
                // while the main thread continues accepting new connections
                #pragma omp task firstprivate(client_socket, slot, id) shared(conn_in_use)
                {
                    handle_connection(client_socket, slot, id);
                }
            }
        }
        
        // NOTE: taskwait ensures all tasks complete before the parallel region ends
        // This prevents the server from shutting down while connections are still active
        #pragma omp taskwait
    }
    
    // Cleanup
    if (server_socket != -1) {
        close(server_socket);
    }
    
    printf("Server shutdown complete\n");
    return 0;
}