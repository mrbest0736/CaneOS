

// CaneOS Device Drivers
#include <stdint.h>

#define VIDEO_MEMORY (unsigned char*)0xB8000
#define KEYBOARD_DATA_PORT 0x60

// Display driver: clear screen and print character
void display_init() {
	unsigned char* video = VIDEO_MEMORY;
	for (int i = 0; i < 80 * 25 * 2; i += 2) {
		video[i] = ' ';
		video[i + 1] = 0x07;
	}
}

void display_putc(char c, int pos) {
	unsigned char* video = VIDEO_MEMORY;
	video[pos * 2] = c;
	video[pos * 2 + 1] = 0x07;
}

// Keyboard driver: read scancode (polling)
unsigned char keyboard_read_scancode() {
	unsigned char scancode;
	__asm {
		in al, KEYBOARD_DATA_PORT
		mov scancode, al
	}
	return scancode;
}

void keyboard_init() {
	// No initialization needed for basic polling
}

// Serial port (COM1) base address
#define SERIAL_PORT 0x3F8

void serial_init() {
	// Configure COM1: 115200 baud, 8N1
	__asm {
		mov dx, SERIAL_PORT + 1
		mov al, 0x00
		out dx, al
		mov dx, SERIAL_PORT + 3
		mov al, 0x80
		out dx, al
		mov dx, SERIAL_PORT
		mov al, 0x01
		out dx, al
		mov dx, SERIAL_PORT + 1
		mov al, 0x00
		out dx, al
		mov dx, SERIAL_PORT + 3
		mov al, 0x03
		out dx, al
		mov dx, SERIAL_PORT + 2
		mov al, 0xC7
		out dx, al
		mov dx, SERIAL_PORT + 4
		mov al, 0x0B
		out dx, al
	}
}

int serial_received() {
	int received;
	__asm {
		mov dx, SERIAL_PORT + 5
		in al, dx
		mov received, al
	}
	return received & 1;
}

char serial_read() {
	while (!serial_received());
	char data;
	__asm {
		mov dx, SERIAL_PORT
		in al, dx
		mov data, al
	}
	return data;
}

int serial_is_transmit_empty() {
	int empty;
	__asm {
		mov dx, SERIAL_PORT + 5
		in al, dx
		mov empty, al
	}
	return empty & 0x20;
}

void serial_write(char a) {
	while (!serial_is_transmit_empty());
	__asm {
		mov dx, SERIAL_PORT
		mov al, a
		out dx, al
	}
}

// Basic framebuffer driver (VGA mode 13h: 320x200, 256 colors)
#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_ADDRESS 0xA0000

// Simple mouse driver (PS/2)
typedef struct {
	int x;
	int y;
	int left_btn;
	int right_btn;
} MouseState;

MouseState mouse = {VGA_WIDTH / 2, VGA_HEIGHT / 2, 0, 0};

void mouse_init() {
	// Enable mouse (simplified, real hardware needs more steps)
	// This is a stub for demonstration
	mouse.x = VGA_WIDTH / 2;
	mouse.y = VGA_HEIGHT / 2;
	mouse.left_btn = 0;
	mouse.right_btn = 0;
}

void mouse_update() {
	// Stub: move mouse with arrow keys for demo
	unsigned char sc = keyboard_read_scancode();
	if (sc == 0x4B && mouse.x > 0) mouse.x -= 2; // Left
	if (sc == 0x4D && mouse.x < VGA_WIDTH - 1) mouse.x += 2; // Right
	if (sc == 0x48 && mouse.y > 0) mouse.y -= 2; // Up
	if (sc == 0x50 && mouse.y < VGA_HEIGHT - 1) mouse.y += 2; // Down
	// Simulate left mouse button with Enter key (scancode 0x1C)
	mouse.left_btn = (sc == 0x1C) ? 1 : 0;
}

void framebuffer_draw_mouse_cursor(int x, int y) {
	// Draw a simple crosshair cursor
	framebuffer_putpixel(x, y, 0x0F);
	framebuffer_putpixel(x - 1, y, 0x0F);
	framebuffer_putpixel(x + 1, y, 0x0F);
	framebuffer_putpixel(x, y - 1, 0x0F);
	framebuffer_putpixel(x, y + 1, 0x0F);
}

void framebuffer_init() {
	// Set VGA to mode 13h
	__asm {
		mov ax, 0x0013
		int 0x10
	}
}

void framebuffer_putpixel(int x, int y, unsigned char color) {
	if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;
	unsigned char* fb = (unsigned char*)VGA_ADDRESS;
	fb[y * VGA_WIDTH + x] = color;
}

// Draw filled rectangle
void framebuffer_draw_rect(int x, int y, int w, int h, unsigned char color) {
	for (int i = x; i < x + w; i++) {
		for (int j = y; j < y + h; j++) {
			framebuffer_putpixel(i, j, color);
		}
	}
}

// Display text (simple, using BIOS interrupt)
void framebuffer_draw_text(int x, int y, const char* text, unsigned char color) {
	// Set cursor position
	__asm {
		mov ah, 0x02
		mov bh, 0x00
		mov dh, y
		mov dl, x
		int 0x10
	}
	// Print string
	while (*text) {
		__asm {
			mov ah, 0x0E
			mov al, [text]
			mov bh, 0x00
			mov bl, color
			int 0x10
		}
		text++;
	}
}
