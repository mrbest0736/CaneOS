

// CaneOS File System
#include <stdint.h>
#include <string.h>

#define MAX_FILES 8
#define MAX_FILENAME 16
#define MAX_FILESIZE 128

#define MAX_DIRS 4
#define MAX_DIRNAME 16

typedef struct {
	char name[MAX_DIRNAME];
	int file_indices[MAX_FILES];
	int file_count;
	uint8_t used;
} Directory;

static Directory dirs[MAX_DIRS];
static int current_dir = 0;

typedef struct {
	char name[MAX_FILENAME];
	char data[MAX_FILESIZE];
	uint32_t size;
	uint8_t used;
} File;

static File files[MAX_FILES];

void fs_init() {
	for (int i = 0; i < MAX_FILES; i++) {
		files[i].used = 0;
		files[i].size = 0;
		files[i].name[0] = '\0';
	}
	for (int i = 0; i < MAX_DIRS; i++) {
		dirs[i].used = 0;
		dirs[i].file_count = 0;
		dirs[i].name[0] = '\0';
	}
	// Create root directory
	cane_strcpy(dirs[0].name, "/");
	dirs[0].used = 1;
	dirs[0].file_count = 0;
	current_dir = 0;
}

int fs_create(const char* filename) {
	for (int i = 0; i < MAX_FILES; i++) {
		if (!files[i].used) {
			cane_strcpy(files[i].name, filename);
			files[i].size = 0;
			files[i].used = 1;
			// Add file to current directory
			Directory* dir = &dirs[current_dir];
			if (dir->file_count < MAX_FILES) {
				dir->file_indices[dir->file_count++] = i;
			}
			return i;
		}
	}
	return -1; // No space
}

int fs_write(const char* filename, const char* data, uint32_t size) {
	for (int i = 0; i < MAX_FILES; i++) {
		if (files[i].used && strncmp(files[i].name, filename, MAX_FILENAME) == 0) {
			uint32_t write_size = size > MAX_FILESIZE ? MAX_FILESIZE : size;
			memcpy(files[i].data, data, write_size);
			files[i].size = write_size;
			return write_size;
		}
	}
	return -1; // Not found
}

int fs_read(const char* filename, void* buffer, uint32_t size) {
	for (int i = 0; i < MAX_FILES; i++) {
		if (files[i].used && strncmp(files[i].name, filename, MAX_FILENAME) == 0) {
			uint32_t read_size = size > files[i].size ? files[i].size : size;
			memcpy(buffer, files[i].data, read_size);
			return read_size;
		}
	}
	return -1; // Not found
}

int fs_mkdir(const char* dirname) {
	for (int i = 0; i < MAX_DIRS; i++) {
		if (!dirs[i].used) {
			cane_strcpy(dirs[i].name, dirname);
			dirs[i].used = 1;
			dirs[i].file_count = 0;
			return i;
		}
	}
	return -1; // No space
}

int fs_chdir(const char* dirname) {
	for (int i = 0; i < MAX_DIRS; i++) {
		if (dirs[i].used && cane_strcmp(dirs[i].name, dirname) == 0) {
			current_dir = i;
			return i;
		}
	}
	return -1; // Not found
}

void fs_ls() {
	Directory* dir = &dirs[current_dir];
	for (int i = 0; i < dir->file_count; i++) {
		int idx = dir->file_indices[i];
		if (files[idx].used) {
			// For demonstration, print first char of filename to display
			display_putc(files[idx].name[0], 200 + i);
		}
	}
}
