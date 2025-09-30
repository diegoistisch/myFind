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
void search_files(const char *dirpath, const char *filename, bool case_insensitive, bool recursive, FILE *writing) {
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

        // Dateinamenvergleich
        int match;
        if (case_insensitive) {
            match = (strcasecmp(direntp->d_name, filename) == 0);
        } else {
            match = (strcmp(direntp->d_name, filename) == 0);
        }
        if (match) {
            fprintf(writing, "%d: %s: %s\n", getpid(), filename, full_path);
            fflush(writing);
        }

        // Rekursive Suche in Unterverzeichnissen
        if (recursive) {
            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                search_files(full_path, filename, case_insensitive, recursive, writing);
            }
        }
    }

    // closedir() wiederholen falls durch Signal unterbrochen
    while ((closedir(dirp) == -1) && (errno == EINTR));
}

/**
 * Synchronisation der Kindprozesse über eine Pipe
 * Jeder Kindprozess schreibt seine Ergebnisse in die Pipe
 * Der Elternprozess liest die Ergebnisse aus der Pipe mit fgets
 * und gibt sie aus mit fputs und fflush(stdout)
 */
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

    const char *searchpath = argv[optind];  // Suchpfad speichern
    char absolute_searchpath[PATH_MAX];     // Puffer für den Suchpfad

    // realpath wandelt den pfad in einen standardisierten, absoluten pfad um
    if (realpath(searchpath, absolute_searchpath) == NULL) {
        perror("Failed to get absolute path");
        return 1;
    }
  
    char **filenames = &argv[optind + 1];   // Array der Dateinamen
    int num_files = argc - optind - 1; // Anzahl der Dateinamen

    // Eine gemeinsame Pipe für alle Kindprozesse erstellen
    int fd[2];
    if (pipe(fd) < 0) {
        perror("pipe failed");
        return 1;
    }

    // For schleife um für jede datei einen kindprozess zu erstellen
    for (int i = 0; i < num_files; i++) {
        if (fork() == 0) {
            // Kindprozess
            if (close(fd[0]) == -1) { // Lesende Seite schließen
                perror("close");
                exit(1);
            }

            // Schreibende Seite der Pipe verwenden
            FILE *writing = fdopen(fd[1], "w");
            if (writing == NULL) {
                perror("fdopen");
                exit(1);
            }

            search_files(absolute_searchpath, filenames[i], modeCaseInsensitive, modeRecursive, writing);
            fclose(writing); // Schließt auch fd[1]
            exit(0);
        }
    }

    // Elternprozess 
    // Schreibende Seite schließen
    if (close(fd[1]) == -1) {
        perror("close");
        return 1;
    }

    // FILE-Pointer für lesende Seite erstellen
    FILE *reading = fdopen(fd[0], "r");
    if (reading == NULL) {
        perror("fdopen");
        return 1;
    }

    // Elternprozess
    // Liest Ergebnisse von der Pipe bis alle Kinder fertig sind
    char buffer[PIPE_BUF];
    while (fgets(buffer, PIPE_BUF, reading) != NULL) {
        fputs(buffer, stdout); //text in buffer
        fflush(stdout);     // leert den buffer (sofortige Ausgabe)
    }
    fclose(reading); // Schließt auch fd[0]

    // Wartet auf alle Kindprozesse um keine Zombies zu erzeugen
    for (int i = 0; i < num_files; i++) {
        wait(NULL);
    }

    return 0;
}