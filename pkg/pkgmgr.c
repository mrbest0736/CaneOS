// CaneOS Package Manager
#include <stdio.h>
#include <string.h>
#define MAX_PACKAGES 64


#include <stdio.h>
#include <string.h>
#define MAX_PKGS 256
#define MAX_REPOS 16
struct Package {
    char name[64];
    char version[16];
    char dependencies[8][64];
    int dep_count;
    int installed;
};
struct Repository {
    char url[128];
    int enabled;
};
struct Package pkg_db[MAX_PKGS];
int pkg_count = 0;
struct Repository repo_db[MAX_REPOS];
int repo_count = 0;

void pkgmgr_init() {
    pkg_count = 0;
    repo_count = 0;
    printf("Package manager initialized\n");
}

int pkgmgr_add_repo(const char* url) {
    if (repo_count >= MAX_REPOS) return -1;
    strncpy(repo_db[repo_count].url, url, 127);
    repo_db[repo_count].enabled = 1;
    repo_count++;
    printf("Repository added: %s\n", url);
    return 0;
}

int pkgmgr_install(const char* pkg) {
    for (int i = 0; i < pkg_count; i++) {
        if (strcmp(pkg_db[i].name, pkg) == 0) {
            if (pkg_db[i].installed) return 0;
            // Install dependencies first
            for (int d = 0; d < pkg_db[i].dep_count; d++) {
                pkgmgr_install(pkg_db[i].dependencies[d]);
            }
            pkg_db[i].installed = 1;
            printf("Installed package: %s\n", pkg);
            return 0;
        }
    }
    printf("Package not found: %s\n", pkg);
    return -1;
}

int pkgmgr_remove(const char* pkg) {
    for (int i = 0; i < pkg_count; i++) {
        if (strcmp(pkg_db[i].name, pkg) == 0) {
            if (!pkg_db[i].installed) return 0;
            pkg_db[i].installed = 0;
            printf("Removed package: %s\n", pkg);
            return 0;
        }
    }
    printf("Package not found: %s\n", pkg);
    return -1;
}

int pkgmgr_update(const char* pkg) {
    for (int i = 0; i < pkg_count; i++) {
        if (strcmp(pkg_db[i].name, pkg) == 0) {
            printf("Updated package: %s\n", pkg);
            return 0;
        }
    }
    printf("Package not found: %s\n", pkg);
    return -1;
}

void pkgmgr_list() {
    printf("Installed packages:\n");
    for (int i = 0; i < pkg_count; i++) {
        if (pkg_db[i].installed) {
            printf("%s %s\n", pkg_db[i].name, pkg_db[i].version);
        }
    }
}

void pkgmgr_repo_list() {
    printf("Repositories:\n");
    for (int i = 0; i < repo_count; i++) {
        printf("%s (enabled=%d)\n", repo_db[i].url, repo_db[i].enabled);
    }
}

void pkgmgr_stats() {
    printf("Packages: %d\n", pkg_count);
    printf("Repositories: %d\n", repo_count);
}
