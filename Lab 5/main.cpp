//
// Created by aaron on 2021-04-05.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

void print_attrs(struct stat sb, char *pathname) {
    printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);
    printf("Link count:               %ld\n", (long) sb.st_nlink);
    printf("Owner's name:             %s\n", getpwuid(sb.st_uid)->pw_name);
    printf("Group name:               %s\n", getgrgid(sb.st_gid)->gr_name);
    printf("File size:                %lld bytes\n", (long long) sb.st_size);
    printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);
    printf("Last file modification:   %s", ctime(&sb.st_mtime));
    printf("Name:                     %s", basename(strdup(pathname)));
}

int main(int argc, char *argv[]) {
    struct stat sb {};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            printf("directory\n");
            break;
        case S_IFLNK:
            printf("symlink\n");
            break;
        case S_IFREG:
            printf("regular file\n");
            break;
        default:
            printf("other\n");
            break;
    }

    print_attrs(sb, argv[1]);

    exit(EXIT_SUCCESS);
}
