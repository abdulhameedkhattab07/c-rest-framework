#ifndef ROUTES_H
#define ROUTES_H

#include <stdio.h>

/*
 * routes.h - Route Definitions
 *
 * Defines the Route struct used for URL mapping.
 */

typedef struct {
    const char *path;
    void (*handler)();
} Route;

#endif
