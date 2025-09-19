// CaneOS Graphics Driver
#include <stdint.h>


#define FB_WIDTH 800
#define FB_HEIGHT 600
static uint8_t framebuffer[FB_WIDTH * FB_HEIGHT];
static uint8_t backbuffer[FB_WIDTH * FB_HEIGHT];
static int use_double_buffer = 1;
static int resolution_x = FB_WIDTH;
static int resolution_y = FB_HEIGHT;

void graphics_init() {
    // Initialize framebuffer and double-buffering
    memset(framebuffer, 0, sizeof(framebuffer));
    memset(backbuffer, 0, sizeof(backbuffer));
    use_double_buffer = 1;
    resolution_x = FB_WIDTH;
    resolution_y = FB_HEIGHT;
}

void graphics_set_resolution(int x, int y) {
    // Demo: change resolution
    resolution_x = x;
    resolution_y = y;
}

void graphics_draw_pixel(int x, int y, uint8_t color) {
    if (x < 0 || y < 0 || x >= resolution_x || y >= resolution_y) return;
    if (use_double_buffer)
        backbuffer[y * resolution_x + x] = color;
    else
        framebuffer[y * resolution_x + x] = color;
}

void graphics_draw_rect(int x, int y, int w, int h, uint8_t color) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            graphics_draw_pixel(i, j, color);
        }
    }
}

void graphics_flip() {
    if (use_double_buffer) {
        memcpy(framebuffer, backbuffer, resolution_x * resolution_y);
        memset(backbuffer, 0, resolution_x * resolution_y);
    }
}

void graphics_accelerate_draw() {
    // Demo: pretend to use hardware acceleration
}
