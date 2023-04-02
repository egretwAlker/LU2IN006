GCC=gcc -g -ftrapv -Wconversion -Wall -Wextra -fno-common -fno-builtin 

bin/myGit.exe: bin/misc.o bin/workTree.o bin/cellList.o bin/fsop.o bin/hashFunc.o bin/gestionCommits.o bin/myGit.o
	$(GCC) $^ -o $@

bin/%.o: src/%.c
	$(GCC) -c $^ -o $@

bin/%.exe: src/%.c
	$(GCC) $^ -o $@