#include "security.c"
#include "acl.c"
#include "secureboot.c"
#include "encryption.c"
#include "sandbox.c"



#define HEAP_SIZE 4096
// CaneOS Kernel
#include <stdint.h>
// Core modules
#include "../drivers/drivers.c"
#include "../drivers/network.c"
#include "../drivers/storage.c"
#include "../drivers/graphics.c"
#include "../drivers/usb.c"
#include "../drivers/smp.c"
#include "../fs/filesystem.c"
#include "../fs/vfs.c"
#include "process.c"
#include "memory.c"
#include "../user/shell.c"

static uint8_t heap[HEAP_SIZE];
static size_t heap_offset = 0;
// User management
#define MAX_USERS 8
struct User {
	char username[16];
	char password[16];
	int uid;
	int is_admin;
};
struct User users[MAX_USERS];
int user_count = 0;
int current_uid = 0;

// Check if user is admin
int is_admin(int uid) {
	for (int i = 0; i < user_count; i++) {
		if (users[i].uid == uid) return users[i].is_admin;
	}
	return 0;
}

void* cane_malloc(size_t size) {
	if (heap_offset + size > HEAP_SIZE) return 0;
	void* ptr = &heap[heap_offset];
	heap_offset += size;
	return ptr;
}

// Window system
#define MAX_WINDOWS 8
typedef enum { WIN_SHELL, WIN_INFO, WIN_APP } WindowType;
struct Window {
	int x, y;
	int width, height;
	int visible;
	char title[32];
	WindowType type;
	char input[64]; // For shell window text input
	int input_len;
	int owner_uid; // User ID of window owner
};

void kernel_main() {
	// Demo users
	cane_strcpy(users[0].username, "root");
	cane_strcpy(users[0].password, "root");
	users[0].uid = 0;
	users[0].is_admin = 1;
	cane_strcpy(users[1].username, "guest");
	cane_strcpy(users[1].password, "guest");
	users[1].uid = 1;
	users[1].is_admin = 0;
	user_count = 2;
	current_uid = 0;



	// Security: user/group management
	add_group("admin");
	add_user("root", 0, 1);
	add_user("guest", 0, 0);

	// Security: ACLs
	set_acl(1, 0, 0x7); // resource_id=1, uid=0, permissions=read/write/execute

	// Security: secure boot
	secure_boot_init();

	// Security: encryption
	char enc_out[16], dec_out[16];
	encrypt_data("secret", enc_out, 6);
	decrypt_data(enc_out, dec_out, 6);

	// Security: sandboxing
	sandbox_init(pid);

	// Demo: create a process
	uint8_t* proc_mem = alloc_mem(4096);
	int pid = create_process(-1, 0, proc_mem, 4096);

	// Demo: mount a virtual filesystem
	vfs_mount("/", FS_TYPE_MEM, NULL);

	// Demo: hardware driver calls
	network_send("Hello", 5);
	storage_write_block(0, "BlockData");
	graphics_draw_pixel(10, 10, 0x0F);
	usb_write_device(0, "USBData", 7);
	int cores = smp_get_core_count();

	int dragging_win = -1;
	int drag_offset_x = 0, drag_offset_y = 0;
	struct Window windows[MAX_WINDOWS];
	int active_win = 0;
	int window_count = 2;
	int menu_height = 24;
	char menu_text[] = " CaneOS | N: New Window | A: App | Tab: Switch | C: Close ";
	int app_counter = 0;
	unsigned int clock = 0;
	char clock_str[16];
	char notification[64] = "";
	unsigned int notify_timer = 0;
	// Initialize two windows
	windows[0].x = 60;
	windows[0].y = 40;
	windows[0].width = 200;
	windows[0].height = 120;
	windows[0].visible = 1;
	cane_strcpy(windows[0].title, "Shell");
	windows[0].type = WIN_SHELL;
	windows[0].input_len = 0;
	windows[1].x = 120;
	windows[1].y = 80;
	windows[1].width = 200;
	windows[1].height = 120;
	windows[1].visible = 1;
	cane_strcpy(windows[1].title, "Info");
	windows[1].type = WIN_INFO;
	windows[1].input_len = 0;

	while (1) {
	// Draw menu bar
	framebuffer_draw_rect(0, 0, VGA_WIDTH, menu_height, 0x1F);
	framebuffer_draw_text(10, 5, menu_text, 0x0F);
		clock++;
		clock++;
		// Draw windows
		for (int i = 0; i < window_count; i++) {
			if (i != active_win && windows[i].visible) {
				// Drop shadow
				framebuffer_draw_rect(windows[i].x + 4, windows[i].y + 8, windows[i].width, windows[i].height, 0x08);
				// Window border
				framebuffer_draw_rect(windows[i].x, windows[i].y, windows[i].width, windows[i].height, 0x2F);
				// Rounded corners (simulate with small circles)
				framebuffer_draw_rect(windows[i].x, windows[i].y, 8, 8, 0x1F);
				framebuffer_draw_rect(windows[i].x + windows[i].width - 8, windows[i].y, 8, 8, 0x1F);
				framebuffer_draw_rect(windows[i].x, windows[i].y + windows[i].height - 8, 8, 8, 0x1F);
				framebuffer_draw_rect(windows[i].x + windows[i].width - 8, windows[i].y + windows[i].height - 8, 8, 8, 0x1F);
				// Title bar
				framebuffer_draw_rect(windows[i].x, windows[i].y, windows[i].width, 24, 0x1F);
				framebuffer_draw_text(windows[i].x + 32, windows[i].y + 5, windows[i].title, 0x0F);
				// Icon
				char icon = '*';
				if (windows[i].type == WIN_SHELL) icon = '$';
				else if (windows[i].type == WIN_INFO) icon = 'i';
				else if (windows[i].type == WIN_APP) icon = 'A';
				framebuffer_draw_text(windows[i].x + 10, windows[i].y + 5, &icon, 0x0E);
				// Close button
				framebuffer_draw_rect(windows[i].x + windows[i].width - 20, windows[i].y + 10, 15, 15, 0x4F);
				framebuffer_draw_text(windows[i].x + windows[i].width - 17, windows[i].y + 13, "X", 0x0F);
				// Resize handle
				framebuffer_draw_rect(windows[i].x + windows[i].width - 10, windows[i].y + windows[i].height - 10, 10, 10, 0x6F);
				// Window content by type
				if (windows[i].type == WIN_SHELL) {
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 40, ">", 0x0E);
					framebuffer_draw_text(windows[i].x + 35, windows[i].y + 40, windows[i].input, 0x0F);
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 60, "Type and press Enter.", 0x0E);
				} else if (windows[i].type == WIN_INFO) {
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 40, "Info Window", 0x0E);
					char buf[32];
					cane_sprintf(buf, "Windows: %d", window_count);
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 60, buf, 0x0E);
				} else if (windows[i].type == WIN_APP) {
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 40, "Counter App", 0x0E);
					char buf[32];
					cane_sprintf(buf, "Value: %d", app_counter);
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 60, buf, 0x0E);
					framebuffer_draw_text(windows[i].x + 20, windows[i].y + 80, "+/- to change", 0x0E);
				}
			}
		}
		// Draw active window last (on top)
		if (windows[active_win].visible) {
			// Drop shadow
			framebuffer_draw_rect(windows[active_win].x + 4, windows[active_win].y + 8, windows[active_win].width, windows[active_win].height, 0x08);
			// Highlighted border for active window
			framebuffer_draw_rect(windows[active_win].x, windows[active_win].y, windows[active_win].width, windows[active_win].height, 0x4F);
			// Rounded corners
			framebuffer_draw_rect(windows[active_win].x, windows[active_win].y, 8, 8, 0x1F);
			framebuffer_draw_rect(windows[active_win].x + windows[active_win].width - 8, windows[active_win].y, 8, 8, 0x1F);
			framebuffer_draw_rect(windows[active_win].x, windows[active_win].y + windows[active_win].height - 8, 8, 8, 0x1F);
			framebuffer_draw_rect(windows[active_win].x + windows[active_win].width - 8, windows[active_win].y + windows[active_win].height - 8, 8, 8, 0x1F);
			// Title bar
			framebuffer_draw_rect(windows[active_win].x, windows[active_win].y, windows[active_win].width, 24, 0x1F);
			framebuffer_draw_text(windows[active_win].x + 32, windows[active_win].y + 5, windows[active_win].title, 0x0F);
			// Icon
			char icon = '*';
			if (windows[active_win].type == WIN_SHELL) icon = '$';
			else if (windows[active_win].type == WIN_INFO) icon = 'i';
			else if (windows[active_win].type == WIN_APP) icon = 'A';
			framebuffer_draw_text(windows[active_win].x + 10, windows[active_win].y + 5, &icon, 0x0E);
			// Close button
			framebuffer_draw_rect(windows[active_win].x + windows[active_win].width - 20, windows[active_win].y + 10, 15, 15, 0x4F);
			framebuffer_draw_text(windows[active_win].x + windows[active_win].width - 17, windows[active_win].y + 13, "X", 0x0F);
			// Resize handle
			framebuffer_draw_rect(windows[active_win].x + windows[active_win].width - 10, windows[active_win].y + windows[active_win].height - 10, 10, 10, 0x6F);
			// Window content by type
			if (windows[active_win].type == WIN_SHELL) {
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 40, ">", 0x0E);
				framebuffer_draw_text(windows[active_win].x + 35, windows[active_win].y + 40, windows[active_win].input, 0x0F);
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 60, "Type and press Enter.", 0x0E);
			} else if (windows[active_win].type == WIN_INFO) {
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 40, "Info Window", 0x0E);
				char buf[32];
				cane_sprintf(buf, "Windows: %d", window_count);
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 60, buf, 0x0E);
			} else if (windows[active_win].type == WIN_APP) {
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 40, "Counter App", 0x0E);
				char buf[32];
				cane_sprintf(buf, "Value: %d", app_counter);
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 60, buf, 0x0E);
				framebuffer_draw_text(windows[active_win].x + 20, windows[active_win].y + 80, "+/- to change", 0x0E);
			}
		}
		// Window resizing logic
		static int resizing_win = -1;
		static int resize_start_x = 0, resize_start_y = 0;
		static int orig_width = 0, orig_height = 0;
		// Check for mouse down on resize handle
		for (int i = 0; i < window_count; i++) {
			if (windows[i].visible) {
				int rx = windows[i].x + windows[i].width - 10;
				int ry = windows[i].y + windows[i].height - 10;
				if (mouse.left_btn && mouse.x >= rx && mouse.x <= rx + 10 && mouse.y >= ry && mouse.y <= ry + 10) {
					if (resizing_win == -1) {
						resizing_win = i;
						resize_start_x = mouse.x;
						resize_start_y = mouse.y;
						orig_width = windows[i].width;
						orig_height = windows[i].height;
					}
				}
			}
		}
		// If resizing, update window size
		if (resizing_win != -1 && mouse.left_btn) {
			int dx = mouse.x - resize_start_x;
			int dy = mouse.y - resize_start_y;
			windows[resizing_win].width = orig_width + dx;
			windows[resizing_win].height = orig_height + dy;
			if (windows[resizing_win].width < 120) windows[resizing_win].width = 120;
			if (windows[resizing_win].height < 60) windows[resizing_win].height = 60;
			if (windows[resizing_win].width > VGA_WIDTH - windows[resizing_win].x) windows[resizing_win].width = VGA_WIDTH - windows[resizing_win].x;
			if (windows[resizing_win].height > VGA_HEIGHT - windows[resizing_win].y) windows[resizing_win].height = VGA_HEIGHT - windows[resizing_win].y;
		}
		if (resizing_win != -1 && !mouse.left_btn) {
			resizing_win = -1;
		}

		// Draw notification (show for 100 ticks)
		if (notification[0] && clock - notify_timer < 100) {
			framebuffer_draw_text(400, 5, notification, 0x4F);
		}
		// Draw clock (example)
		int idx = 0;
		unsigned int t = clock;
		if (t == 0) { clock_str[idx++] = '0'; }
		else {
			while (t > 0 && idx < 15) {
				clock_str[idx++] = '0' + (t % 10);
				t /= 10;
			}
		}
		clock_str[idx] = 0;
		framebuffer_draw_text(300, 5, clock_str, 0x0E);

		// Update and draw mouse cursor
		mouse_update();
		framebuffer_draw_mouse_cursor(mouse.x, mouse.y);

		// Mouse click detection for close button and dragging
		for (int i = 0; i < window_count; i++) {
			if (windows[i].visible) {
				int cx = windows[i].x + 180, cy = windows[i].y + 10;
				if (mouse.left_btn && mouse.x >= cx && mouse.x <= cx + 15 && mouse.y >= cy && mouse.y <= cy + 15) {
					// Only owner or admin can close
					if (windows[i].owner_uid == current_uid || is_admin(current_uid)) {
						windows[i].visible = 0;
						cane_strcpy(notification, "Window closed (mouse)");
						notify_timer = clock;
					} else {
						cane_strcpy(notification, "Permission denied");
						notify_timer = clock;
					}
				}
				// Window dragging: title bar area
				if (mouse.left_btn && mouse.x >= windows[i].x && mouse.x <= windows[i].x + 200 && mouse.y >= windows[i].y && mouse.y <= windows[i].y + 20) {
					if (dragging_win == -1) {
						dragging_win = i;
						drag_offset_x = mouse.x - windows[i].x;
						drag_offset_y = mouse.y - windows[i].y;
						active_win = i; // Set focus to clicked window
					}
				}
			}
		}

		// Move window if dragging
		if (dragging_win != -1 && mouse.left_btn) {
			windows[dragging_win].x = mouse.x - drag_offset_x;
			windows[dragging_win].y = mouse.y - drag_offset_y;
			// Clamp to screen
			if (windows[dragging_win].x < 0) windows[dragging_win].x = 0;
			if (windows[dragging_win].x > VGA_WIDTH - 200) windows[dragging_win].x = VGA_WIDTH - 200;
			if (windows[dragging_win].y < 20) windows[dragging_win].y = 20;
			if (windows[dragging_win].y > VGA_HEIGHT - 120) windows[dragging_win].y = VGA_HEIGHT - 120;
		}
		if (dragging_win != -1 && !mouse.left_btn) {
			dragging_win = -1;
		}

		// Poll keyboard for arrow key input, Tab, 'C' for close, 'N' for new window
		unsigned char sc = keyboard_read_scancode();
		// Tab key (scancode 0x0F)
		if (sc == 0x0F) {
			// Cycle through windows
			int next = active_win;
			do {
				next = (next + 1) % window_count;
			} while (!windows[next].visible && next != active_win);
			active_win = next;
		}
		// 'C' key (scancode 0x2E)
		if (sc == 0x2E && windows[active_win].visible) {
			// Only owner or admin can close
			if (windows[active_win].owner_uid == current_uid || is_admin(current_uid)) {
				windows[active_win].visible = 0;
				cane_strcpy(notification, "Window closed");
				notify_timer = clock;
			} else {
				cane_strcpy(notification, "Permission denied");
				notify_timer = clock;
			}
		}
		// 'N' key (scancode 0x31) opens a new window (alternates type)
		if (sc == 0x31 && window_count < MAX_WINDOWS) {
			windows[window_count].x = 30 + 20 * window_count;
			windows[window_count].y = 30 + 20 * window_count;
			windows[window_count].width = 200;
			windows[window_count].height = 120;
			windows[window_count].visible = 1;
			windows[window_count].owner_uid = current_uid;
			if (window_count % 3 == 0) {
				cane_strcpy(windows[window_count].title, "Shell");
				windows[window_count].type = WIN_SHELL;
				windows[window_count].input_len = 0;
			} else if (window_count % 3 == 1) {
				cane_strcpy(windows[window_count].title, "Info");
				windows[window_count].type = WIN_INFO;
				windows[window_count].input_len = 0;
			} else {
				cane_strcpy(windows[window_count].title, "App");
				windows[window_count].type = WIN_APP;
				windows[window_count].input_len = 0;
			}
			active_win = window_count;
			window_count++;
			cane_strcpy(notification, "New window opened");
			notify_timer = clock;
		}
		// 'A' key (scancode 0x1E) opens an app window
		if (sc == 0x1E && window_count < MAX_WINDOWS) {
			windows[window_count].x = 40 + 20 * window_count;
			windows[window_count].y = 40 + 20 * window_count;
			windows[window_count].width = 200;
			windows[window_count].height = 120;
			windows[window_count].visible = 1;
			windows[window_count].owner_uid = current_uid;
			cane_strcpy(windows[window_count].title, "App");
			windows[window_count].type = WIN_APP;
			windows[window_count].input_len = 0;
			active_win = window_count;
			window_count++;
			cane_strcpy(notification, "App window opened");
			notify_timer = clock;
		}
		// App window: increment/decrement counter
		if (windows[active_win].type == WIN_APP) {
			if (sc == 0x4E) { // '+' key (numpad plus)
				app_counter++;
			} else if (sc == 0x4A) { // '-' key (numpad minus)
				app_counter--;
			}
		}
		// Basic text input for shell window
		if (windows[active_win].type == WIN_SHELL) {
			// Accept alphanumeric keys and backspace
			if (sc >= 0x10 && sc <= 0x19) { // QWERTY row
				char ch = 'q' + (sc - 0x10);
				if (windows[active_win].input_len < 63) {
					windows[active_win].input[windows[active_win].input_len++] = ch;
					windows[active_win].input[windows[active_win].input_len] = 0;
				}
			} else if (sc >= 0x1E && sc <= 0x26) { // ASDF row
				char row[] = "asdfghjkl";
				char ch = row[sc - 0x1E];
				if (windows[active_win].input_len < 63) {
					windows[active_win].input[windows[active_win].input_len++] = ch;
					windows[active_win].input[windows[active_win].input_len] = 0;
				}
			} else if (sc >= 0x2C && sc <= 0x32) { // ZXCV row
				char row[] = "zxcvbnm";
				char ch = row[sc - 0x2C];
				if (windows[active_win].input_len < 63) {
					windows[active_win].input[windows[active_win].input_len++] = ch;
					windows[active_win].input[windows[active_win].input_len] = 0;
				}
			} else if (sc == 0x0E && windows[active_win].input_len > 0) { // Backspace
				windows[active_win].input[--windows[active_win].input_len] = 0;
			} else if (sc == 0x1C) { // Enter
				windows[active_win].input_len = 0;
				windows[active_win].input[0] = 0;
			}
		}
	}
