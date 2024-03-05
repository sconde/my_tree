#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

// Colors
#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define DEFAULT "\x1b[39m"
#define BLUE "\x1b[34m"
#define CYAN "\x1b[36m"

// Function to print the tree of the directory, recursively
void tree(char *path, int dashes, int spaces, int level)
{
    DIR *dp = opendir(path);
    struct dirent *direc_prt;

    while ((direc_prt = readdir(dp)) != NULL)
    {
        if ((strcmp(direc_prt->d_name, ".") == 0) || (strcmp(direc_prt->d_name, "..") == 0))
        {
            continue;
        }

        char *newppath = (char *)malloc(strlen(path) + strlen(direc_prt->d_name) + 2); // room for a slash and a null terminator
        sprintf(newppath, "%s/%s", path, direc_prt->d_name);
        printf("\n");

        if (spaces > 0)
        {
            for (int lvl = 0; lvl < level; lvl++)
            {
                printf("│");
                for (int spc = 0; spc < spaces; spc++)
                {
                    printf(" ");
                }
            }
        }
        printf("|");
        for (int place = 0; place < dashes; place++)
        {
            printf("-");
        }
        printf(" ");

        if ((direc_prt->d_type == DT_DIR) && (access(newppath, R_OK) == 0))
        {
            printf("%s%s%s", BLUE, direc_prt->d_name, RESET);
            tree(newppath, dashes, spaces, level + 1);
            free(newppath);
            continue;
        }

        struct stat st;
        int r = lstat(newppath, &st); // lstat to avoid following symlinks
        if (r == -1)
        {
            perror("lstat");
            exit(1);
        }
        if (S_ISLNK(st.st_mode))
        {
            printf("%s%s%s -> %lld", CYAN, direc_prt->d_name, RESET, st.st_size);
            free(newppath);
            continue;
        }

        if (S_ISFIFO(st.st_mode))
        {
            printf("%s%s%s", YELLOW, direc_prt->d_name, RESET);
            free(newppath);
            continue;
        }

        if (access(newppath, X_OK) == 0)
        {
            printf("%s%s%s", GREEN, direc_prt->d_name, RESET);
            free(newppath);
            continue;
        }

        if (access(newppath, R_OK) == -1)
        {
            printf("%s%s%s", RED, direc_prt->d_name, RESET);
            free(newppath);
            continue;
        }

        if (S_ISREG(st.st_mode))
        {
            printf("%s%s%s", DEFAULT, direc_prt->d_name, RESET);
            free(newppath);
            continue;
        }

        // default;
        free(newppath);
    }
    closedir(dp);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        exit(1);
    }

    struct stat st;
    int r = stat(argv[1], &st);
    if (r == -1)
    {
        perror("stat");
        exit(1);
    }

    if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "%s is not a directory\n", argv[1]);
        exit(1);
    }

    char *path = realpath(argv[1], NULL);
    printf("%s%s%s\n", CYAN, path, RESET);

    tree(path, 1, 1, 0);

    free(path);
    return 0;
}
