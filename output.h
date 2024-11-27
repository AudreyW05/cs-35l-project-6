#ifndef OUTPUT
#define OUTPUT

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>


bool writebytes(unsigned long long x, int nbytes);
int handle_output(char* input, char* output, int nbytes);

#endif