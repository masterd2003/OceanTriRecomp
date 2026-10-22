#pragma once

#if defined(_WIN32)
#include <windows.h>
#include <process.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef HANDLE pthread_t;

typedef struct {
    size_t stacksize;
    int detachstate;
} pthread_attr_t;

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

struct PthreadStart {
    void* (*function)(void*);
    void* argument;
};

static inline unsigned __stdcall PthreadTrampoline(void* value) {
    PthreadStart* start = static_cast<PthreadStart*>(value);
    void* result = start->function(start->argument);
    delete start;
    return static_cast<unsigned>(reinterpret_cast<uintptr_t>(result));
}

static inline int pthread_attr_init(pthread_attr_t* attr) {
    if (!attr)
        return -1;

    memset(attr, 0, sizeof(*attr));
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
    return 0;
}

static inline int pthread_attr_destroy(pthread_attr_t* attr) {
    (void)attr;
    return 0;
}

static inline int pthread_attr_setstacksize(pthread_attr_t* attr, size_t size) {
    if (!attr)
        return -1;

    attr->stacksize = size;
    return 0;
}

static inline int pthread_attr_setdetachstate(pthread_attr_t* attr, int state) {
    if (!attr)
        return -1;

    attr->detachstate = state;
    return 0;
}

static inline int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*function)(void*), void* argument) {
    if (!thread || !function)
        return -1;

    PthreadStart* start = new PthreadStart{function, argument};
    const unsigned stack = attr && attr->stacksize ? static_cast<unsigned>(attr->stacksize) : 0;
    const uintptr_t handle = _beginthreadex(nullptr, stack, PthreadTrampoline, start, 0, nullptr);

    if (!handle) {
        delete start;
        return -1;
    }

    *thread = reinterpret_cast<pthread_t>(handle);

    if (attr && attr->detachstate == PTHREAD_CREATE_DETACHED)
        CloseHandle(*thread);

    return 0;
}

static inline int pthread_join(pthread_t thread, void** result) {
    if (!thread)
        return -1;

    if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED)
        return -1;

    if (result) {
        DWORD code = 0;
        GetExitCodeThread(thread, &code);
        *result = reinterpret_cast<void*>(static_cast<uintptr_t>(code));
    }

    CloseHandle(thread);
    return 0;
}

static inline int pthread_detach(pthread_t thread) {
    if (!thread)
        return -1;

    CloseHandle(thread);
    return 0;
}

static inline pthread_t pthread_self(void) {
    return GetCurrentThread();
}

static inline int pthread_equal(pthread_t left, pthread_t right) {
    return left == right;
}
#else
#include_next <pthread.h>
#endif
