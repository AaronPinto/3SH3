// SOURCE: https://stackoverflow.com/a/29402705/6713362

/* We want POSIX.1-2008 + XSI, i.e. SuSv4, features */
#define _XOPEN_SOURCE 700

/* Added on 2017-06-25:
   If the C library can support 64-bit file sizes
   and offsets, using the standard names,
   these defines tell the C library to do so. */
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <cerrno>
#include <ftw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

/* POSIX.1 says each process has at least 20 file descriptors.
 * Three of those belong to the standard streams.
 * Here, we use a conservative estimate of 15 available;
 * assuming we use at most two for other uses in this program,
 * we should never run into any problems.
 * Most trees are shallower than that, so it is efficient.
 * Deeper trees are traversed fine, just a bit slower.
 * (Linux allows typically hundreds to thousands of open files,
 *  so you'll probably never see any issues even if you used
 *  a much higher value, say a couple of hundred, but
 *  15 is a safe, reasonable value.)
*/
#ifndef USE_FDS
#define USE_FDS 15
#endif

int print_entry(const char *filepath, const struct stat *info, const int typeflag, struct FTW *pathinfo) {
    /* const char *const filename = filepath + pathinfo->base; */
    const auto bytes = (double) info->st_size; /* Not exact if large! */
    struct tm mtime {};

    localtime_r(&(info->st_mtime), &mtime);

    printf("%04d-%02d-%02d %02d:%02d:%02d", mtime.tm_year + 1900, mtime.tm_mon + 1, mtime.tm_mday, mtime.tm_hour, mtime.tm_min,
           mtime.tm_sec);
    printf(" %8.0f B  ", bytes);

    if (typeflag == FTW_SL) {
        char *target;
        size_t maxlen = 1023;
        ssize_t len;

        while (true) {
            target = static_cast<char *>(malloc(maxlen + 1));
            if (target == nullptr) {
                return ENOMEM;
            }

            len = readlink(filepath, target, maxlen);

            if (len == (ssize_t) -1) {
                const int saved_errno = errno;
                free(target);
                return saved_errno;
            }

            if (len >= (ssize_t) maxlen) {
                free(target);
                maxlen += 1024;
                continue;
            }

            target[len] = '\0';
            break;
        }

        printf(" %s -> %s\n", filepath, target);
        free(target);
    } else if (typeflag == FTW_SLN) {
        printf(" %s (dangling symlink)\n", filepath);
    } else if (typeflag == FTW_F) {
        printf(" %s\n", filepath);
    } else if (typeflag == FTW_D || typeflag == FTW_DP) {
        printf(" %s/\n", filepath);
    } else if (typeflag == FTW_DNR) {
        printf(" %s/ (unreadable)\n", filepath);
    } else {
        printf(" %s (unknown)\n", filepath);
    }

    return 0;
}


int print_directory_tree(const char *const dirpath) {
    /* Invalid directory path? */
    if (dirpath == nullptr || *dirpath == '\0') {
        return errno = EINVAL;
    }

    int result = nftw(dirpath, print_entry, USE_FDS, FTW_PHYS);
    if (result >= 0) {
        errno = result;
    }

    return errno;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        if (print_directory_tree(".")) {
            fprintf(stderr, "%s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        for (int arg = 1; arg < argc; arg++) {
            if (print_directory_tree(argv[arg])) {
                fprintf(stderr, "%s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}