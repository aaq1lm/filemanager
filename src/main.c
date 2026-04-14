#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "scanner.h"
#include "backup.h"

int pipe_fd[2];
pid_t child_pid = -1;

void handle_sigint(int sig) {
    (void)sig;
    if (child_pid > 0) {
        kill(child_pid, SIGKILL);
        waitpid(child_pid, NULL, 0);
    }
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    const char *scan_dir = argv[1];
    const char *backup_dir = "./backup";

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    if (pipe(pipe_fd) == -1) return 1;

    child_pid = fork();
    if (child_pid == -1) return 1;

    if (child_pid > 0) {
        close(pipe_fd[0]);

        FileDetails *files = NULL;
        int count = scan_directory(scan_dir, &files);

        if (count >= 0) {
            backup_files(files, count, backup_dir);
            write(pipe_fd[1], &count, sizeof(int));
            for (int i = 0; i < count; i++) {
                write(pipe_fd[1], &files[i], sizeof(FileDetails));
            }
        }
        
        close(pipe_fd[1]);
        if (files) free(files);
        waitpid(child_pid, NULL, 0);
    } else {
        close(pipe_fd[1]);

        int count = 0;
        if (read(pipe_fd[0], &count, sizeof(int)) > 0) {
            printf("Total .txt files detected: %d\n\n", count);
            for (int i = 0; i < count; i++) {
                FileDetails fd;
                if (read(pipe_fd[0], &fd, sizeof(FileDetails)) > 0) {
                    printf("File: %s\nSize: %ld bytes\n", fd.filename, (long)fd.size);
                    char time_str[64];
                    struct tm *tm_info = localtime(&fd.mod_time);
                    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
                    printf("Last Modified: %s\n---------------------------\n", time_str);
                }
            }
        }
        close(pipe_fd[0]);
        exit(0);
    }
    return 0;
}
