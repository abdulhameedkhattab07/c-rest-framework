#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "types.h"
#include "routes.h"
#include "server.h"

#define BUFFER_SIZE 1024

// Function to check if we are in a valid C-Rest project
int is_crest_project(void) {
    return access(".crest", F_OK) == 0;
}

// HTTP Status Codes Mapping
typedef struct {
    int code;
    const string message;
} HttpStatus;

HttpStatus STATUS_CODES[] = {
    {200, "OK"},
    {404, "Not Found"},
    {500, "Internal Server Error"},
    {0, NULL}  // Sentinel value
};

// Function to get status message from code
const char* get_status_message(int code) {
    for (int i = 0; STATUS_CODES[i].code != 0; i++) {
        if (STATUS_CODES[i].code == code) {
            return STATUS_CODES[i].message;
        }
    }
    return "Unknown Status";
}

// Define the actual routes array with a fixed size
Route ROUTES[100];  // Can hold up to 100 routes
size_t NUM_ROUTES = 0;  // Track the number of registered routes

// Function to add routes dynamically
void add_route(const char *path, void (*handler)(void)) {
    if (NUM_ROUTES < 100) {  // Ensure we don't exceed array bounds
        ROUTES[NUM_ROUTES].path = path;
        ROUTES[NUM_ROUTES].handler = handler;
        NUM_ROUTES++;
    }
}

// Function to get formatted time (Django style)
void get_formatted_time(char *buffer, int size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%d/%b/%Y %H:%M:%S]", t);
}

// Function to log requests in Django style
void log_request(const char *method, const char *path, int status_code) {
    char time_buffer[32];
    get_formatted_time(time_buffer, sizeof(time_buffer));
    printf("%s \"%s %s HTTP/1.1\" %d\n", time_buffer, method, path, status_code);
}

// Function to log errors
void log_message(const char *level, const char *message) {
    char timestamp[20];
    time_t now;
    time(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("[%s] %s: %s\n", timestamp, level, message);
}

void run_server(int port) {
    if (!is_crest_project()) {
        printf("[ERROR] Not inside a C-Rest project!\n");
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        log_message("ERROR", "Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    // Bind to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        log_message("ERROR", "Port binding failed.");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        log_message("ERROR", "Listening on port failed.");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server running at http://localhost:%d/\n", port);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            log_message("ERROR", "Failed to accept connection.");
            continue;
        }

        read(new_socket, buffer, BUFFER_SIZE);

        // Extract request method and path
        char method[10], path[256];
        sscanf(buffer, "%s %s", method, path);

        // Ignore favicon.ico requests
        if (strcmp(path, "/favicon.ico") == 0) {
            close(new_socket);
            continue;
        }

        // Find matching route
        Route *matched_route = find_route(method, path);

        char response_body[BUFFER_SIZE];
        int status_code;

        if (matched_route) {
            // If a route exists, execute the route handler
            matched_route->handler();
            status_code = 200;
        } else {
            // Route does not exist → 404 Not Found
            snprintf(response_body, sizeof(response_body),
                     "<h1>404 Not Found</h1><p>The page you requested does not exist.</p>");
            status_code = 404;
        }

        // Prepare full HTTP response
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 %d OK\n"
                 "Content-Type: text/html\n"
                 "Content-Length: %ld\n\n%s",
                 status_code, strlen(response_body), response_body);

        send(new_socket, response, strlen(response), 0);
        close(new_socket);

        // Log the request
        log_request(method, path, status_code);
    }
}
