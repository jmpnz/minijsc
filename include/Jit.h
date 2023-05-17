//===----------------------------------------------------------------------===//
// Jit.h: This file contains definitions for the classes and functions for JIT
// compilation.
//
// The JIT compiler we implement is a bytecode level tracing JIT. JIT compilers
// typically operaete at the bytecode level and do several transformations on
// the "hot" bytecode before transpiling it to the host's backend machine code
// in our case we chose aarch64 as a backend.
//
// The JIT we implement here has a very compact structure, composed of a JIT
// cache which keeps a cache of jitted bytecode and a reference to the VM.
//
// The engine functions in two phases, during the first phase the bytecode
// executed by the VM is profiled, the profiling of the entire bytecode results
// in traces. Traces are essentially a datastructure that represents a trace
// of execution of the bytecode recording instruction occurences within blocks
// and functions. When a function is marked as hot we compile the traced code
// to machine code and create a function pointer within the JitContext.
//
// JitContext is mainly used to encapsulate the machine code and makes the heap
// allocations required later for execution. We then write the bytecode to the
// allocated memory page and mark it as executable. This context is wwrapped in
// a function that makes the syscalls to get the permissions to execute JIT
// code.
//
// The first phase sets the stage for the second and last phase, execution.
//
// When the VM enters a hot section again (VM keeps pointers to top-k traces)
// it skips the bytecode and makes a function call to the jitted function
// storing the result on the top of the stack.
//===----------------------------------------------------------------------===//
#ifndef JIT_H
#define JIT_H

#include <vector>

#include <cstdlib>

#include <pthread.h>
#include <sys/mman.h>

#define JIT_WRITE_PROTECT(flag) pthread_jit_write_protect_np(flag);
#define CLEAR_CACHE(page)                                                      \
    __builtin_c___clear_cache((char*)page, (char*)page + size);

struct TraceContext {};

struct JitCache {};

struct JitContext {
    // Allocate page aligned executable memory.
    auto alloc(size_t size) -> void* {
        // Memory protection flags.
        int protFlags = PROT_READ | PROT_WRITE | PROD_EXEC;
        // Process mapping flags.
        int mapFlags = MAP_JIT | MAP_PRIVATE | MAP_ANONYMOUS;
        // Allocate a memory page.
        void* page = mmap(NULL, size, protFlags, mapFlags, -1, 0);
        // If allocation fails we should throw an exception. But for now
        // let's exit with status code 1.
        if (page == MAP_FAILED || !page) {
            exit(1);
        }
        // Return a pointer to the allocated page.
        return page;
    }

    // Write encoded instructions into memory page.
    auto writeInst(std::vector<uint32_t> code, void* page) -> void {
        // Set pthread JIT write protection to false.
        JIT_WRITE_PROTECT(false);
        // Write code to executable memory.
        std::memcpy(page, code.data(), code.size());
        // Flush instruction cache within the memory page to ensure
        // deterministic code.
        CLEAR_CACHE(code);
        // Enable pthread JIT write protection
        JIT_WRITE_PROTECT(true);
    }
};

#endif
