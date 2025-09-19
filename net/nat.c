// CaneOS NAT Module
#include <stdint.h>

void nat_init() {
    // Demo: pretend to initialize NAT
}

#include <stdio.h>
#define MAX_NAT_ENTRIES 128
struct NatEntry {
    int src_port;
    int dst_port;
    int iface;
    int session_id;
};
struct NatEntry nat_table[MAX_NAT_ENTRIES];
int nat_count = 0;

void nat_init() {
    nat_count = 0;
    printf("NAT initialized\n");
}

int nat_map_port(int src_port, int dst_port, int iface, int session_id) {
    if (nat_count >= MAX_NAT_ENTRIES) return -1;
    nat_table[nat_count].src_port = src_port;
    nat_table[nat_count].dst_port = dst_port;
    nat_table[nat_count].iface = iface;
    nat_table[nat_count].session_id = session_id;
    nat_count++;
    printf("NAT mapping added: src=%d dst=%d iface=%d session=%d\n", src_port, dst_port, iface, session_id);
    return 0;
}

int nat_unmap_port(int src_port, int dst_port) {
    for (int i = 0; i < nat_count; i++) {
        if (nat_table[i].src_port == src_port && nat_table[i].dst_port == dst_port) {
            for (int j = i; j < nat_count - 1; j++) nat_table[j] = nat_table[j + 1];
            nat_count--;
            printf("NAT mapping removed: src=%d dst=%d\n", src_port, dst_port);
            return 0;
        }
    }
    return -1;
}

int nat_lookup(int src_port, int iface) {
    for (int i = 0; i < nat_count; i++) {
        if (nat_table[i].src_port == src_port && nat_table[i].iface == iface) {
            return nat_table[i].dst_port;
        }
    }
    return -1;
}

void nat_stats() {
    printf("NAT entries: %d\n", nat_count);
    for (int i = 0; i < nat_count; i++) {
        printf("Entry %d: src=%d dst=%d iface=%d session=%d\n", i, nat_table[i].src_port, nat_table[i].dst_port, nat_table[i].iface, nat_table[i].session_id);
    }
}

int nat_translate(int src_ip, int dst_ip) {
    // Demo: always translate src_ip to dst_ip
    return dst_ip;
}
