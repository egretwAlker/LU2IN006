rm -r *
rm -r .mygit
valgrind ../../bin/myGit.exe init
echo a > a
echo b > b
../../bin/myGit.exe add a
../../bin/myGit.exe commit master -m "a"
../../bin/myGit.exe branch junior
../../bin/myGit.exe branch poly
../../bin/myGit.exe add a b
../../bin/myGit.exe commit master -m "a b"
../../bin/myGit.exe add b
../../bin/myGit.exe commit master -m "b"
../../bin/myGit.exe merge junior "merge a b"
echo 2 | valgrind --leak-check=full ../../bin/myGit.exe merge poly "merge with conflict"
../../bin/myGit.exe branch-print master