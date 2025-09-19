// CaneOS Network Driver
#include <stdint.h>


#define MAX_NET_DEVICES 8
struct NetDevice {
    char name[16];
    int up;
    int mtu;
    int packets_sent;
    int packets_recv;
    int errors;
};
struct NetDevice net_devices[MAX_NET_DEVICES];
int net_device_count = 0;

void network_init() {
    // Enumerate and initialize network devices
    net_device_count = 2;
    strcpy(net_devices[0].name, "eth0");
    net_devices[0].up = 1;
    net_devices[0].mtu = 1500;
    net_devices[0].packets_sent = 0;
    net_devices[0].packets_recv = 0;
    net_devices[0].errors = 0;
    strcpy(net_devices[1].name, "wlan0");
    net_devices[1].up = 1;
    net_devices[1].mtu = 1500;
    net_devices[1].packets_sent = 0;
    net_devices[1].packets_recv = 0;
    net_devices[1].errors = 0;
}

int network_send(const void* data, int len) {
    // Protocol stack: demo with eth0
    if (net_devices[0].up) {
        net_devices[0].packets_sent++;
        return len;
    }
    net_devices[0].errors++;
    return -1;
}

int network_recv(void* buf, int maxlen) {
    // Protocol stack: demo with eth0
    if (net_devices[0].up && maxlen > 0) {
        ((char*)buf)[0] = 'N';
        net_devices[0].packets_recv++;
        return 1;
    }
    net_devices[0].errors++;
    return 0;
}

void network_stats() {
    for (int i = 0; i < net_device_count; i++) {
        printf("%s: sent=%d recv=%d errors=%d\n", net_devices[i].name, net_devices[i].packets_sent, net_devices[i].packets_recv, net_devices[i].errors);
    }
}
