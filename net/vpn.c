// CaneOS VPN Module
#include <stdint.h>


#include <string.h>
#include <stdio.h>

typedef enum { VPN_PROTO_OPENVPN, VPN_PROTO_WIREGUARD } VpnProto;
struct VpnSession {
    char server[64];
    VpnProto proto;
    int encrypted;
    int connected;
};
struct VpnSession vpn_sessions[4];
int vpn_session_count = 0;

void vpn_init() {
    vpn_session_count = 0;
    printf("VPN module initialized\n");
}

int vpn_connect(const char* server, VpnProto proto, int encrypted) {
    if (vpn_session_count >= 4) return -1;
    struct VpnSession* s = &vpn_sessions[vpn_session_count];
    strncpy(s->server, server, 63);
    s->server[63] = 0;
    s->proto = proto;
    s->encrypted = encrypted;
    s->connected = 1;
    vpn_session_count++;
    printf("VPN connected to %s using %s encryption=%d\n", server, proto == VPN_PROTO_OPENVPN ? "OpenVPN" : "WireGuard", encrypted);
    return 1;
}

void vpn_disconnect(int idx) {
    if (idx < 0 || idx >= vpn_session_count) return;
    vpn_sessions[idx].connected = 0;
    printf("VPN disconnected from %s\n", vpn_sessions[idx].server);
}

void vpn_stats() {
    for (int i = 0; i < vpn_session_count; i++) {
        struct VpnSession* s = &vpn_sessions[i];
        printf("Session %d: %s proto=%d encrypted=%d connected=%d\n", i, s->server, s->proto, s->encrypted, s->connected);
    }
}
