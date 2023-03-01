GCC=gcc -g -ftrapv -Wconversion -Wall -Wextra -pedantic -fno-common -fno-builtin 
bin/%.o: src/%.c
	$(GCC) -c $^ -o $@

bin/%.exe: src/%.c
	$(GCC) $^ -o $@

test: bin/cellList.o
	$(GCC) tests/tests.c bin/cellList.o -o bin/tests.exe
	./bin/tests.exe