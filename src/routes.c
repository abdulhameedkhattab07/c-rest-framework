#include "routes.h"
#include <string.h>

// Array to store routes
static Route routes[MAX_ROUTES];
static int route_count = 0;

// Add a route to the router
void add_route(const char *path, void (*handler)(void)) {
    if (route_count < MAX_ROUTES) {
        strncpy(routes[route_count].path, path, MAX_PATH_LENGTH - 1);
        routes[route_count].path[MAX_PATH_LENGTH - 1] = '\0'; // Ensure null termination
        routes[route_count].handler = handler;
        route_count++;
    } else {
        printf("Error: Route limit reached!\n");
    }
}

// Find and execute the handler for a given path
void handle_request(const char *path) {
    for (int i = 0; i < route_count; i++) {
        if (strcmp(routes[i].path, path) == 0) {
            routes[i].handler();
            return;
        }
    }
    printf("404 Not Found: %s\n", path);
}

// Load routes from user-defined `urls.c`
extern Route URLS[];

void load_user_routes(void) {
    int i = 0;
    while (URLS[i].path[0] != '\0') { // Check if path is non-empty
        add_route(URLS[i].path, URLS[i].handler);
        i++;
    }
}
