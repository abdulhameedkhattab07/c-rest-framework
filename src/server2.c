#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "types.h"

#define BUFFER_SIZE 1024

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

// Function to get formatted time (Django style)
void get_formatted_time(string buffer, int size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%d/%b/%Y %H:%M:%S]", t);
}

// Function to log requests in Django style
void log_request(const string method, const string path, int status_code) {
    char time_buffer[32];
    get_formatted_time(time_buffer, sizeof(time_buffer));
    printf("%s \"%s %s HTTP/1.1\" %d\n", time_buffer, method, path, status_code);
}

// Function to check if we are in a valid C-Rest project
int is_crest_project(void) {
    return access(".crest", F_OK) == 0;
}

// Function to dynamically extract installed apps from settings.c
void get_installed_apps(char apps[][50], int *app_count) {
    FILE *file = fopen("config/settings.c", "r");
    if (!file) {
        printf("[ERROR] Could not open config/settings.c\n");
        exit(EXIT_FAILURE);
    }

    char line[256];
    *app_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "APP_INSTALLED[")) {
            char app_name[50];
            if (sscanf(line, " APP_INSTALLED[%*d] = \"%49[^\"]", app_name) == 1) {
                strcpy(apps[*app_count], app_name);
                (*app_count)++;
            }
        }
    }

    fclose(file);
}

// Function to log errors
void log_message(const string level, const string message) {
    char timestamp[20];
    time_t now;
    time(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("[%s] %s: %s\n", timestamp, level, message);
}

// Function to check if URL belongs to an installed app
int is_valid_route(const string request_path, char apps[][50], int app_count) {
    for (int i = 0; i < app_count; i++) {
        char app_url[100];
        snprintf(app_url, sizeof(app_url), "/%s", apps[i]);

        if (strcmp(request_path, app_url) == 0) {
            return 1;  // Valid route
        }
    }
    return 0;  // 404 if not found
}

void run_server(int port) {
    if (!is_crest_project()) {
        printf("[ERROR] Not inside a C-Rest project!\n");
        exit(EXIT_FAILURE);
    }

    char apps[10][50];
    int app_count = 0;
    get_installed_apps(apps, &app_count);

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

        // Extract request path
        char method[10], path[256];
        sscanf(buffer, "%s %s", method, path);

        // Ignore favicon.ico
        if (strcmp(path, "/favicon.ico") == 0) {
            close(new_socket);
            continue;
        }

        // Prepare response
        char response_body[BUFFER_SIZE];
        int status_code;

        if (is_valid_route(path, apps, app_count)) {
            // Route exists → 200 OK
            snprintf(response_body, sizeof(response_body), "<h1>Welcome to %s</h1>", path);
            status_code = 200;
        } else {
            // Route does not exist → 404 Not Found
            snprintf(response_body, sizeof(response_body), "<h1>404 Not Found</h1><p>The page you requested does not exist.</p>");
            status_code = 404;
        }

        // Simulate a server error for testing (e.g., accessing "/error")
        if (strcmp(path, "/error") == 0) {
            snprintf(response_body, sizeof(response_body), "<h1>500 Internal Server Error</h1><p>Something went wrong!</p>");
            status_code = 500;
        }

        // Prepare full HTTP response with dynamic status codes
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 %d %s\n"
                 "Content-Type: text/html\n"
                 "Content-Length: %ld\n\n%s",
                 status_code, get_status_message(status_code), strlen(response_body), response_body);

        send(new_socket, response, strlen(response), 0);
        close(new_socket);

        // Log request with correct status
        log_request(method, path, status_code);
    }
}
