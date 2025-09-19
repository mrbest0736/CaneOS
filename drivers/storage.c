// CaneOS Storage Driver
#include <stdint.h>


#define MAX_STORAGE_DEVICES 4
struct StorageDevice {
    char name[16];
    int online;
    int sectors;
    int errors;
    void* cache;
};
struct StorageDevice storage_devices[MAX_STORAGE_DEVICES];
int storage_device_count = 0;

void storage_init() {
    // Enumerate and initialize storage devices
    storage_device_count = 2;
    strcpy(storage_devices[0].name, "sda");
    storage_devices[0].online = 1;
    storage_devices[0].sectors = 2048;
    storage_devices[0].errors = 0;
    storage_devices[0].cache = malloc(2048 * 512);
    strcpy(storage_devices[1].name, "sdb");
    storage_devices[1].online = 1;
    storage_devices[1].sectors = 1024;
    storage_devices[1].errors = 0;
    storage_devices[1].cache = malloc(1024 * 512);
}

int storage_read(void* buf, int sector, int count) {
    // Demo: read from sda cache
    if (storage_devices[0].online && sector + count <= storage_devices[0].sectors) {
        memcpy(buf, (uint8_t*)storage_devices[0].cache + sector * 512, count * 512);
        return count;
    }
    storage_devices[0].errors++;
    return -1;
}

int storage_write(const void* buf, int sector, int count) {
    // Demo: write to sda cache
    if (storage_devices[0].online && sector + count <= storage_devices[0].sectors) {
        memcpy((uint8_t*)storage_devices[0].cache + sector * 512, buf, count * 512);
        return count;
    }
    storage_devices[0].errors++;
    return -1;
}

void storage_hotplug(const char* name) {
    // Demo: add new device
    if (storage_device_count < MAX_STORAGE_DEVICES) {
        strcpy(storage_devices[storage_device_count].name, name);
        storage_devices[storage_device_count].online = 1;
        storage_devices[storage_device_count].sectors = 512;
        storage_devices[storage_device_count].errors = 0;
        storage_devices[storage_device_count].cache = malloc(512 * 512);
        storage_device_count++;
    }
}

void storage_stats() {
    for (int i = 0; i < storage_device_count; i++) {
        printf("%s: online=%d sectors=%d errors=%d\n", storage_devices[i].name, storage_devices[i].online, storage_devices[i].sectors, storage_devices[i].errors);
    }
}
