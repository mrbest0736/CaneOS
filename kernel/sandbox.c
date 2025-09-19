// CaneOS Sandboxing
#include <stdint.h>


#define MAX_SANDBOXES 32
struct Sandbox {
	int pid;
	int allowed_syscalls[32];
	int syscall_count;
	int cpu_quota;
	int mem_quota;
	int namespace_id;
};
struct Sandbox sandbox_table[MAX_SANDBOXES];
int sandbox_count = 0;

int sandbox_init(int pid, int* syscalls, int syscall_count, int cpu_quota, int mem_quota, int namespace_id) {
	if (sandbox_count >= MAX_SANDBOXES) return -1;
	struct Sandbox* s = &sandbox_table[sandbox_count];
	s->pid = pid;
	for (int i = 0; i < syscall_count && i < 32; i++) s->allowed_syscalls[i] = syscalls[i];
	s->syscall_count = syscall_count;
	s->cpu_quota = cpu_quota;
	s->mem_quota = mem_quota;
	s->namespace_id = namespace_id;
	sandbox_count++;
	return 0;
}

int sandbox_check(int pid, int syscall, int resource_id, int mem_used, int cpu_used, int namespace_id) {
	for (int i = 0; i < sandbox_count; i++) {
		struct Sandbox* s = &sandbox_table[i];
		if (s->pid == pid) {
			// Syscall filtering
			int allowed = 0;
			for (int j = 0; j < s->syscall_count; j++) {
				if (s->allowed_syscalls[j] == syscall) allowed = 1;
			}
			// Resource quotas
			if (mem_used > s->mem_quota || cpu_used > s->cpu_quota) return 0;
			// Namespace isolation
			if (s->namespace_id != namespace_id) return 0;
			return allowed;
		}
	}
	return 0;
}
