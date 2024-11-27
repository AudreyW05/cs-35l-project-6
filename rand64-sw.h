#ifndef RAND64SW
#define RAND64SW

#include <immintrin.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>


void software_rand64_init(void);
unsigned long long software_rand64(void);
void software_rand64_fini(void);
void init_with_file(char* filepath);
void lrand_init(void);
unsigned long long lrand(void);
void lrand_fini(void);


#endif