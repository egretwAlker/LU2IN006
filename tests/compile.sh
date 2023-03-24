gcc -g -Wall -ftrapv -Wconversion -Wall -Wextra -fno-common -fno-builtin tests.c \
../src/workTree.c ../src/cellList.c ../src/fsop.c ../src/hashFunc.c ../src/gestionCommits.c -o tests.exe