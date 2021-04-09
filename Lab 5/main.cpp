//
// Created by aaron on 2021-04-05.
//
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

void print_attrs(struct stat &sb, const char *pathname) {
    printf("Mode: %lo (octal)\t\t", (unsigned long) sb.st_mode);
    printf("Link count: %ld\t", (long) sb.st_nlink);
    printf("Owner's name: %s\t", getpwuid(sb.st_uid)->pw_name);
    printf("Group name: %s\t", getgrgid(sb.st_gid)->gr_name);
    printf("File size: %lld bytes\t\t", (long long) sb.st_size);
    printf("Blocks allocated: %lld\t", (long long) sb.st_blocks);

    char buf[200];
    strftime(buf, sizeof(buf), "%b %e %H:%M", localtime(&sb.st_mtime));
    printf("Last file modification: %s\t", buf);

    printf("Name: %s\n", basename(strdup(pathname)));
}

void list_dir(char *pathname, struct stat &sb, int indent) {
    DIR *dir;
    char *cwd;

    if (lstat(pathname, &sb) == -1) {
        perror("lstat");
        return;
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            print_attrs(sb, pathname);

            if (!(dir = opendir(pathname))) {
                perror("opendir");
                return;
            }

            if (chdir(pathname)) {
                perror("chdir");
                return;
            }

            while (struct dirent *entry = readdir(dir)) {
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                    continue;
                }

                cwd = getcwd(nullptr, 0);

                char *full_path = new char[1024];
                strcpy(full_path, cwd);
                strcat(full_path, "/");
                strcat(full_path, entry->d_name);

                list_dir(full_path, sb, indent + 4);
            }

            chdir("..");
            closedir(dir);
            break;
        case S_IFLNK:
        case S_IFREG:
            print_attrs(sb, pathname);
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    struct stat sb {};
    int indent = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR: {
            list_dir(argv[1], sb, indent);
            break;
        }
        case S_IFLNK:
        case S_IFREG:
            print_attrs(sb, argv[1]);
            break;
        default:
            printf("type not supported\n");
            break;
    }

    exit(EXIT_SUCCESS);
}
