#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <strings.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/stat.h>

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-R] [-i] searchpath filename1 [filename2 ...]\n", prog);
}

void search_files(const char *dirpath, char **filenames, int num_files, bool case_insensitive, bool recursive) {
    DIR *dirp = opendir(dirpath);
    if (dirp == NULL) {
        perror("Failed to open directory");
        return;
    }

    struct dirent *direntp;
    while ((direntp = readdir(dirp)) != NULL) {
        
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
            continue;

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, direntp->d_name);

        for (int i = 0; i < num_files; i++) {
            int match;
            if (case_insensitive) {
                match = (strcasecmp(direntp->d_name, filenames[i]) == 0);
            } else {
                match = (strcmp(direntp->d_name, filenames[i]) == 0);
            }
            if (match) {
                //printf("%s\n", full_path);
            }
        }

        if (recursive) {
            struct stat statbuf;
                if (stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                    search_files(full_path, filenames, num_files, case_insensitive, recursive);
                }

        }
    }
    closedir(dirp);
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
            usage(argv[0]);
            return 1;
        }
    }
    
    int remaining = argc - optind;
    if (remaining < 2) {
        usage(argv[0]);
        return 1;
    }

    //Debug Output
    const char *searchpath = argv[optind];
    fprintf(stderr, "OK: searchpath='%s', files=%d, -R=%d, -i=%d\n",
            searchpath, argc - optind - 1, modeRecursive, modeCaseInsensitive);

    char absolute_searchpath[PATH_MAX];
    if (realpath(searchpath, absolute_searchpath) == NULL) {
        perror("Failed to resolve absolute path");
        return 1;
    }

    char **filenames = &argv[optind + 1];
    int num_files = argc - optind - 1;

    for (int i = optind; i < argc; ++i) {
        if(fork() == 0) {
            // child process
            // do something with argv[i]
            printf("%d : %s : %s\n", getpid(), filenames[i], absolute_searchpath);
            exit(0); // terminate child process
        }
    }
    search_files(absolute_searchpath, filenames, num_files, modeCaseInsensitive, modeRecursive);
    
    
    return 0;
}