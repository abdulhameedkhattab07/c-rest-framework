#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "types.h"
#include "server.h"
#include "cli.h"

void handle_command(int argc, string argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./bin/crest-cli <command> [options]\n");
        return;
    }

    if (strcmp(argv[1], "startproject") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: ./bin/crest-cli startproject <project_name>\n");
            return;
        }
        start_project(argv[2]);
    }
    else if (strcmp(argv[1], "startapp") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: crest-cli startapp <app_name>\n");
            return;
        }
        start_app(argv[2]);
    }
    else if (strcmp(argv[1], "runserver") == 0)
    {
        int port = 8080;
        if (argv[2])
        {
            port = atoi(argv[2]);
        }
        run_server(port);
        return;
    }
    else if (strcmp(argv[1], "migrate") == 0)
    {
        make_migrations();
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
    }
}

void start_project(const string name) {
    char command[512];

    // Create project directories
    snprintf(command, sizeof(command), "mkdir -p %s/app %s/config %s/migrations %s/server", name, name, name, name);
    system(command);

    // Create core project files
    snprintf(command, sizeof(command),
             "touch %s/app/views.c %s/app/views.h %s/config/routes.c %s/config/settings.c %s/config/settings.h %s/server/server.c",
             name, name, name, name, name, name);
    system(command);

    // Create a hidden file to mark this as a C-Rest project
    snprintf(command, sizeof(command), "touch %s/.crest", name);
    system(command);

    // Write settings.h
    char settings_h_path[512];
    snprintf(settings_h_path, sizeof(settings_h_path), "%s/config/settings.h", name);
    FILE *settings_h = fopen(settings_h_path, "w");
    if (settings_h) {
        fprintf(settings_h, "#ifndef SETTINGS_H\n#define SETTINGS_H\n\n");
        fprintf(settings_h, "#define MAX_APPS 20\n#define MAX_HOSTS 5\n\n");
        fprintf(settings_h, "extern const string DATABASE_ENGINE;\nextern const string DATABASE_NAME;\n");
        fprintf(settings_h, "extern const string ALLOWED_HOSTS[MAX_HOSTS];\n\n#endif\n");
        fclose(settings_h);
    }

    // Write settings.c
    char settings_c_path[512];
    snprintf(settings_c_path, sizeof(settings_c_path), "%s/config/settings.c", name);
    FILE *settings_c = fopen(settings_c_path, "w");
    if (settings_c) {
        fprintf(settings_c, "#include \"settings.h\"\n\n");
        fprintf(settings_c, "const string DATABASE_ENGINE = \"sqlite3\";\nconst string DATABASE_NAME = \"db.sqlite3\";\n");
        fprintf(settings_c, "const string ALLOWED_HOSTS[MAX_HOSTS] = {\"localhost\", \"127.0.0.1\", NULL};\n");
        fclose(settings_c);
    }

    // Write routes.c
    char routes_c_path[512];
    snprintf(routes_c_path, sizeof(routes_c_path), "%s/config/routes.c", name);
    FILE *routes_c = fopen(routes_c_path, "w");
    if (routes_c) {
        fprintf(routes_c, "#include \"../app/views.h\"\n\n");
        fprintf(routes_c, "typedef struct {\n    const string path;\n    void (*handler)();\n} Route;\n\n");
        fprintf(routes_c, "Route ROUTES[] = {\n    {\"/\", home_view},\n    {NULL, NULL}\n};\n");
        fclose(routes_c);
    }

    // Write views.h
    char views_h_path[512];
    snprintf(views_h_path, sizeof(views_h_path), "%s/app/views.h", name);
    FILE *views_h = fopen(views_h_path, "w");
    if (views_h) {
        fprintf(views_h, "#ifndef VIEWS_H\n#define VIEWS_H\n\nvoid home_view();\n\n#endif\n");
        fclose(views_h);
    }

    // Write views.c
    char views_c_path[512];
    snprintf(views_c_path, sizeof(views_c_path), "%s/app/views.c", name);
    FILE *views_c = fopen(views_c_path, "w");
    if (views_c) {
        fprintf(views_c, "#include <stdio.h>\n#include \"views.h\"\n\n");
        fprintf(views_c, "void home_view() {\n    printf(\"Home Page\\n\");\n}\n");
        fclose(views_c);
    }

    // Write server.c
    char server_c_path[512];
    snprintf(server_c_path, sizeof(server_c_path), "%s/server/server.c", name);
    FILE *server_c = fopen(server_c_path, "w");
    if (server_c) {
        fprintf(server_c, "#include <stdio.h>\n#include \"../config/routes.c\"\n\n");
        fprintf(server_c, "int main() {\n");
        fprintf(server_c, "    printf(\"Starting C-Rest server...\\n\");\n");
        fprintf(server_c, "    printf(\"Available routes:\\n\");\n");
        fprintf(server_c, "    for (int i = 0; ROUTES[i].path != NULL; i++) {\n");
        fprintf(server_c, "        printf(\"- %%s\\n\", ROUTES[i].path);\n    }\n");
        fprintf(server_c, "    return 0;\n}\n");
        fclose(server_c);
    }

    printf("Project '%s' created successfully!\n", name);
}

// Function to create a new app inside a C-Rest project
void start_app(const string app_name) {
    char project_path[256];

    // Get the current directory (assumes it's the project folder)
    if (getcwd(project_path, sizeof(project_path)) == NULL) {
        printf("Error: Unable to determine current directory.\n");
        return;
    }

    // Check if we are inside a valid C-Rest project
    char crest_file[512];
    snprintf(crest_file, sizeof(crest_file), "%s/.crest", project_path);
    if (access(crest_file, F_OK) != 0) {
        printf("Error: Not inside a valid C-Rest project!\n");
        return;
    }

    char command[512];

    // Create the app directory
    snprintf(command, sizeof(command), "mkdir -p app/%s", app_name);
    system(command);

    // Create app files
    snprintf(command, sizeof(command), "touch app/%s/urls.c app/%s/urls.h app/%s/views.c app/%s/views.h", app_name, app_name, app_name, app_name);
    system(command);

    // Write views.h
    char views_h_path[512];
    snprintf(views_h_path, sizeof(views_h_path), "app/%s/views.h", app_name);
    FILE *views_h = fopen(views_h_path, "w");
    if (views_h) {
        fprintf(views_h, "#ifndef %s_VIEWS_H\n#define %s_VIEWS_H\n\nvoid %s_view();\n\n#endif\n", app_name, app_name, app_name);
        fclose(views_h);
    }

    // Write views.c
    char views_c_path[512];
    snprintf(views_c_path, sizeof(views_c_path), "app/%s/views.c", app_name);
    FILE *views_c = fopen(views_c_path, "w");
    if (views_c) {
        fprintf(views_c, "#include <stdio.h>\n#include \"views.h\"\n\n");
        fprintf(views_c, "void %s_view() {\n    printf(\"Welcome to %s app!\\n\");\n}\n", app_name, app_name);
        fclose(views_c);
    }

    // Write urls.c
    char urls_c_path[512];
    snprintf(urls_c_path, sizeof(urls_c_path), "app/%s/urls.c", app_name);
    FILE *urls_c = fopen(urls_c_path, "w");
    if (urls_c) {
        fprintf(urls_c, "#include \"views.h\"\n\n");
        fprintf(urls_c, "Route %s_routes[] = {\n    {\"/%s\", %s_view},\n    {NULL, NULL}\n};\n", app_name, app_name, app_name);
        fclose(urls_c);
    }

    printf("App '%s' created successfully inside the project!\n", app_name);
}

void make_migrations(void) {
    printf("Running migrations...\n");

    FILE *file = fopen("config/settings.c", "r");
    if (!file) {
        printf("Error: Could not find config/settings.c. Are you inside a project?\n");
        return;
    }

    // Ensure a database file exists
    system("touch db.sqlite3");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "APP_INSTALLED += [")) { 
            printf("Applying migrations for: %s", line);
        }
    }

    fclose(file);
    printf("Migrations completed successfully!\n");
}


int main(int argc, string argv[])
{
    handle_command(argc, argv);
    return 0;
}
