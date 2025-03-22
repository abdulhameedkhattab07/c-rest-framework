#ifndef CLI_H
#define CLI_H

#include "types.h"

// Function Prototypes
void start_project(string name);
void start_app(string name);
void make_migrations(void);
void handle_command(int argc, string argv[]);

#endif
