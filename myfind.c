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
#include <sys/wait.h>

// Hilfsfunktion zur Anzeige der Benutzung
static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-R] [-i] searchpath filename1 [filename2 ...]\n", prog);
}

// Funktion zum Durchsuchen von Verzeichnissen
void search_files(const char *dirpath, const char *filename, bool case_insensitive, bool recursive) {
    DIR *dirp = opendir(dirpath);
    if (dirp == NULL) {
        perror("Failed to open directory");
        return;
    }

    // Verzeichnisinhalt durchgehen
    struct dirent *direntp;
    
    while ((direntp = readdir(dirp)) != NULL) {
        // Überspringen von "." und ".."
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
            continue;


        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, direntp->d_name);

        // Dateinamenvergleich (davor schauen ob case insensitive)
        int match;
        if (case_insensitive) {
            match = (strcasecmp(direntp->d_name, filename) == 0);
        } else {
            match = (strcmp(direntp->d_name, filename) == 0);
        }
        if (match) {
            printf("%d: %s: %s\n", getpid(), filename, full_path);
        }

        // Rekursive Suche in Unterverzeichnissen
        if (recursive) {
            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                search_files(full_path, filename, case_insensitive, recursive);
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

    //abfrage der optionen -R und -i
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

    // Überprüfen, ob genügend Argumente vorhanden sind
    int remaining = argc - optind;
    if (remaining < 2) {
        usage(argv[0]);
        return 1;
    }

    const char *searchpath = argv[optind];  // Suchpfad pointer
    char absolute_searchpath[PATH_MAX];     // Absoluter Pfad des Suchpfads

    if (realpath(searchpath, absolute_searchpath) == NULL) {
        perror("Failed to resolve absolute path");
        return 1;
    }
  
    char **filenames = &argv[optind + 1];   // Array der Dateinamen
    int num_files = argc - optind - 1; // Anzahl der Dateinamen

    // For schleife um für jede datei einen kindprozess zu erstellen
    for (int i = 0; i < num_files; i++) {
        if (fork() == 0) {
            search_files(absolute_searchpath, filenames[i], modeCaseInsensitive, modeRecursive);
            exit(0);
        }
    }

    // Elternprozess
    for (int i = 0; i < num_files; i++) {
        wait(NULL); //Wartet auf alle Kindprozesse um keine Zombies zu erzeugen
    }

    return 0;
}