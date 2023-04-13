GCC=gcc -g -ftrapv -Wconversion -Wall -Wextra -fno-common -fno-builtin 
GCCR=gcc

bin/myGit.exe: bin/misc.o bin/workTree.o bin/cellList.o bin/fsop.o bin/hashFunc.o bin/gestionCommits.o bin/myGit.o
	$(GCC) $^ -o $@

bin/%.o: src/%.c
	$(GCC) -c $^ -o $@

release: bin/myGit

bin/myGit: src/misc.c src/workTree.c src/cellList.c src/fsop.c src/hashFunc.c src/gestionCommits.c src/myGit.c
	$(GCCR) $^ -o $@

clean:
	rm bin/*