
#include <stdio.h>
#define MAX_WINDOWS 32
struct Window {
    int x, y, w, h;
    int visible;
    char title[64];
};
struct Window windows[MAX_WINDOWS];
int window_count = 0;

void gui_init() {
    window_count = 0;
    printf("GUI initialized\n");
}

int gui_create_window(int x, int y, int w, int h, const char* title) {
    if (window_count >= MAX_WINDOWS) return -1;
    windows[window_count].x = x;
    windows[window_count].y = y;
    windows[window_count].w = w;
    windows[window_count].h = h;
    windows[window_count].visible = 1;
    snprintf(windows[window_count].title, 63, "%s", title);
    window_count++;
    printf("Window created: %s (%d,%d,%d,%d)\n", title, x, y, w, h);
    return window_count - 1;
}

void gui_draw_window(int idx) {
    if (idx < 0 || idx >= window_count || !windows[idx].visible) return;
    printf("Drawing window: %s (%d,%d,%d,%d)\n", windows[idx].title, windows[idx].x, windows[idx].y, windows[idx].w, windows[idx].h);
}

void gui_handle_event(int idx, const char* event) {
    if (idx < 0 || idx >= window_count) return;
    printf("Window %s received event: %s\n", windows[idx].title, event);
}

void gui_set_theme(const char* theme) {
    printf("Theme set: %s\n", theme);
}

void gui_stats() {
    printf("Windows: %d\n", window_count);
    for (int i = 0; i < window_count; i++) {
        printf("Window %d: %s (%d,%d,%d,%d) visible=%d\n", i, windows[i].title, windows[i].x, windows[i].y, windows[i].w, windows[i].h, windows[i].visible);
    }
}
