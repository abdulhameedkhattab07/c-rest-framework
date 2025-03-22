#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "types.h"

int main(int argc, string argv[]) {
    if (argc < 2) {
        printf("Usage: crest-cli <command> [args]\n");
        return 1;
    }

    if (strcmp(argv[1], "startproject") == 0 && argc == 3) {
        start_project(argv[2]);
    } else if (strcmp(argv[1], "runserver") == 0) {
        run_server();
    } else if (strcmp(argv[1], "makemigrations") == 0) {
        make_migrations();
    } else {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}
