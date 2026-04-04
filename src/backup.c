#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "backup.h"

#define BUFFER_SIZE 4096

int backup_files(FileDetails *files, int count, const char *backup_dir) {
    char dest_path[MAX_PATH * 2];
    char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;
    
    struct stat st = {0};
    if (stat(backup_dir, &st) == -1) {
        mkdir(backup_dir, 0700);
    }

    int success_count = 0;

    for (int i = 0; i < count; i++) {
        snprintf(dest_path, sizeof(dest_path), "%s/%s", backup_dir, files[i].filename);
        
        FILE *src = fopen(files[i].filepath, "rb");
        if (!src) continue;
        
        FILE *dest = fopen(dest_path, "wb");
        if (!dest) {
            fclose(src);
            continue;
        }
        
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
            bytes_written = fwrite(buffer, 1, bytes_read, dest);
            if (bytes_written != bytes_read) break;
        }
        
        fclose(src);
        fclose(dest);
        success_count++;
    }
    
    return success_count;
}
