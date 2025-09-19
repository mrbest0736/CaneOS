// CaneOS App Store
#include <stdio.h>


#define MAX_APPS 128
struct App {
    char name[64];
    char version[16];
    int installed;
    int reviews;
};
struct App apps[MAX_APPS];
int app_count = 0;

void appstore_init() {
    app_count = 0;
    printf("App Store initialized\n");
}

int appstore_add_app(const char* name, const char* version) {
    if (app_count >= MAX_APPS) return -1;
    snprintf(apps[app_count].name, 63, "%s", name);
    snprintf(apps[app_count].version, 15, "%s", version);
    apps[app_count].installed = 0;
    apps[app_count].reviews = 0;
    app_count++;
    printf("App added: %s %s\n", name, version);
    return app_count - 1;
}

int appstore_install_app(const char* name) {
    for (int i = 0; i < app_count; i++) {
        if (strcmp(apps[i].name, name) == 0) {
            apps[i].installed = 1;
            printf("App installed: %s\n", name);
            return 0;
        }
    }
    printf("App not found: %s\n", name);
    return -1;
}

int appstore_update_app(const char* name, const char* version) {
    for (int i = 0; i < app_count; i++) {
        if (strcmp(apps[i].name, name) == 0) {
            snprintf(apps[i].version, 15, "%s", version);
            printf("App updated: %s %s\n", name, version);
            return 0;
        }
    }
    printf("App not found: %s\n", name);
    return -1;
}

int appstore_review_app(const char* name) {
    for (int i = 0; i < app_count; i++) {
        if (strcmp(apps[i].name, name) == 0) {
            apps[i].reviews++;
            printf("App reviewed: %s (total reviews: %d)\n", name, apps[i].reviews);
            return 0;
        }
    }
    printf("App not found: %s\n", name);
    return -1;
}

void appstore_list_apps() {
    printf("Apps:\n");
    for (int i = 0; i < app_count; i++) {
        printf("%s %s installed=%d reviews=%d\n", apps[i].name, apps[i].version, apps[i].installed, apps[i].reviews);
    }
}

void appstore_list_apps() {
    printf("Listing available apps\n");
}
