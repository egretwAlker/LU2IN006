# run with sh -x
rm -r *
rm -r .mygit
../../bin/myGit.exe init
echo a > a
echo b > b
echo m > m
echo c > c
mkdir d
echo d > d/d
chmod 777 a
ls -l
../../bin/myGit.exe add a b m c
../../bin/myGit.exe commit master -m "a b m c"
../../bin/myGit.exe branch nim
../../bin/myGit.exe add a b m d
echo a2 > a
echo b2 > b
../../bin/myGit.exe commit master -m "a b m d"
rm -r *
echo '3 a' | ../../bin/myGit.exe merge nim "merge"
COMMENT='Should show files a b m c d and only refs HEAD master'
ls -l
../../bin/myGit.exe list-refs
../../bin/myGit.exe log
COMMENT='Create a reference on the second predecessor'
../../bin/myGit.exe create-ref port abf1297c0c27577b81d20bf2dae111385bcb6688fd41cc4fd5752754a65fe4fe
rm -r *
../../bin/myGit.exe checkout-branch port
../../bin/myGit.exe get-current-branch
COMMENT='We deleted only a from a b m c, so it should show files b m c'
ls -l
../../bin/myGit.exe branch-print port
../../bin/myGit.exe delete-ref port
../../bin/myGit.exe list-refs
../../bin/myGit.exe checkout-commit eb921830919d839 
../../bin/myGit.exe add a b c d
../../bin/myGit.exe list-add
../../bin/myGit.exe clear-add
../../bin/myGit.exe list-add