#include "urls.h"

/*
 * urls.c - Root URL Configuration
 *
 * This file defines the central routing structure for the application.
 */

Route main_routes[] = {
    {"/", home_view},
    {NULL, NULL}
};
