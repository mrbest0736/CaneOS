// CaneOS Desktop Environment
#include <stdio.h>

struct Icon {
    char name[64];
    int x, y;
};
// Maximum number of desktop icons
#define MAX_ICONS 64
struct Icon icons[MAX_ICONS];
int icon_count = 0;
char wallpaper[128] = "default.jpg";
int workspace_id = 0;

void desktop_init() {
    icon_count = 0;
    workspace_id = 0;
    snprintf(wallpaper, 127, "default.jpg");
    printf("Desktop initialized\n");
}

int desktop_add_icon(const char* name, int x, int y) {
    if (icon_count >= MAX_ICONS) return -1;
    snprintf(icons[icon_count].name, 63, "%s", name);
    icons[icon_count].x = x;
    icons[icon_count].y = y;
    icon_count++;
    printf("Icon added: %s at (%d,%d)\n", name, x, y);
    return icon_count - 1;
}

void desktop_set_wallpaper(const char* file) {
    snprintf(wallpaper, 127, "%s", file);
    printf("Wallpaper set: %s\n", wallpaper);
}

void desktop_switch_workspace(int id) {
    workspace_id = id;
    printf("Switched to workspace %d\n", id);
}

void desktop_notify(const char* msg) {
    printf("Notification: %s\n", msg);
}

void desktop_stats() {
    printf("Icons: %d\n", icon_count);
    printf("Wallpaper: %s\n", wallpaper);
    printf("Workspace: %d\n", workspace_id);
}

void desktop_launch_app(const char* app) {
    printf("Launching app: %s\n", app);
}
