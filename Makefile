# Make x86-64 random byte generators.

# Copyright 2015, 2020, 2021 Paul Eggert

# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

# Optimization level.  Change this -O2 to -Og or -O0 or whatever.
OPTIMIZE =

# The C compiler and its options.
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -fanalyzer \
  -march=native -mtune=native -mrdrnd

# The archiver command, its options and filename extension.
TAR = tar
TARFLAGS = --gzip --transform 's,^,randall/,'
TAREXT = tgz

# Relevant files.
FILES = $(wildcard *.c) $(wildcard *.h)

default: randall

randall: randall.c $(FILES)
	$(CC) $(CFLAGS) *.c -o $@

assignment: randall-assignment.$(TAREXT)
assignment-files = COPYING Makefile randall.c
randall-assignment.$(TAREXT): $(assignment-files)
	$(TAR) $(TARFLAGS) -cf $@ $(assignment-files)

# This generates a submission tarball of your randall source code.
submission-tarball: randall-submission.$(TAREXT)
submission-files = Makefile notes.txt options.c options.h output.c output.h rand64-hw.c rand64-hw.h rand64-sw.c rand64-sw.h randall.c
randall-submission.$(TAREXT): $(submission-files)
	$(TAR) $(TARFLAGS) -cf $@ $(submission-files)

# This generates a submission tarball of your Git repository that you will submit.
repository-tarball:
	$(TAR) -czf randall-git.tgz .git

# Make sure to add these rules to .PHONY. These tells make that it should not look for files called each of these recipe targets.
.PHONY: default clean assignment submission-tarball repository-tarball check format valgrind sanitizers

# Deletes existing build files. Helpful to run before building to start from scratch.
clean:
	rm -f *.o *.$(TAREXT) randall

# Unit test that checks NBYTES.
check: randall
	@echo "Running test cases..."
	@echo "=========== TEST 1 ==========="
	@./randall 20 | wc -c | (grep -q "^20$$" && echo "PASSED: Check length") || (echo "FAILED: Check length" && false)
	@echo "=========== TEST 2 ==========="
	@./randall 1 | wc -c | (grep -q "^1$$" && echo "PASSED: Check length of 1") || (echo "FAILED: Check length of 1" && false)
	@echo "=========== TEST 3 ==========="
	@./randall 0 | wc -c | (grep -q "^0$$" && echo "PASSED: Check zero length") || (echo "FAILED: Check zero length" && false)
	@echo "=========== TEST 4 ==========="
	@./randall 20 -i rdrand | wc -c | (grep -q "^20$$" && echo "PASSED: Check rdrand input option") || (echo "FAILED: Check rdrand input option" && false)
	@echo "=========== TEST 5 ==========="
	@./randall -i rdrand 20 | wc -c | (grep -q "^20$$" && echo "PASSED: Check NBYTES after single input option") || (echo "FAILED: Check NBYTES after single input option" && false)
	@echo "=========== TEST 6 ==========="
	@./randall 20 -o stdio | wc -c | (grep -q "^20$$" && echo "PASSED: Check stdio output option") || (echo "FAILED: Check stdio output option" && false)
	@echo "=========== TEST 7 ==========="
	@./randall -o stdio 20 | wc -c | (grep -q "^20$$" && echo "PASSED: Check NBYTES after single output option") || (echo "FAILED: Check NBYTES after single output option" && false)
	@echo "=========== TEST 8 ==========="
	@./randall 20 -i rdrand -o stdio | wc -c | (grep -q "^20$$" && echo "PASSED: Check default IO options") || (echo "FAILED: Check default IO options" && false)
	@echo "=========== TEST 9 ==========="
	@./randall -i rdrand -o stdio 20 | wc -c | (grep -q "^20$$" && echo "PASSED: Check NBYTES after default IO options") || (echo "FAILED: Check NBYTES after default IO options" && false)
	@echo "=========== TEST 10 ==========="
	@./randall 20 -i /usr/bin/cat | wc -c | (grep -q "^20$$" && echo "PASSED: Check /F option") || (echo "FAILED: Check /F option" && false)
	@echo "=========== TEST 11 ==========="
	@./randall 20 -i lrand48_r | wc -c | (grep -q "^20$$" && echo "PASSED: Check lrand48_r option") || (echo "FAILED: Check lrand48_r option" && false)
	@echo "=========== TEST 12 ==========="
	@strace ./randall -o 100 1000 2>&1 | grep --text write | wc -l | (grep -q "^10$$" && echo "PASSED: Check N option") || (echo "FAILED: Check N option" && false)
	@echo "=========== TEST 13 ==========="
	@strace ./randall -o 10 20  2>&1 | grep --text write | wc -l | (grep -q "^2$$" && echo "PASSED: Check N option with less bytes") || (echo "FAILED: Check N option with less bytes" && false)
	@echo "=========== TEST 14 ==========="
	@strace ./randall -o 5 15 2>&1 | grep --text write | wc -l | (grep -q "^3$$" && echo "PASSED: Check N option again") || (echo "FAILED: Check N option again" && false)
	@echo "=========== TEST 15 ==========="
	@strace ./randall -o 5 23 2>&1 | grep --text write | wc -l | (grep -q "^5$$" && echo "PASSED: Check N option with non-multiple NBYTES") || (echo "FAILED: Check N option with non-multiple NBYTES" && false)
	@echo "=========== TEST 16 ==========="
	@strace ./randall -o 10 5 2>&1 | grep --text write | wc -l | (grep -q "^1$$" && echo "PASSED: Check N option s.t. NBYTES < N") || (echo "FAILED: Check N option s.t. NBYTES < N" && false)
	@echo "=========== TEST 17 ==========="
	@strace ./randall -i lrand48_r -o 100 1000 2>&1 | grep --text write | wc -l | (grep -q "^10$$" && echo "PASSED: Check N option with lrand48_r option") || (echo "FAILED: Check N option with lrand48_r option" && false)
	@echo "=========== TEST 18 ==========="
	@strace ./randall -i /usr/bin/cat -o 100 1000 2>&1 | grep --text write | wc -l | (grep -q "^10$$" && echo "PASSED: Check N option with /F option") || (echo "FAILED: Check N option with /F option" && false)
	@echo "=========== TEST 19 ==========="
	@./randall 20 -o 10 | wc -c | (grep -q "^20$$" && echo "PASSED: NBYTES unchanged by N option") || (echo "FAILED: NBYTES unchanged by N option" && false)


# Automatically formats all of your C code.
format:
	clang-format -i *.c *.h

# Helps find memory leaks. For an explanation on each flag, see this link.
valgrind: randall
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./randall 100

# Runs the Undefined Behavior (UBSAN) and Address (ASAN) sanitizers. GCC doesn’t support these on the SEASNet servers so we recommend using clang instead (a different C compiler).
sanitizers:
	clang -g3 -Wall -Wextra -mrdrnd -fsanitize=address -fsanitize=undefined *.c -o randall
