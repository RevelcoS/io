#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

typedef double data_t;
const char *FMT = "%lf";
const char *SEP = ", ";

enum
{
    PERMISSIONS = 0777,
    READ_MODE,
    WRITE_MODE,
};

void io_read(char *fname);
void io_write(char *fname);


void io_read(char *fname) {
    struct stat attrs;
    if (stat(fname, &attrs)) {
        printf("[Error] File <%s> doesn't exist\n", fname);
        exit(EXIT_FAILURE);
    }

    int file = open(fname, O_RDONLY, PERMISSIONS);
    size_t len = attrs.st_size / sizeof(data_t);
    data_t data;
    const char *sep = "";
    for (size_t i = 0; i < len; i++) {
        read(file, &data, sizeof(data));
        printf("%s", sep);
        printf(FMT, data);
        sep = SEP;
    }
    printf("\n");

    if (close(file) == -1) {
        exit(EXIT_FAILURE);
    }
}

void io_write(char *fname) {
    int file = open(fname, O_WRONLY | O_TRUNC | O_CREAT, PERMISSIONS);
    data_t data;
    while (scanf(FMT, &data) == 1) {
        write(file, &data, sizeof(data));
    }

    if (close(file) == -1) {
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char **argv) {
    const struct option options[] = {
        {"read", no_argument, NULL, 'r'},
        {"write", no_argument, NULL, 'w'},
        {"type", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    const char optstring[] = "rwt:";

    int mode = 0;
    int index = 0;
    char option = 0;
    bool success = true;
    bool hasmode = false;
    while ((option = getopt_long(argc, argv, optstring, options, &index)) != -1) {
        switch (option) {
        case 'r':
            if (hasmode) {
                printf("[Error] Specify read/write option just once\n");
                return EXIT_FAILURE;
            }
            mode = READ_MODE;
            hasmode = true;
            break;
        case 'w':
            if (hasmode) {
                printf("[Error] Specify read/write option just once\n");
                return EXIT_FAILURE;
            }
            mode = WRITE_MODE;
            hasmode = true;
            break;
        case '?':
        default:
            success = false;
            break;
        }

        if (!success) {
            printf("[Error] Invalid option\n");
            return EXIT_FAILURE;
        }
    }

    if (!hasmode) {
        printf("[Error] Specify the mode (-r or -w)\n");
        return EXIT_FAILURE;
    } else if (optind < argc - 1) {
        printf("[Error] Too many arguments, require just file name\n");
        return EXIT_FAILURE;
    } else if (optind == argc) {
        printf("[Error] Specify the file name\n");
        return EXIT_FAILURE;
    }
    char *fname = argv[optind];
 
    switch (mode) {
    case READ_MODE:
        io_read(fname);
        break;
    case WRITE_MODE:
        io_write(fname);
        break;
    default:
        break;
    }

    return EXIT_SUCCESS;
}
