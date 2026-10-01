/**
 * @file syscalls.c
 * @brief Minimal syscalls implementation for STM32 with newlib
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Heap memory
extern char _heap_start;
extern char _heap_end;
static char* _heap_ptr = &_heap_start;

// C++ placement new support
void* operator new(size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) {
    free(ptr);
}

// Syscall implementations
caddr_t _sbrk(int incr) {
    char* prev_heap = _heap_ptr;
    
    if (_heap_ptr + incr > &_heap_end) {
        errno = ENOMEM;
        return (caddr_t)-1;
    }
    
    _heap_ptr += incr;
    return (caddr_t)prev_heap;
}

int _close(int file) {
    return -1;
}

int _fstat(int file) {
    return -1;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char* ptr, int len) {
    return 0;
}

int _write(int file, char* ptr, int len) {
    // TODO: Implement UART output if needed
    return len;
}

int _exit(int status) {
    while (1) {
        __asm__("bkpt #0");
    }
}

void __libc_init_array(void) {
    // Empty implementation
}
