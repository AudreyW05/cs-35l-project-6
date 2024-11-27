#include "options.h"

long long handle_nbytes(int argc, char **argv) {
    bool valid = false;
    long long nbytes = 0;

    while (optind < argc) {
        char *arg = argv[optind];
        char *endptr;
        
        if (arg[0] == '-' && arg[1] != '\0') {
            optind++;
            continue;
        }

        errno = 0;
        nbytes = strtoll(arg, &endptr, 10);

        if (errno != 0) {
            perror(arg);
            valid = false;
        } else if (*endptr != '\0') {
            fprintf(stderr, "Invalid numeric argument: %s\n", arg);
            valid = false;
        } else {
            valid = true;
            break;
        }

        optind++;
    }

    if (!valid) {
        fprintf(stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
        return 1.01;
    }

    return nbytes;
}
