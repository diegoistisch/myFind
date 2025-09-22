#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <strings.h>
#include <errno.h>

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-R] [-i] searchpath filename1 [filename2 ...]\n", prog);
}

int main(int argc, char **argv)
{
    int opt;
    bool modeRecursive = false;
    bool modeCaseInsensitive = false;

    while ((opt = getopt(argc, argv, "Ri")) != -1)
    {
        switch (opt)
        {
        case 'R':
            modeRecursive = true;
            break;
        case 'i':
            modeCaseInsensitive = true;
            break;
        
        default:
            fprintf(stderr, "Usage: %s [-R] [-i] searchpath filename1 [filename2 ...]\n", argv[0]);
            return 1;
        }
    }

    int remaining = argc - optind;
    if (remaining < 2) {
        usage(argv[0]);
        return 1;
    }

    const char *searchpath = argv[optind];
    fprintf(stderr, "OK: searchpath='%s', files=%d, -R=%d, -i=%d\n",
            searchpath, argc - optind - 1, modeRecursive, modeCaseInsensitive);

    DIR *dirp = opendir(searchpath);
    if (dirp == NULL) {
        perror("Failed to open directory");
        return 1;
    }

    struct dirent *direntp;

    while ((direntp = readdir(dirp)) != NULL) {
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
            continue;

        for (int i = optind + 1; i < argc; ++i) {
            int match;
            if (modeCaseInsensitive) {
                match = (strcasecmp(direntp->d_name, argv[i]) == 0);
            } else {
                match = (strcmp(direntp->d_name, argv[i]) == 0);
            }
            if (match) {
                printf("%s/%s\n", searchpath, direntp->d_name);
            }
        }
    }
    closedir(dirp);

    return 0;
}