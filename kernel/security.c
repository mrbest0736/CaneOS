// CaneOS Security Core
#include <stdint.h>

#define MAX_GROUPS 8
#define MAX_USERS 32

struct Group {
    char name[16];
    int gid;
};


#include <string.h>
#include <stdio.h>
#include <time.h>
#define MAX_SESSIONS 32

struct UserSec {
    char username[16];
    int uid;
    int gid;
    int is_admin;
    char password_hash[32];
};

struct Session {
    int uid;
    char token[32];
    time_t login_time;
    int active;
};

struct AuditLog {
    int uid;
    char action[64];
    time_t timestamp;
};

struct UserSec user_table[MAX_USERS];
struct Group group_table[MAX_GROUPS];
struct Session session_table[MAX_SESSIONS];
struct AuditLog audit_log[256];
int audit_count = 0;
int user_sec_count = 0;
int group_count = 0;
int session_count = 0;

void log_action(int uid, const char* action) {
    if (audit_count < 256) {
        audit_log[audit_count].uid = uid;
        strncpy(audit_log[audit_count].action, action, 63);
        audit_log[audit_count].action[63] = 0;
        audit_log[audit_count].timestamp = time(NULL);
        audit_count++;
    }
}

void hash_password(const char* password, char* out) {
    // Demo: simple hash
    int len = strlen(password);
    for (int i = 0; i < 31; i++) out[i] = (i < len) ? password[i] ^ 0x5A : 0;
    out[31] = 0;
}

int add_user(const char* username, int gid, int is_admin, const char* password) {
    if (user_sec_count >= MAX_USERS) return -1;
    struct UserSec* u = &user_table[user_sec_count];
    strncpy(u->username, username, 15);
    u->username[15] = 0;
    u->uid = user_sec_count;
    u->gid = gid;
    u->is_admin = is_admin;
    hash_password(password, u->password_hash);
    user_sec_count++;
    log_action(u->uid, "add_user");
    return u->uid;
}

int add_group(const char* name) {
    if (group_count >= MAX_GROUPS) return -1;
    struct Group* g = &group_table[group_count];
    strncpy(g->name, name, 15);
    g->name[15] = 0;
    g->gid = group_count;
    group_count++;
    return g->gid;
}

int start_session(int uid, const char* token) {
    if (session_count >= MAX_SESSIONS) return -1;
    session_table[session_count].uid = uid;
    strncpy(session_table[session_count].token, token, 31);
    session_table[session_count].token[31] = 0;
    session_table[session_count].login_time = time(NULL);
    session_table[session_count].active = 1;
    session_count++;
    log_action(uid, "start_session");
    return 0;
}

int end_session(const char* token) {
    for (int i = 0; i < session_count; i++) {
        if (strcmp(session_table[i].token, token) == 0 && session_table[i].active) {
            session_table[i].active = 0;
            log_action(session_table[i].uid, "end_session");
            return 0;
        }
    }
    return -1;
}

int check_permission(int uid, int resource_id, int perm) {
    // Demo: allow admin, deny others
    for (int i = 0; i < user_sec_count; i++) {
        if (user_table[i].uid == uid) {
            if (user_table[i].is_admin) {
                log_action(uid, "check_permission: allow");
                return 1;
            }
            log_action(uid, "check_permission: deny");
            return 0;
        }
    }
    return 0;
}

int escalate_privilege(int uid) {
    for (int i = 0; i < user_sec_count; i++) {
        if (user_table[i].uid == uid) {
            user_table[i].is_admin = 1;
            log_action(uid, "escalate_privilege");
            return 1;
        }
    }
    return 0;
}
