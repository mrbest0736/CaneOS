// CaneOS Routing Module
#include <stdint.h>


#include <stdio.h>
#define MAX_ROUTES 128
typedef enum { ROUTE_STATIC, ROUTE_DYNAMIC } RouteType;
struct RouteEntry {
    int dest;
    int gateway;
    int iface;
    RouteType type;
    int metric;
};
struct RouteEntry route_table[MAX_ROUTES];
int route_count = 0;

void routing_init() {
    route_count = 0;
    printf("Routing initialized\n");
}

int routing_add_route(int dest, int gateway, int iface, RouteType type, int metric) {
    if (route_count >= MAX_ROUTES) return -1;
    route_table[route_count].dest = dest;
    route_table[route_count].gateway = gateway;
    route_table[route_count].iface = iface;
    route_table[route_count].type = type;
    route_table[route_count].metric = metric;
    route_count++;
    printf("Route added: dest=%d gateway=%d iface=%d type=%d metric=%d\n", dest, gateway, iface, type, metric);
    return 0;
}

int routing_remove_route(int dest) {
    for (int i = 0; i < route_count; i++) {
        if (route_table[i].dest == dest) {
            for (int j = i; j < route_count - 1; j++) route_table[j] = route_table[j + 1];
            route_count--;
            printf("Route removed: dest=%d\n", dest);
            return 0;
        }
    }
    return -1;
}

int routing_lookup(int dest, int iface) {
    int best_metric = 1<<30, best_gateway = -1;
    for (int i = 0; i < route_count; i++) {
        if (route_table[i].dest == dest && route_table[i].iface == iface && route_table[i].metric < best_metric) {
            best_metric = route_table[i].metric;
            best_gateway = route_table[i].gateway;
        }
    }
    return best_gateway;
}

void routing_stats() {
    printf("Routes: %d\n", route_count);
    for (int i = 0; i < route_count; i++) {
        printf("Route %d: dest=%d gateway=%d iface=%d type=%d metric=%d\n", i, route_table[i].dest, route_table[i].gateway, route_table[i].iface, route_table[i].type, route_table[i].metric);
    }
}
