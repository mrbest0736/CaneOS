// CaneOS Access Control Lists
#include <stdint.h>

#define MAX_RESOURCES 64
#define MAX_ACL_ENTRIES 128


typedef enum { RES_FILE, RES_DEVICE, RES_PROCESS } ResourceType;
struct ACL {
    int resource_id;
    ResourceType type;
    int uid;
    int permissions; // Bitmask: read/write/execute/admin
    int parent_resource_id;
};

struct ACL acl_table[MAX_ACL_ENTRIES];
int acl_count = 0;

int set_acl(int resource_id, ResourceType type, int uid, int permissions, int parent_resource_id) {
    if (acl_count >= MAX_ACL_ENTRIES) return -1;
    struct ACL* a = &acl_table[acl_count];
    a->resource_id = resource_id;
    a->type = type;
    a->uid = uid;
    a->permissions = permissions;
    a->parent_resource_id = parent_resource_id;
    acl_count++;
    return 0;
}

int get_acl(int resource_id, int uid) {
    // Fine-grained and inherited permissions
    for (int i = 0; i < acl_count; i++) {
        if (acl_table[i].resource_id == resource_id && acl_table[i].uid == uid) {
            return acl_table[i].permissions;
        }
    }
    // Inheritance: check parent
    for (int i = 0; i < acl_count; i++) {
        if (acl_table[i].resource_id == resource_id) {
            int parent = acl_table[i].parent_resource_id;
            if (parent != -1) {
                return get_acl(parent, uid);
            }
        }
    }
    return 0;
}
