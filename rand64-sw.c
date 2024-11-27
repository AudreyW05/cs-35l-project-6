#include "rand64-sw.h"


/* Software implementation.  */

/* Input stream containing random bytes.  */
FILE *urandstream;

/* Initialize the software rand64 implementation.  */
void software_rand64_init(void) {
  urandstream = fopen("/dev/random", "r");
  if (!urandstream)
    abort();
}

void init_with_file(char* filepath) {
  urandstream = fopen(filepath, "r");
  if (!urandstream)
    abort();
}

/* Return a random value, using software operations.  */
unsigned long long software_rand64(void) {
  unsigned long long int x;
  if (fread(&x, sizeof x, 1, urandstream) != 1)
    abort();
  return x;
}

/* Finalize the software rand64 implementation.  */
void software_rand64_fini(void) { fclose(urandstream); }


// lrand48_r methods
struct drand48_data state;

void lrand_init(void) {
  srand48_r(time(NULL), &state);
}

unsigned long long lrand(void) {
  
  long int high, low;

  mrand48_r(&state, &high);
  mrand48_r(&state, &low);

  unsigned long long result = ((unsigned long long)(uint32_t)high << 32) | (uint32_t)low;

  return result;
}

void lrand_fini(void) {}