// CaneOS Memory Management
#include <stdint.h>
#include <stddef.h>


#define MEM_TOTAL_SIZE (1024 * 1024) // 1MB for demo
static uint8_t memory_pool[MEM_TOTAL_SIZE];
static int mem_used = 0;

// Simple free list for demo
typedef struct MemBlock {
    void* addr;
    int size;
    int free;
} MemBlock;

#define MAX_BLOCKS 128
static MemBlock block_table[MAX_BLOCKS];
static int block_count = 0;

void* alloc_mem(int size) {
    // First-fit allocation
    for (int i = 0; i < block_count; i++) {
        if (block_table[i].free && block_table[i].size >= size) {
            block_table[i].free = 0;
            return block_table[i].addr;
        }
    }
    if (mem_used + size > MEM_TOTAL_SIZE || block_count >= MAX_BLOCKS) return NULL;
    void* ptr = &memory_pool[mem_used];
    block_table[block_count].addr = ptr;
    block_table[block_count].size = size;
    block_table[block_count].free = 0;
    block_count++;
    mem_used += size;
    return ptr;
}

void free_mem(void* ptr) {
    for (int i = 0; i < block_count; i++) {
        if (block_table[i].addr == ptr) {
            block_table[i].free = 1;
            return;
        }
    }
}

// Simple paging system
#define PAGE_SIZE 4096
int get_page_num(void* addr) {
    return ((uint8_t*)addr - memory_pool) / PAGE_SIZE;
}

void* get_page_addr(int page_num) {
    if (page_num < 0 || page_num * PAGE_SIZE >= MEM_TOTAL_SIZE) return NULL;
    return &memory_pool[page_num * PAGE_SIZE];
}

// Memory protection: demo with read/write flags
typedef struct PageProt {
    int page_num;
    int read;
    int write;
} PageProt;

static PageProt page_prot_table[MEM_TOTAL_SIZE / PAGE_SIZE];

void set_page_protection(int page_num, int read, int write) {
    if (page_num < 0 || page_num >= MEM_TOTAL_SIZE / PAGE_SIZE) return;
    page_prot_table[page_num].page_num = page_num;
    page_prot_table[page_num].read = read;
    page_prot_table[page_num].write = write;
}

int check_page_access(void* addr, int write) {
    int page_num = get_page_num(addr);
    if (page_num < 0 || page_num >= MEM_TOTAL_SIZE / PAGE_SIZE) return 0;
    if (write) return page_prot_table[page_num].write;
    return page_prot_table[page_num].read;
}
