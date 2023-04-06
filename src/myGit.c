#include "cellList.h"
#include "fsop.h"
#include "misc.h"
#include "cellList.h"
#include "hashFunc.h"
#include "workTree.h"
#include "gestionCommits.h"

void list_refs() {
    if(!file_exists(REF)) err("References n'existent pas\n");
    else {
        printf("REFS : \n");
        List* l = listdir(REF);
        for(Cell* ptr = *l; ptr != NULL; ptr = ptr->next) {
            char* content = getRef(ptr->data);
            printf("- %s \t %s \n", ptr->data, content);
            free(content);
        }
        freeList(l);
    }
}

void list_add() {
    if(!file_exists(ADD)) err("Zone de preparation est vide\n");
    else {
        printf("Zone de preparation : \n");
        WorkTree* wt = ftwt(ADD);
        char* s = wtts(wt);
        printf("%s\n", s);
        freeWt(wt);
        free(s);
    }
}

void help() {
    printf("myGit init\n");
    printf("myGit list-refs\n");
    printf("myGit create-ref name hash\n");
    printf("myGit delete-ref name\n");
    printf("myGit add <elem> [<elem2> <elem3> ...]\n");
    printf("myGit list-add\n");
    printf("myGit clear-add\n");
    printf("myGit commit <branch-name> [-m <message>]\n");
    printf("myGit get-current-branch\n");
    printf("myGit branch <branch-name> : créer une branche\n");
    printf("myGit branch-print <branch-name>\n");
    printf("myGit checkout-branch <branch-name>\n");
    printf("myGit checkout-commit <pattern>\n");
}

int main(int argc, char* argv[]){
    if(argc == 1) {
        help();
        return 0;
    }
    if(strcmp(argv[1], "init")==0) {
        initRefs();
        initBranch();
    }
    if(!file_exists(REF)) {
        err("Il faut initialiser d'abord\n");
        return 0;
    }
    if(strcmp(argv[1], "list-refs")==0) list_refs();
    if(strcmp(argv[1], "create-ref")==0) createUpdateRef(argv[2], argv[3]);
    if(strcmp(argv[1], "delete-ref")==0) deleteRef(argv[2]);
    if(strcmp(argv[1], "add")==0) {
        for(int i = 2; i < argc; i++){
            myGitAdd(argv[i]);
        }
    }
    if(strcmp(argv[1], "clear-add")==0) {
        remove(ADD);
        assert(file_exists(ADD) == 0);
    }
    if(strcmp(argv[1], "list-add") == 0) list_add();
    if(strcmp(argv[1], "commit")==0)
        myGitCommit(argv[2], argc == 5 && strcmp(argv[3], "-m")==0 ? argv[4] : NULL);
    if(strcmp(argv[1], "get-current-branch")==0) {
        assert(argc == 2);
        char* s = getCurrentBranch();
        printf("%s", s);
        free(s);
    }
    if(strcmp(argv[1], "branch")==0) {
        assert(argc == 3);
        if(branchExists(argv[2])) {
            err("La branche existe déjà.\n");
        } else createBranch(argv[2]);
    }
    if(strcmp(argv[1], "branch-print")==0) {
        assert(argc == 3);
        if(!branchExists(argv[2])) {
            err("La branche n'existe pas.\n");
        } else printBranch(argv[2]);
    }
    if(strcmp(argv[1], "checkout-branch")==0) {
        assert(argc == 3);
        if(!branchExists(argv[2])) {
            err("La branche n'existe pas.\n");
        } else myGitCheckoutBranch(argv[2]);
    }
    if(strcmp(argv[1], "checkout-commit")==0) {
        assert(argc == 3);
        myGitCheckoutCommit(argv[2]);
    }
    return 0;
}