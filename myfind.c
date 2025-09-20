#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

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

    const char *searchpath = argv[optind++];
    fprintf(stderr, "OK: searchpath='%s', files=%d, -R=%d, -i=%d\n",
            searchpath, argc - optind, modeRecursive, modeCaseInsensitive);

    return 0;
}