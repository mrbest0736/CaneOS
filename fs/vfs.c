// CaneOS Virtual Filesystem Layer
#include <stdint.h>
#include <string.h>

#define VFS_MAX_MOUNTS 4
#define VFS_MAX_PATH 128

typedef enum { FS_TYPE_MEM, FS_TYPE_EXT4, FS_TYPE_BTRFS } FsType;

struct VfsMount {
    char mount_point[VFS_MAX_PATH];
    FsType type;
    void* fs_data;
};


struct VfsMount vfs_mounts[VFS_MAX_MOUNTS];
int vfs_mount_count = 0;

#define VFS_MAX_FILES 128
struct VfsFile {
    char path[VFS_MAX_PATH];
    int fd;
    int flags;
    int uid;
    int gid;
    int perm; // Bitmask: r=1, w=2, x=4
    int mount_id;
    int size;
    void* cache;
};
struct VfsFile vfs_file_table[VFS_MAX_FILES];
int vfs_file_count = 0;
int next_fd = 3;

int vfs_mount(const char* path, FsType type, void* fs_data) {
    if (vfs_mount_count >= VFS_MAX_MOUNTS) return -1;
    strncpy(vfs_mounts[vfs_mount_count].mount_point, path, VFS_MAX_PATH);
    vfs_mounts[vfs_mount_count].type = type;
    vfs_mounts[vfs_mount_count].fs_data = fs_data;
    vfs_mount_count++;
    return 0;
}

int vfs_umount(const char* path) {
    for (int i = 0; i < vfs_mount_count; i++) {
        if (strcmp(vfs_mounts[i].mount_point, path) == 0) {
            for (int j = i; j < vfs_mount_count - 1; j++) {
                vfs_mounts[j] = vfs_mounts[j + 1];
            }
            vfs_mount_count--;
            return 0;
        }
    }
    return -1;
}

int vfs_open(const char* path, int flags) {
    if (vfs_file_count >= VFS_MAX_FILES) return -1;
    int fd = next_fd++;
    struct VfsFile* f = &vfs_file_table[vfs_file_count];
    strncpy(f->path, path, VFS_MAX_PATH);
    f->fd = fd;
    f->flags = flags;
    f->uid = 0;
    f->gid = 0;
    f->perm = 3; // rw
    f->mount_id = 0;
    f->size = 0;
    f->cache = NULL;
    vfs_file_count++;
    return fd;
}

int vfs_read(int fd, void* buf, int size) {
    for (int i = 0; i < vfs_file_count; i++) {
        if (vfs_file_table[i].fd == fd && (vfs_file_table[i].perm & 1)) {
            // Demo: fill buffer with file path
            int len = strlen(vfs_file_table[i].path);
            int copy_size = len > size ? size : len;
            memcpy(buf, vfs_file_table[i].path, copy_size);
            return copy_size;
        }
    }
    return -1;
}

int vfs_write(int fd, const void* buf, int size) {
    for (int i = 0; i < vfs_file_count; i++) {
        if (vfs_file_table[i].fd == fd && (vfs_file_table[i].perm & 2)) {
            // Demo: cache written data
            if (!vfs_file_table[i].cache) vfs_file_table[i].cache = malloc(size);
            memcpy(vfs_file_table[i].cache, buf, size);
            vfs_file_table[i].size = size;
            return size;
        }
    }
    return -1;
}
