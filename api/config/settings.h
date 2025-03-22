#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>

/*
 * settings.h - Configuration Header File
 *
 * This file declares global project settings for the C-Rest framework.
 */

#define MAX_APPS 20
#define MAX_HOSTS 5

extern const char *APP_INSTALLED[MAX_APPS];
extern int app_count;
extern const char *DATABASE_ENGINE;
extern const char *DATABASE_NAME;
extern const char *DEBUG;
extern const char *SECRET_KEY;
extern const char *ALLOWED_HOSTS[MAX_HOSTS];
extern const char *MIDDLEWARE[];
extern const char *LOG_LEVEL;

#endif
