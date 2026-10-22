#pragma once

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <direct.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifndef O_CLOEXEC
#define O_CLOEXEC _O_BINARY
#endif
#ifndef _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif
static inline char* mkdtemp(char* pattern) {
    if (!pattern || !strstr(pattern, "XXXXXX")) {
        errno = EINVAL;
        return nullptr;
    }
    const size_t position = size_t(strstr(pattern, "XXXXXX") - pattern);
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    unsigned value = unsigned(GetTickCount64()) ^ unsigned(uintptr_t(pattern));
    for (int attempt = 0; attempt < 100; ++attempt) {
        for (int i = 0; i < 6; ++i) {
            value = value * 1103515245u + 12345u;
            pattern[position + size_t(i)] = alphabet[(value >> 16) % (sizeof(alphabet) - 1)];
        }
        if (_mkdir(pattern) == 0)
            return pattern;
        if (errno != EEXIST)
            return nullptr;
    }
    return nullptr;
}
static inline long long pread(int fd, void* buf, size_t n, long long off) {
    HANDLE h = (HANDLE)_get_osfhandle(fd);
    OVERLAPPED ov = {0};
    ov.Offset = (DWORD)(off & 0xFFFFFFFFull);
    ov.OffsetHigh = (DWORD)((unsigned long long)off >> 32);
    DWORD got = 0;
    if (!ReadFile(h, buf, (DWORD)n, &got, &ov)) {
        if (GetLastError() == ERROR_HANDLE_EOF) return 0;
        return -1;
    }
    return (long long)got;
}
static inline int setenv(const char* k, const char* v, int o) {
    (void)o;
    return _putenv_s(k, v);
}
static inline int unsetenv(const char* k) { return _putenv_s(k, ""); }
#else
#include_next <unistd.h>
#endif
