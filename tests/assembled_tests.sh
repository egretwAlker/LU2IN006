# run with sh +x
rm -r *
rm -r .mygit
../../bin/myGit.exe init
echo a > a
echo b > b
echo c > c
mkdir d
echo d > d/d
chmod 777 a
ls -l
../../bin/myGit.exe add a b c
../../bin/myGit.exe commit master -m "a b c"
../../bin/myGit.exe branch nim
../../bin/myGit.exe add a b d
../../bin/myGit.exe commit master -m "a b d"
rm -r *
echo '3 a' | ../../bin/myGit.exe merge nim "merge"
COMMENT='Should show files a b c d and only refs HEAD master'
ls -l
../../bin/myGit.exe list-refs
../../bin/myGit.exe log
COMMENT='Create a reference on the second predecessor'
../../bin/myGit.exe create-ref port a1d2160b26d1f8a12c7149e71e54ce1f66720e41a4e8137806955d2c9a8dbfea
../../bin/myGit.exe checkout-branch port
../../bin/myGit.exe get-current-branch
../../bin/myGit.exe branch-print port
../../bin/myGit.exe delete-ref port
../../bin/myGit.exe list-refs
../../bin/myGit.exe checkout-commit 7410c2b9a2f268745042
../../bin/myGit.exe add a b c d
../../bin/myGit.exe list-add
../../bin/myGit.exe clear-add
../../bin/myGit.exe list-add