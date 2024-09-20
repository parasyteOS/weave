#pragma once

#include <stdio.h>
#include <memory>

// Copy from Magisk/native/src/base/files.{hpp, cpp}
using sFILE = std::unique_ptr<FILE, decltype(&fclose)>;
static inline sFILE make_file(FILE *fp) {
    return sFILE(fp, [](FILE *fp){ return fp ? fclose(fp) : 1; });
}
static inline sFILE xopen_file(const char *path, const char *mode) {
    return make_file(fopen(path, mode));
}
static inline sFILE xopen_file(int fd, const char *mode) {
    return make_file(fdopen(fd, mode));
}
