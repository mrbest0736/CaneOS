


// CaneOS User Shell
#include <stdint.h>
#include "../drivers/drivers.c"
#include "../lib/lib.c"

#define MAX_USERS 4
#define USERNAME_LEN 16
#define PASSWORD_LEN 16

typedef struct {
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN];
	int logged_in;
} ShellUser;

static ShellUser shell_users[MAX_USERS] = {
	{"admin", "admin", 0},
	{"user", "pass", 0}
};

int authenticate(const char* username, const char* password) {
	for (int i = 0; i < MAX_USERS; i++) {
		if (cane_strcmp(shell_users[i].username, username) == 0 && cane_strcmp(shell_users[i].password, password) == 0) {
			shell_users[i].logged_in = 1;
			return 1;
		}
	}
	return 0;
}

void shell_login() {
	char uname[USERNAME_LEN];
	char pword[PASSWORD_LEN];
	int pos = 0;
	display_init();
	display_putc('U', 0); display_putc('s', 1); display_putc('e', 2); display_putc('r', 3); display_putc(':', 4);
	pos = 5;
	int ulen = 0;
	while (1) {
		unsigned char sc = keyboard_read_scancode();
		char c = (sc >= 0x02 && sc <= 0x0A) ? ('1' + sc - 0x02) : (sc == 0x0B ? '0' : (sc == 0x1C ? '\n' : 0));
		if (c) {
			if (c == '\n') {
				uname[ulen] = '\0';
				break;
			} else {
				uname[ulen++] = c;
				display_putc(c, pos++);
			}
		}
	}
	display_putc('P', pos++); display_putc('a', pos++); display_putc('s', pos++); display_putc('s', pos++); display_putc(':', pos++);
	int plen = 0;
	while (1) {
		unsigned char sc = keyboard_read_scancode();
		char c = (sc >= 0x02 && sc <= 0x0A) ? ('1' + sc - 0x02) : (sc == 0x0B ? '0' : (sc == 0x1C ? '\n' : 0));
		if (c) {
			if (c == '\n') {
				pword[plen] = '\0';
				break;
			} else {
				pword[plen++] = c;
				display_putc('*', pos++);
			}
		}
	}
	if (authenticate(uname, pword)) {
		display_putc('L', pos++); display_putc('o', pos++); display_putc('g', pos++); display_putc('i', pos++); display_putc('n', pos++); display_putc(' ', pos++); display_putc('O', pos++); display_putc('K', pos++);
	} else {
		display_putc('F', pos++); display_putc('a', pos++); display_putc('i', pos++); display_putc('l', pos++); display_putc('e', pos++); display_putc('d', pos++);
		shell_login();
	}
}

#define INPUT_BUF_SIZE 64

void shell_execute(const char* cmd) {
	if (cane_strcmp(cmd, "echo") == 0) {
		display_putc('E', 100);
		display_putc('C', 101);
		display_putc('H', 102);
		display_putc('O', 103);
	} else if (cane_strcmp(cmd, "ls") == 0) {
		fs_ls();
	} else if (cane_strcmp(cmd, "mkdir") == 0) {
		fs_mkdir("newdir"); // For demo, always creates "newdir"
		display_putc('M', 105);
	} else if (cane_strcmp(cmd, "cd") == 0) {
		fs_chdir("newdir"); // For demo, always changes to "newdir"
		display_putc('C', 106);
	} else {
		display_putc('?', 104);
	}
}

void shell_start() {
	shell_login();
	char input[INPUT_BUF_SIZE];
	int pos = 1;
	display_putc('>', 0);
	while (1) {
		unsigned char sc = keyboard_read_scancode();
		char c = (sc >= 0x02 && sc <= 0x0A) ? ('1' + sc - 0x02) : (sc == 0x0B ? '0' : (sc == 0x1C ? '\n' : 0));
		if (c) {
			if (c == '\n') {
				input[pos - 1] = '\0';
				shell_execute(input);
				pos = 1;
				display_putc('>', pos - 1);
			} else {
				input[pos - 1] = c;
				display_putc(c, pos++);
			}
		}
	}
}
