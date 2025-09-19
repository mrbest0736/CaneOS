// CaneOS Firewall
#include <stdint.h>


#include <stdio.h>
#define MAX_FW_RULES 64
typedef enum { FW_ALLOW, FW_BLOCK } FwAction;
struct FwRule {
    int port;
    FwAction action;
    int stateful;
};
struct FwRule fw_rules[MAX_FW_RULES];
int fw_rule_count = 0;

void firewall_init() {
    fw_rule_count = 0;
    printf("Firewall initialized\n");
}

int firewall_add_rule(int port, FwAction action, int stateful) {
    if (fw_rule_count >= MAX_FW_RULES) return -1;
    fw_rules[fw_rule_count].port = port;
    fw_rules[fw_rule_count].action = action;
    fw_rules[fw_rule_count].stateful = stateful;
    fw_rule_count++;
    printf("Firewall rule added: port=%d action=%d stateful=%d\n", port, action, stateful);
    return 0;
}

int firewall_remove_rule(int port) {
    for (int i = 0; i < fw_rule_count; i++) {
        if (fw_rules[i].port == port) {
            for (int j = i; j < fw_rule_count - 1; j++) fw_rules[j] = fw_rules[j + 1];
            fw_rule_count--;
            printf("Firewall rule removed: port=%d\n", port);
            return 0;
        }
    }
    return -1;
}

int firewall_check(int port, int stateful) {
    for (int i = 0; i < fw_rule_count; i++) {
        if (fw_rules[i].port == port && fw_rules[i].stateful == stateful) {
            return fw_rules[i].action == FW_ALLOW ? 1 : 0;
        }
    }
    return 1; // Default allow
}

void firewall_stats() {
    printf("Firewall rules: %d\n", fw_rule_count);
    for (int i = 0; i < fw_rule_count; i++) {
        printf("Rule %d: port=%d action=%d stateful=%d\n", i, fw_rules[i].port, fw_rules[i].action, fw_rules[i].stateful);
    }
}
