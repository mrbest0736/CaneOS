// CaneOS Ethernet Driver
#include <stdint.h>


#include <string.h>
#include <stdio.h>

static char mac_address[18] = "00:11:22:33:44:55";
static int link_up = 1;
static int frames_sent = 0;
static int frames_recv = 0;

void ethernet_init() {
    // Initialize MAC address and link status
    strcpy(mac_address, "00:11:22:33:44:55");
    link_up = 1;
    frames_sent = 0;
    frames_recv = 0;
    printf("Ethernet initialized. MAC: %s\n", mac_address);
}

void ethernet_set_mac(const char* mac) {
    strncpy(mac_address, mac, 17);
    mac_address[17] = 0;
}

void ethernet_set_link(int up) {
    link_up = up;
}

int ethernet_send(const void* data, int len) {
    if (!link_up) return -1;
    frames_sent++;
    return len;
}

int ethernet_recv(void* buf, int maxlen) {
    if (!link_up || maxlen <= 0) return 0;
    ((char*)buf)[0] = 'E';
    frames_recv++;
    return 1;
}

void ethernet_stats() {
    printf("Ethernet MAC: %s Link: %d Sent: %d Recv: %d\n", mac_address, link_up, frames_sent, frames_recv);
}
