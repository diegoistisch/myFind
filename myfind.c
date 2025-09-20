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
            fprintf(modeRecursive ? stderr : stdout, "Recursive mode is not implemented yet.\n");
            break;
        case 'i':
            modeCaseInsensitive = true;
            fprintf(modeCaseInsensitive ? stderr : stdout, "Case-insensitive mode is not implemented yet.\n");
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

    struct dirent *direntp;
    DIR *dirp;
    if ((dirp = opendir(argv[1])) == NULL)
    {
      perror("Failed to open directory");
      return 1;
    }

    while ((direntp = readdir(dirp)) != NULL)
      printf("%s\n", direntp->d_name);
    while ((closedir(dirp) == -1) && (errno == EINTR))
      ;
    /** 
    const char *searchpath = argv[optind++];
    fprintf(stderr, "OK: searchpath='%s', files=%d, -R=%d, -i=%d\n",
            searchpath, argc - optind, modeRecursive, modeCaseInsensitive);

    // Durchsuche das Verzeichnis
    DIR *dirp = opendir(searchpath);
    if ((dirp = opendir(argv[1])) == NULL)
   {
      perror("Failed to open directory");
      return 1;
   }

    
    struct dirent *direntp;
    
    while ((direntp = readdir(dirp)) != NULL) {
        // Ignoriere . und ..
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
            continue;

        // Vergleiche mit allen gesuchten Dateinamen
        for (int i = optind; i < argc; ++i) {
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
    */

    return 0;
}