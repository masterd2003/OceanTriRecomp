#pragma once

#if defined(_WIN32)
#include <windows.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dirent {
    char d_name[MAX_PATH];
};

struct DIR {
    HANDLE find;
    WIN32_FIND_DATAA data;
    int first;
    int valid;
    struct dirent entry;
};

inline DIR* opendir(const char* path) {
    if (!path)
        return nullptr;

    char pattern[4096];
    const size_t length = strlen(path);
    const char last = length ? path[length - 1] : '\0';

    if (last == '\\' || last == '/')
        snprintf(pattern, sizeof(pattern), "%s*", path);
    else
        snprintf(pattern, sizeof(pattern), "%s\\*", path);

    DIR* dir = static_cast<DIR*>(calloc(1, sizeof(DIR)));

    if (!dir)
        return nullptr;

    dir->find = FindFirstFileA(pattern, &dir->data);

    if (dir->find == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        errno = (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) ? ENOENT : EACCES;
        free(dir);
        return nullptr;
    }

    dir->first = 1;
    dir->valid = 1;
    return dir;
}

inline struct dirent* readdir(DIR* dir) {
    if (!dir || !dir->valid)
        return nullptr;

    if (!dir->first && !FindNextFileA(dir->find, &dir->data)) {
        const DWORD error = GetLastError();
        dir->valid = 0;
        errno = (error == ERROR_NO_MORE_FILES) ? 0 : EIO;
        return nullptr;
    }

    dir->first = 0;
    strncpy(dir->entry.d_name, dir->data.cFileName, MAX_PATH - 1);
    dir->entry.d_name[MAX_PATH - 1] = '\0';
    return &dir->entry;
}

inline int closedir(DIR* dir) {
    if (!dir)
        return -1;

    if (dir->find != INVALID_HANDLE_VALUE)
        FindClose(dir->find);

    free(dir);
    return 0;
}
#else
#include_next <dirent.h>
#endif
