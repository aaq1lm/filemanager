#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "scanner.h"

int scan_directory(const char *dir_path, FileDetails **files_out) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char path[MAX_PATH];
    
    int capacity = 10;
    int count = 0;
    FileDetails *files = malloc(capacity * sizeof(FileDetails));
    
    if (!files) return -1;

    dir = opendir(dir_path);
    if (!dir) {
        free(files);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".txt") == 0) {
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
            
            if (stat(path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
                if (count >= capacity) {
                    capacity *= 2;
                    FileDetails *temp = realloc(files, capacity * sizeof(FileDetails));
                    if (!temp) {
                        closedir(dir);
                        free(files);
                        return -1;
                    }
                    files = temp;
                }
                
                strncpy(files[count].filename, entry->d_name, MAX_PATH - 1);
                files[count].filename[MAX_PATH - 1] = '\0';
                strncpy(files[count].filepath, path, MAX_PATH - 1);
                files[count].filepath[MAX_PATH - 1] = '\0';
                files[count].size = file_stat.st_size;
                files[count].mod_time = file_stat.st_mtime;
                count++;
            }
        }
    }

    closedir(dir);
    *files_out = files;
    return count;
}
