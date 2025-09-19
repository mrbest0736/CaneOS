
// CaneOS Process Management
#include <stdint.h>

#define MAX_PROCESSES 32


typedef enum { PROC_READY, PROC_RUNNING, PROC_BLOCKED, PROC_TERMINATED, PROC_WAITING, PROC_ZOMBIE } ProcState;

typedef struct IPCMessage {
    int from_pid;
    int to_pid;
    char data[128];
    int size;
} IPCMessage;

struct Process {
    int pid;
    ProcState state;
    uint32_t pc; // Program counter
    uint32_t sp; // Stack pointer
    uint8_t* memory; // Process memory
    int memory_size;
    int parent_pid;
    int uid;
    int gid;
    int priority;
    int signals;
    IPCMessage ipc_queue[8];
    int ipc_count;
};

struct Process process_table[MAX_PROCESSES];
int process_count = 0;
int current_pid = -1;

int create_process(int parent_pid, int uid, uint8_t* code, int code_size) {
    if (process_count >= MAX_PROCESSES) return -1;
    int pid = process_count;
    struct Process* proc = &process_table[pid];
    proc->pid = pid;
    proc->state = PROC_READY;
    proc->pc = 0;
    proc->sp = code_size;
    proc->memory = code;
    proc->memory_size = code_size;
    proc->parent_pid = parent_pid;
    proc->uid = uid;
    proc->gid = 0;
    proc->priority = 1;
    proc->signals = 0;
    proc->ipc_count = 0;
    process_count++;
    return pid;
}

int fork_process(int pid) {
    if (process_count >= MAX_PROCESSES || pid < 0 || pid >= process_count) return -1;
    struct Process* parent = &process_table[pid];
    int child_pid = process_count;
    struct Process* child = &process_table[child_pid];
    *child = *parent;
    child->pid = child_pid;
    child->parent_pid = pid;
    child->state = PROC_READY;
    process_count++;
    return child_pid;
}

int exec_process(int pid, uint8_t* new_code, int new_size) {
    if (pid < 0 || pid >= process_count) return -1;
    struct Process* proc = &process_table[pid];
    proc->memory = new_code;
    proc->memory_size = new_size;
    proc->pc = 0;
    proc->sp = new_size;
    return 0;
}

void send_signal(int pid, int signal) {
    if (pid < 0 || pid >= process_count) return;
    process_table[pid].signals |= (1 << signal);
}

void send_ipc(int from_pid, int to_pid, const char* data, int size) {
    if (to_pid < 0 || to_pid >= process_count) return;
    struct Process* proc = &process_table[to_pid];
    if (proc->ipc_count < 8) {
        IPCMessage* msg = &proc->ipc_queue[proc->ipc_count++];
        msg->from_pid = from_pid;
        msg->to_pid = to_pid;
        int copy_size = size > 128 ? 128 : size;
        for (int i = 0; i < copy_size; i++) msg->data[i] = data[i];
        msg->size = copy_size;
    }
}

int recv_ipc(int pid, char* out, int max_size) {
    if (pid < 0 || pid >= process_count) return 0;
    struct Process* proc = &process_table[pid];
    if (proc->ipc_count > 0) {
        IPCMessage* msg = &proc->ipc_queue[0];
        int copy_size = msg->size > max_size ? max_size : msg->size;
        for (int i = 0; i < copy_size; i++) out[i] = msg->data[i];
        // Shift queue
        for (int i = 1; i < proc->ipc_count; i++) proc->ipc_queue[i-1] = proc->ipc_queue[i];
        proc->ipc_count--;
        return copy_size;
    }
    return 0;
}

void schedule() {
    // Preemptive priority-based scheduler
    int highest = -1, high_prio = -1;
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].state == PROC_READY && process_table[i].priority > high_prio) {
            high_prio = process_table[i].priority;
            highest = i;
        }
    }
    if (highest != -1) {
        process_table[highest].state = PROC_RUNNING;
        current_pid = process_table[highest].pid;
        // ...simulate running...
        process_table[highest].state = PROC_READY;
    }
}

void terminate_process(int pid) {
    if (pid < 0 || pid >= process_count) return;
    process_table[pid].state = PROC_TERMINATED;
}

void context_switch(int from_pid, int to_pid) {
    // Demo: pretend to save/restore context
    if (from_pid < 0 || to_pid < 0 || from_pid >= process_count || to_pid >= process_count) return;
    process_table[from_pid].state = PROC_READY;
    process_table[to_pid].state = PROC_RUNNING;
    current_pid = to_pid;
}
