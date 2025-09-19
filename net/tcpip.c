// CaneOS TCP/IP Stack
#include <stdint.h>


#include <string.h>
#include <stdio.h>

#define MAX_TCP_CONN 32
typedef enum { TCP_CLOSED, TCP_LISTEN, TCP_SYN_SENT, TCP_ESTABLISHED, TCP_FIN_WAIT } TcpState;
struct TcpConn {
    int id;
    char remote_ip[16];
    int remote_port;
    TcpState state;
    int secure;
    int bytes_sent;
    int bytes_recv;
};
struct TcpConn tcp_conn_table[MAX_TCP_CONN];
int tcp_conn_count = 0;

void tcpip_init() {
    // Initialize protocol layers and diagnostics
    tcp_conn_count = 0;
    printf("TCP/IP stack initialized\n");
}

int tcpip_open(const char* ip, int port, int secure) {
    if (tcp_conn_count >= MAX_TCP_CONN) return -1;
    struct TcpConn* c = &tcp_conn_table[tcp_conn_count];
    c->id = tcp_conn_count;
    strncpy(c->remote_ip, ip, 15);
    c->remote_ip[15] = 0;
    c->remote_port = port;
    c->state = TCP_ESTABLISHED;
    c->secure = secure;
    c->bytes_sent = 0;
    c->bytes_recv = 0;
    tcp_conn_count++;
    return c->id;
}

int tcpip_send(int conn_id, const void* data, int len) {
    if (conn_id < 0 || conn_id >= tcp_conn_count) return -1;
    struct TcpConn* c = &tcp_conn_table[conn_id];
    if (c->state != TCP_ESTABLISHED) return -1;
    c->bytes_sent += len;
    // Security: demo with secure flag
    if (c->secure) {
        // Demo: pretend to encrypt
    }
    return len;
}

int tcpip_recv(int conn_id, void* buf, int maxlen) {
    if (conn_id < 0 || conn_id >= tcp_conn_count) return -1;
    struct TcpConn* c = &tcp_conn_table[conn_id];
    if (c->state != TCP_ESTABLISHED) return -1;
    if (maxlen > 0) {
        ((char*)buf)[0] = 'T';
        c->bytes_recv++;
        return 1;
    }
    return 0;
}

void tcpip_close(int conn_id) {
    if (conn_id < 0 || conn_id >= tcp_conn_count) return;
    tcp_conn_table[conn_id].state = TCP_CLOSED;
}

void tcpip_stats() {
    for (int i = 0; i < tcp_conn_count; i++) {
        struct TcpConn* c = &tcp_conn_table[i];
        printf("conn %d: %s:%d state=%d sent=%d recv=%d secure=%d\n", c->id, c->remote_ip, c->remote_port, c->state, c->bytes_sent, c->bytes_recv, c->secure);
    }
}
