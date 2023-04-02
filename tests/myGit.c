#include "cellList.h"
#include "fsop.h"
#include "misc.h"
#include "hashFunc.h"
#include "workTree.h"
#include "gestionCommits.h"

int main(int argc, char* argv[]){

    if(strcmp(argv[1], "init")==0){
        initRefs();
    }

    if(strcmp(argv[1], "list-refs")==0){
        char buff[MAXL];
        sprintf(buff, "%s/.refs", SPFLDR);
        if(!file_exists(buff)) err("References n'existent pas\n");
        else{
            printf("REFS : \n");
            List *l = listdir(buff);
            for(Cell*ptr = *l; ptr != NULL; ptr = ptr->next){
                char* content = getRef(ptr->data);
                printf("- %s \t %s \n",ptr->data, content);
                free(content);
            }
            freeList(l);
        }
    }
    
    if(strcmp(argv[1], "create-ref")==0){
        createUpdateRef(argv[2], argv[3]);
    }

    if (strcmp(argv[1], "delete-ref")==0){
        deleteRef(argv[2]);
    }

    if(strcmp(argv[1], "add")==0){
        for(int i = 2; i < argc; i++){
            myGitAdd(argv[i]);
        }
    }

    if(strcmp(argv[1], "clear-add")==0){
        char buff[MAXL];
        sprintf(buff, "%s/.add", SPFLDR);
        if(remove(buff)) err("Removing error\n");
    }
 
    if(strcmp(argv[1], "list-add")==0){
        char buff[MAXL];
        sprintf(buff, "%s/.add", SPFLDR);
        if(!file_exists(buff)) err("Zone de preparation est vide\n");
        else {
            printf("Zone de preparation : \n");
            WorkTree* wt = ftwt(buff);
            printf("%s\n", wtts(wt));
            freeWt(wt);
        }
    }

    if(strcmp(argv[1], "commit")==0){
        myGitCommit(argv[2], strcmp(argv[3], "-m")==0 ? argv[4] : NULL);
    }
    
    return 0;

}