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
    snprintf(command, sizeof(command), "mkdir -p %s/app %s/config %s/migrations", name, name, name);
    system(command);

    // Create main project files
    snprintf(command, sizeof(command), 
        "touch %s/app/main.c %s/app/views.c %s/config/urls.c %s/config/settings.c %s/config/settings.h %s/config/routes.c %s/migrations/models.c", 
        name, name, name, name, name, name, name);
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
        fprintf(settings_h, "/*\n * settings.h - Configuration Header File\n *\n");
        fprintf(settings_h, " * This file declares global project settings for the C-Rest framework.\n */\n\n");
        fprintf(settings_h, "#define MAX_APPS 20\n#define MAX_HOSTS 5\n\n");
        fprintf(settings_h, "extern string APP_INSTALLED[MAX_APPS];\nextern int app_count;\n");
        fprintf(settings_h, "extern const string DATABASE_ENGINE;\nextern const string DATABASE_NAME;\n");
        fprintf(settings_h, "extern const string DEBUG;\nextern const string SECRET_KEY;\n");
        fprintf(settings_h, "extern const string ALLOWED_HOSTS[MAX_HOSTS];\nextern const string MIDDLEWARE[];\n");
        fprintf(settings_h, "extern const string LOG_LEVEL;\n\n#endif\n");
        fclose(settings_h);
    }

    // Write settings.c
    char settings_c_path[512];
    snprintf(settings_c_path, sizeof(settings_c_path), "%s/config/settings.c", name);
    FILE *settings_c = fopen(settings_c_path, "w");
    if (settings_c) {
        fprintf(settings_c, "#include <stdio.h>\n#include \"settings.h\"\n#include \"types.h\"\n\n");
        fprintf(settings_c, "/*\n * settings.c - Project Configuration Source File\n *\n");
        fprintf(settings_c, " * This file defines default settings for the C-Rest framework.\n */\n\n");
        fprintf(settings_c, "string APP_INSTALLED[MAX_APPS] = {};\n\n");
        fprintf(settings_c, "const string DEBUG = \"True\";\nconst string SECRET_KEY = \"your-secret-key\";\n\n");
        fprintf(settings_c, "const string ALLOWED_HOSTS[MAX_HOSTS] = {\"localhost\", \"127.0.0.1\", NULL};\n\n");
        fprintf(settings_c, "const string DATABASE_ENGINE = \"sqlite3\";\nconst string DATABASE_NAME = \"db.sqlite3\";\n\n");
        fprintf(settings_c, "const string MIDDLEWARE[] = {\"SecurityMiddleware\", \"SessionMiddleware\", \"CSRFProtectionMiddleware\", NULL};\n\n");
        fprintf(settings_c, "const string LOG_LEVEL = \"INFO\";\n");
        fclose(settings_c);
    }

    // Write urls.c (Root URL routing)
    char urls_c_path[512];
    snprintf(urls_c_path, sizeof(urls_c_path), "%s/config/urls.c", name);
    FILE *urls_c = fopen(urls_c_path, "w");
    if (urls_c) {
        fprintf(urls_c, "#include \"routes.c\"\n\n");
        fprintf(urls_c, "/*\n * urls.c - Root URL Configuration\n *\n");
        fprintf(urls_c, " * This file defines the central routing structure for the application.\n */\n\n");
        fprintf(urls_c, "Route main_routes[] = {\n    {\"/\", home_view},\n    {NULL, NULL}\n};\n");
        fclose(urls_c);
    }

    // Write views.c (Handles request logic)
    char views_c_path[512];
    snprintf(views_c_path, sizeof(views_c_path), "%s/app/views.c", name);
    FILE *views_c = fopen(views_c_path, "w");
    if (views_c) {
        fprintf(views_c, "#include <stdio.h>\n\n");
        fprintf(views_c, "/*\n * views.c - Request Handling Logic\n *\n");
        fprintf(views_c, " * Define your views (handlers) for different URL paths here.\n */\n\n");
        fprintf(views_c, "void home_view() {\n    printf(\"Home Page\\n\");\n}\n");
        fclose(views_c);
    }

    printf("Project '%s' created successfully with default settings!\n", name);
}

// Function to create a new app in a C-Rest project
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

    // Create sub-app files
    snprintf(command, sizeof(command), "touch app/%s/urls.c app/%s/urls.h", app_name, app_name);
    system(command);
    snprintf(command, sizeof(command), "touch app/%s/views.c app/%s/views.h", app_name, app_name);
    system(command);
    snprintf(command, sizeof(command), "touch app/%s/models.c", app_name);
    system(command);

    // Write views.h (Declaring function prototypes)
    char views_h_path[512];
    snprintf(views_h_path, sizeof(views_h_path), "app/%s/views.h", app_name);
    FILE *views_h = fopen(views_h_path, "w");
    if (views_h) {
        fprintf(views_h, "#ifndef VIEWS_H\n#define VIEWS_H\n\n");
        fprintf(views_h, "#include <stdio.h>\n\n");
        fprintf(views_h, "void home_view(void);\n\n#endif\n");
        fclose(views_h);
    }

    // Write views.c (Implementation)
    char views_c_path[512];
    snprintf(views_c_path, sizeof(views_c_path), "app/%s/views.c", app_name);
    FILE *views_c = fopen(views_c_path, "w");
    if (views_c) {
        fprintf(views_c, "#include \"views.h\"\n\n");
        fprintf(views_c, "void home_view(void) {\n");
        fprintf(views_c, "    printf(\"Welcome to the %s app!\\n\");\n", app_name);
        fprintf(views_c, "}\n");
        fclose(views_c);
    }

    // Write urls.h (Declaring routes)
    char urls_h_path[512];
    snprintf(urls_h_path, sizeof(urls_h_path), "app/%s/urls.h", app_name);
    FILE *urls_h = fopen(urls_h_path, "w");
    if (urls_h) {
        fprintf(urls_h, "#ifndef URLS_H\n#define URLS_H\n\n");
        fprintf(urls_h, "#include \"views.h\"\n\n");
        fprintf(urls_h, "typedef struct {\n");
        fprintf(urls_h, "    const char *path;\n");
        fprintf(urls_h, "    void (*handler)();\n");
        fprintf(urls_h, "} Route;\n\n");
        fprintf(urls_h, "extern Route URLS[];\n\n#endif\n");
        fclose(urls_h);
    }

    // Write urls.c (Registering routes)
    char urls_c_path[512];
    snprintf(urls_c_path, sizeof(urls_c_path), "app/%s/urls.c", app_name);
    FILE *urls_c = fopen(urls_c_path, "w");
    if (urls_c) {
        fprintf(urls_c, "#include \"urls.h\"\n\n");
        fprintf(urls_c, "Route URLS[] = {\n");
        fprintf(urls_c, "    {\"/\", home_view},\n");
        fprintf(urls_c, "    {NULL, NULL}\n};\n");
        fclose(urls_c);
    }

    // Write models.c (Default model)
    char models_c_path[512];
    snprintf(models_c_path, sizeof(models_c_path), "app/%s/models.c", app_name);
    FILE *models_c = fopen(models_c_path, "w");
    if (models_c) {
        fprintf(models_c, "#include <stdio.h>\n\n");
        fprintf(models_c, "void define_models(void) {\n");
        fprintf(models_c, "    printf(\"Defining models for %s...\\n\");\n", app_name);
        fprintf(models_c, "}\n");
        fclose(models_c);
    }

    snprintf(command, sizeof(command), "sed -i '/string APP_INSTALLED\\[MAX_APPS\\] = {/s/}/\"%s\",}/' config/settings.c", app_name);
    system(command);    

    printf("App '%s' created successfully in path '%s'!\n", app_name, project_path);
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
