/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.

   Copyright 2015, 2017, 2020 Paul Eggert

   This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <cpuid.h>
#include <errno.h>
#include <immintrin.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/getopt_core.h>

#include "rand64-hw.h"
#include "rand64-sw.h"
#include "output.h"
#include "options.h"

/* Main program, which outputs N bytes of random data.  */
int main(int argc, char **argv) {

  char* input = "rdrand";
  char* output = "stdio";

  int c;

  while ((c = getopt(argc, argv, ":i:o:")) != -1)
    switch (c)
      {
        case 'i':
          input = optarg;
          break;
        case 'o':
          output = optarg;
          break;
        case ':':
          fprintf(stderr, "Option -%c requires an operand\n", optopt);
          return 1;
        case '?':
          fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
          return 1;
        default:
          abort();
      }
    
  //printf("Input: %s\nOutput: %s\n", input, output);

  if (!rdrand_supported)
  {
    fprintf(stderr, "RDRAND not supported on current machine\n");
    return 1;
  }
  
  /* Check arguments.  */
  long long nbytes = handle_nbytes(argc, argv);
  
  if (nbytes == 1.01)
    return 1;

  int errno_out = handle_output(input, output, nbytes);
  return errno_out;
}
