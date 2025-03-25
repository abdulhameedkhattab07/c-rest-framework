#ifndef SERVER_H
#define SERVER_H

#include "types.h"
#include "routes.h"

// Function to add user-defined routes
void add_route(const char *path, void (*handler)(void));


void run_server(int port);  // Accept port as an argument
Route *find_route(const char *method, const char *path);

#endif
