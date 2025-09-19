
#include <stdio.h>
#define MAX_MONITORS 8
struct Monitor {
    int id;
    int x, y, w, h;
    int workspace_id;
};
struct Monitor monitors[MAX_MONITORS];
int monitor_count = 0;

void multimonitor_init() {
    monitor_count = 0;
    printf("Multi-monitor initialized\n");
}

int multimonitor_add(int id, int x, int y, int w, int h) {
    if (monitor_count >= MAX_MONITORS) return -1;
    monitors[monitor_count].id = id;
    monitors[monitor_count].x = x;
    monitors[monitor_count].y = y;
    monitors[monitor_count].w = w;
    monitors[monitor_count].h = h;
    monitors[monitor_count].workspace_id = 0;
    monitor_count++;
    printf("Monitor added: id=%d (%d,%d,%d,%d)\n", id, x, y, w, h);
    return monitor_count - 1;
}

int multimonitor_count() {
    return monitor_count;
}

void multimonitor_assign_workspace(int id, int workspace_id) {
    for (int i = 0; i < monitor_count; i++) {
        if (monitors[i].id == id) {
            monitors[i].workspace_id = workspace_id;
            printf("Monitor %d assigned to workspace %d\n", id, workspace_id);
            return;
        }
    }
}

void multimonitor_stats() {
    printf("Monitors: %d\n", monitor_count);
    for (int i = 0; i < monitor_count; i++) {
        printf("Monitor %d: (%d,%d,%d,%d) workspace=%d\n", monitors[i].id, monitors[i].x, monitors[i].y, monitors[i].w, monitors[i].h, monitors[i].workspace_id);
    }
}
