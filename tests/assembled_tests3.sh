rm -r *
rm -r .mygit
valgrind ../../bin/myGit.exe init
echo a > a
echo b > b
echo c > c
echo d > d
../../bin/myGit.exe add a b c
../../bin/myGit.exe commit master -m "a b c"
../../bin/myGit.exe branch junior
../../bin/myGit.exe add a b d
../../bin/myGit.exe commit master -m "a b d"
echo "3 a" | valgrind --leak-check=full ../../bin/myGit.exe merge junior "merge with conflict"
../../bin/myGit.exe branch-print master
rm -r *
../../bin/myGit.exe checkout-commit 8a2ca72527
ls