// CaneOS USB Driver
#include <stdint.h>


#define MAX_USB_DEVICES 8
typedef enum { USB_CTRL, USB_BULK, USB_INT, USB_ISO } UsbTransferType;
struct UsbDevice {
    int id;
    char name[16];
    int online;
    UsbTransferType type;
    int errors;
};
struct UsbDevice usb_devices[MAX_USB_DEVICES];
int usb_device_count = 0;

void usb_init() {
    // Enumerate and initialize USB devices
    usb_device_count = 2;
    usb_devices[0].id = 1;
    strcpy(usb_devices[0].name, "kbd");
    usb_devices[0].online = 1;
    usb_devices[0].type = USB_INT;
    usb_devices[0].errors = 0;
    usb_devices[1].id = 2;
    strcpy(usb_devices[1].name, "mouse");
    usb_devices[1].online = 1;
    usb_devices[1].type = USB_INT;
    usb_devices[1].errors = 0;
}

int usb_read_device(int dev, void* buf, int len) {
    // Demo: read from device tree
    for (int i = 0; i < usb_device_count; i++) {
        if (usb_devices[i].id == dev && usb_devices[i].online) {
            memset(buf, usb_devices[i].name[0], len);
            return len;
        }
    }
    return -1;
}

int usb_write_device(int dev, const void* buf, int len) {
    // Demo: write to device tree
    for (int i = 0; i < usb_device_count; i++) {
        if (usb_devices[i].id == dev && usb_devices[i].online) {
            return len;
        }
    }
    return -1;
}

int usb_send(const void* data, int len) {
    // Demo: send bulk transfer
    if (usb_devices[0].online && usb_devices[0].type == USB_BULK) {
        return len;
    }
    usb_devices[0].errors++;
    return -1;
}

int usb_recv(void* buf, int maxlen) {
    // Demo: receive bulk transfer
    if (usb_devices[0].online && usb_devices[0].type == USB_BULK && maxlen > 0) {
        ((char*)buf)[0] = 'U';
        return 1;
    }
    usb_devices[0].errors++;
    return 0;
}

void usb_hotplug(const char* name, UsbTransferType type) {
    // Demo: add new device
    if (usb_device_count < MAX_USB_DEVICES) {
        int id = usb_device_count + 1;
        usb_devices[usb_device_count].id = id;
        strcpy(usb_devices[usb_device_count].name, name);
        usb_devices[usb_device_count].online = 1;
        usb_devices[usb_device_count].type = type;
        usb_devices[usb_device_count].errors = 0;
        usb_device_count++;
    }
}

void usb_stats() {
    for (int i = 0; i < usb_device_count; i++) {
        printf("id=%d name=%s online=%d type=%d errors=%d\n", usb_devices[i].id, usb_devices[i].name, usb_devices[i].online, usb_devices[i].type, usb_devices[i].errors);
    }
}
