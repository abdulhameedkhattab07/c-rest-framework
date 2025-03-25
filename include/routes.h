#ifndef ROUTES_H
#define ROUTES_H

#include <stdio.h>

#define MAX_PATH_LENGTH 256
#define MAX_ROUTES 100

// Define Route structure
typedef struct {
    char path[MAX_PATH_LENGTH]; // Allocate memory for the path
    void (*handler)(void);
} Route;

// Function prototypes
void add_route(const char *path, void (*handler)(void));
void handle_request(const char *path);
void load_user_routes(void);

#endif // ROUTES_H
