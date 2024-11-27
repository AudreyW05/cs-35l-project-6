#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"


bool writebytes(unsigned long long x, int nbytes) {
    do {
        if (putchar(x) < 0)
            return false;
        x >>= CHAR_BIT;
        nbytes--;
    } while (0 < nbytes);

    return true;
}

int handle_output(char* input, char* output, int nbytes) {
    if (nbytes == 0)
        return 0;

    void (*initialize)(void);
    unsigned long long (*rand64)(void);
    void (*finalize)(void);

    if (strcmp(input, "(null)") == 0) {
        fprintf(stderr, "Missing input\n");
        return 1;
    }

    if (strcmp(output, "(null)") == 0) {
        fprintf(stderr, "Missing output\n");
        return 1;
    }

    char* filename = "/dev/random";
    if (strcmp(input, "rdrand") == 0) {
        initialize = hardware_rand64_init;
        rand64 = hardware_rand64;
        finalize = hardware_rand64_fini;
        initialize();
    } else if (strcmp(input, "lrand48_r") == 0) {
        initialize = lrand_init;
        rand64 = lrand;
        finalize = lrand_fini;
        initialize();
    } else if (strncmp(input, "/", 1) == 0) {
        filename = input;
        initialize = software_rand64_init;
        init_with_file(filename);
        rand64 = software_rand64;
        finalize = software_rand64_fini;
    } else {
        fprintf(stderr, "Invalid input option\n");
        return 1;
    }


    int wordsize = sizeof rand64();
    int output_errno = 0;

    if (strcmp(output, "stdio") == 0) {
        do {
            unsigned long long x = rand64();
            int outbytes = nbytes < wordsize ? nbytes : wordsize;
            if (!writebytes(x, outbytes)) {
                output_errno = errno;
                break;
            }
            nbytes -= outbytes;
        } while (0 < nbytes);
    } else {
        char* endptr;
        long int maxbytes;

        maxbytes = strtol(output, &endptr, 10);

        if (*endptr != '\0') {
            fprintf(stderr, "Invalid output option\n");
            return 1;
        }

        char *buffer = malloc(maxbytes);

        do {
            unsigned long long x = rand64();
            int outbytes = nbytes < maxbytes ? nbytes : maxbytes;

            for (int i = 0; i < outbytes; i += sizeof(x)) {
                for (size_t j = 0; j < sizeof(x); j++) {
                    unsigned char curbyte = *((unsigned char *)&x + j);
                    buffer[i + j] = curbyte;
                }
            }

            write(1, buffer, outbytes);
            nbytes -= outbytes;

        } while (0 < nbytes);

        free(buffer);
    }

    if (fclose(stdout) != 0)
        output_errno = errno;

    if (output_errno) {
        errno = output_errno;
        perror("output");
    }

    finalize();
    return !!output_errno;
}
