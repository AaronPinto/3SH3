//
// Created by aaron on 2021-04-05.
//
#define GREEN "\x1B[1;32m"
#define CYAN "\x1B[1;36m"
#define NORMAL_COLOUR "\x1B[0m"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

void print_dir_attrs(const char *pathname, struct stat &sb) {
    printf("%s", NORMAL_COLOUR);
    printf("%sMode: %6lo   ", GREEN, (unsigned long) sb.st_mode);
    printf("%sLink count: %-4ld", GREEN, (long) sb.st_nlink);
    printf("%sOwner's name: %-8s", GREEN, getpwuid(sb.st_uid)->pw_name);
    printf("%sGroup name: %-8s", GREEN, getgrgid(sb.st_gid)->gr_name);
    printf("%sFile size: %6lld bytes   ", GREEN, (long long) sb.st_size);
    printf("%sBlocks allocated: %-6lld", GREEN, (long long) sb.st_blocks);

    char buf[200];
    strftime(buf, sizeof(buf), "%b %e %H:%M", localtime(&sb.st_mtime));
    printf("%sLast file modification: %-15s", GREEN, buf);

    printf("%sName: %s\n", GREEN, basename(strdup(pathname)));
    printf("%s", NORMAL_COLOUR);
}

void print_sym_attrs(const char *pathname, struct stat &sb) {
    printf("%s", NORMAL_COLOUR);
    printf("%sMode: %6lo   ", CYAN, (unsigned long) sb.st_mode);
    printf("%sLink count: %-4ld", CYAN, (long) sb.st_nlink);
    printf("%sOwner's name: %-8s", CYAN, getpwuid(sb.st_uid)->pw_name);
    printf("%sGroup name: %-8s", CYAN, getgrgid(sb.st_gid)->gr_name);
    printf("%sFile size: %6lld bytes   ", CYAN, (long long) sb.st_size);
    printf("%sBlocks allocated: %-6lld", CYAN, (long long) sb.st_blocks);

    char buf[200];
    strftime(buf, sizeof(buf), "%b %e %H:%M", localtime(&sb.st_mtime));
    printf("%sLast file modification: %-15s", CYAN, buf);

    printf("%sName: %s\n", CYAN, basename(strdup(pathname)));
    printf("%s", NORMAL_COLOUR);
}

void print_file_attrs(const char *pathname, struct stat &sb) {
    printf("%s", NORMAL_COLOUR);
    printf("Mode: %6lo   ", (unsigned long) sb.st_mode);
    printf("Link count: %-4ld", (long) sb.st_nlink);
    printf("Owner's name: %-8s", getpwuid(sb.st_uid)->pw_name);
    printf("Group name: %-8s", getgrgid(sb.st_gid)->gr_name);
    printf("File size: %6lld bytes   ", (long long) sb.st_size);
    printf("Blocks allocated: %-6lld", (long long) sb.st_blocks);

    char buf[200];
    strftime(buf, sizeof(buf), "%b %e %H:%M", localtime(&sb.st_mtime));
    printf("Last file modification: %-15s", buf);

    printf("Name: %s\n", basename(strdup(pathname)));
    printf("%s", NORMAL_COLOUR);
}

void list_dir(const char *pathname, struct stat &sb) {
    DIR *dir;
    char *cwd;

    if (lstat(pathname, &sb) == -1) {
        perror("lstat");
        return;
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            printf("\n");
            print_dir_attrs(pathname, sb);

            if (!(dir = opendir(pathname))) {
                perror("opendir");
                return;
            }

            if (chdir(pathname)) {
                perror("chdir");
                return;
            }

            while (struct dirent *entry = readdir(dir)) {
                // Check if it's current dir or parent dir
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                    continue;
                }

                cwd = getcwd(nullptr, 0);

                char *full_path = new char[1024];
                strcpy(full_path, cwd);
                strcat(full_path, "/");
                strcat(full_path, entry->d_name);

                list_dir(full_path, sb);
            }

            chdir("..");
            closedir(dir);
            break;
        case S_IFLNK:
            print_sym_attrs(pathname, sb);
            break;
        case S_IFREG:
            print_file_attrs(pathname, sb);
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    struct stat sb {};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            list_dir(argv[1], sb);
            break;
        case S_IFLNK:
            print_sym_attrs(argv[1], sb);
            break;
        case S_IFREG:
            print_file_attrs(argv[1], sb);
            break;
        default:
            printf("type not supported\n");
            break;
    }

    exit(EXIT_SUCCESS);
}
