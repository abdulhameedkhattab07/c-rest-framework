#include "settings.h"

/*
 * settings.c - Project Configuration Source File
 *
 * This file defines default settings for the C-Rest framework.
 */

const char *APP_INSTALLED[MAX_APPS] = {NULL};
int app_count = 0;

const char *DEBUG = "True";
const char *SECRET_KEY = "your-secret-key";

const char *ALLOWED_HOSTS[MAX_HOSTS] = {"localhost", "127.0.0.1", NULL};

const char *DATABASE_ENGINE = "sqlite3";
const char *DATABASE_NAME = "db.sqlite3";

const char *MIDDLEWARE[] = {"SecurityMiddleware", "SessionMiddleware", "CSRFProtectionMiddleware", NULL};

const char *LOG_LEVEL = "INFO";
