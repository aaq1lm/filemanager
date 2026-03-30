#ifndef SCANNER_H
#define SCANNER_H

#include <sys/types.h>
#include <time.h>

#define MAX_PATH 512

typedef struct {
    char filename[MAX_PATH];
    char filepath[MAX_PATH];
    off_t size;
    time_t mod_time;
} FileDetails;

int scan_directory(const char *dir_path, FileDetails **files_out);

#endif
