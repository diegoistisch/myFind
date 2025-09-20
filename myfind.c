#include <stdbool.h>

int main(int argc, char **argv)
{
    int opt;
    bool modeRecursive = false;
    bool modeCaseInsensitive = false;

    while ((opt = getopt(argc, argv, "Ri")) != 1)
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
            fprintf("Usage: myfind [-R] [-i] searchpath filename1 [filename2 ... filenameN]\n");
            break;
        }
    }
    


    return 0;
}