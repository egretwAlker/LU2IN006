#include "cellList.h"
#include "fsop.h"
#include "misc.h"
#include "cellList.h"
#include "hashFunc.h"
#include "workTree.h"
#include "gestionCommits.h"

/**
 * @brief Allow to put other metadata (e.g. second_predecessor) in the commit generated
 */
void myGitCommit2(const char* branch, const char* message, const char* pred2) {
  if(!branchExists(branch)) {
    err("La branche n'existe pas\n");
    return;
  }
  if(!file_exists(ADD)) createFile(ADD);

  char* last_hash = getRef(branch);
  char* head_hash = getRef("HEAD");
  assert(last_hash);
  assert(head_hash);
  if(strcmp(last_hash, head_hash) == 0) {
    WorkTree* wt = ftwt(ADD);
    char* hashwt = saveWorkTree(wt, ".");
    Commit* c = createCommit(hashwt);
    if(strlen(last_hash) != 0)
      commitSet(c, "predecessor", last_hash);
    if(pred2 != NULL)
      commitSet(c, "second_predecessor", pred2);
    if(message != NULL)
      commitSet(c, "message", message);
    char* hashc = blobCommit(c);
    createUpdateRef(branch, hashc);
    createUpdateRef("HEAD", hashc);
    remove(ADD);
    freeWt(wt);
    free(hashwt);
    free(hashc);
    freeCommit(c);
  } else err("HEAD doit pointer sur le dernier commit de la branche");
  free(last_hash);
  free(head_hash);
}

/**
 * @brief If HEAD and branch point to the same thing, then move HEAD and branch towards together
 */
void myGitCommit(const char* branch, const char* message) {
  myGitCommit2(branch, message, NULL);
}

/**
 * @brief Move with HEAD to where the branch points
 */
void myGitCheckoutBranch(const char* branch) {
  assert(branchExists(branch));
  stf(branch, CURB);
  char* h = getRef(branch);
  createUpdateRef("HEAD", h);
  if(hashValid(h)) restoreCommit(h);
  free(h);
}

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
    char* s = wtts_upd(wt, 1);
    printf("%s\n", s);
    freeWt(wt);
    free(s);
  }
}

/**
 * @brief If no conflicts, merge remote_branch to current_branch, otherwise no operations;
 * this fonctions will discard all the changes after the last commit
 * @return NULL if no conflicts, a List of names of conflict files
 */
List* merge(const char* remote_branch, const char* message) {
  remove(ADD);
  assert(branchExists(remote_branch));
  char* current_branch = getCurrentBranch();
  WorkTree *wt1 = btwt(current_branch), *wt2 = btwt(remote_branch);
  List* l = NULL;
  WorkTree* wt = mergeWorkTrees(wt1, wt2, &l);
  if(listSize(l) == 0) {
    freeList(l); l = NULL;
    restoreWorkTree(wt, ".");
    wttf(wt, ADD);
    char* h = getRef(remote_branch);
    myGitCommit2(current_branch, message, h);
    free(h);
    deleteRef(remote_branch);
  }
  free(current_branch);
  freeWt(wt);
  freeWt(wt1);
  freeWt(wt2);
  return l;
}

/**
 * @brief Advance by a commit of deletion of files in conflit on branch with message
 */
void createDeletionCommit(const char* branch, List* conflicts, const char* message) {
  remove(ADD);
  assert(branchExists(branch));
  char* curb = getCurrentBranch();
  myGitCheckoutBranch(branch);
  WorkTree* wt = btwt(branch);
  for(int i=0; i<wt->n; ++i) {
    if(searchList(conflicts, wt->tab[i].name) == NULL) {
      myGitAdd(wt->tab[i].name);
    }
  }
  freeWt(wt);
  myGitCommit(branch, message);
  myGitCheckoutBranch(curb);
  free(curb);
}

/**
 * @brief Show information about commits whose hashes start by pattern
 */
void myGitCheckoutCommit(const char* pattern) {
  List* l = getAllCommits();
  List* filtered = filterList(l, pattern);
  int sz = listSize(filtered);
  if(sz == 0) {
    err("Pattern not matched\n");
  } else if(sz > 1) {
    err("Multiple matching found:\n");
    for(Cell* c = *filtered; c; c=c->next) {
      printf("-> %s\n", c->data);
    }
  } else if(sz == 1) {
    char* h = (*filtered)->data;
    createUpdateRef("HEAD", h);
    restoreCommit(h);
  }
  freeList(l);
  freeList(filtered);
}

void commandMerge(const char* remb, const char* meg) {
  char* curb = getCurrentBranch();
  if(branchExists(remb) == 0)
    err("La branche %s n'existe pas\n", remb);
  else if(strcmp(curb, remb) == 0)
    err("On ne peut pas faire merge sur les même branches\n");
  else {
    List* conflicts = merge(remb, meg);
    if(conflicts == NULL) {
      err("Merge bien réussi\n");
    } else {
      char* s = ltos(conflicts);
      err("Il existe des conflits des fichiers (%s), veuillez opérer l'un d'eux avant de faire merge:\n", s);
      free(s);
      err("1. Créer un commit de suppression pour la branch %s\n", remb);
      err("2. Créer un commit de suppression pour la branch courante %s\n", curb);
      err("3. Choisir des fichiers que l'on garde dans la branch courante\n");
      int d = 0;
      scanf("%d", &d);
      assert(1 <= d && d <= 3);
      if(d == 1) {
        createDeletionCommit(remb, conflicts, "Deletion of conflicts");
      } else if(d == 2) {
        createDeletionCommit(curb, conflicts, "Deletion of conflicts");
      } else if(d == 3) {
        err("Veuillez taper les noms des fichiers que vous souhaiter garder dans la branch courante:\n");
        err("Sous forme de string1|string2|...|stringn\n");
        char buf[MAXL];
        scanf("%s", buf);
        List* remains = stol(buf);
        List* remainsb = initList();
        for(Cell* c=*conflicts; c; c=c->next) {
          if(searchList(remains, c->data) == NULL)
            insertFirstString(remainsb, c->data);
        }
        createDeletionCommit(remb, remains, "Deletion of conflicts");
        createDeletionCommit(curb, remainsb, "Deletion of conflicts");
        freeList(remains);
        freeList(remainsb);
      }
      merge(remb, meg);
    }
    freeList(conflicts);
  }
  free(curb);
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
    printf("myGit merge <branch> <message>\n");
}

int main(int argc, char* argv[]){
  if(argc == 1) {
    help();
    return 0;
  }
  if(strcmp(argv[1], "init")==0) {
    initRefs();
    initBranch();
    return 0;
  }
  if(!file_exists(REF)) {
    err("Il faut initialiser d'abord\n");
    return 0;
  }
  if(strcmp(argv[1], "list-refs")==0) list_refs();
  else if(strcmp(argv[1], "create-ref")==0) createUpdateRef(argv[2], argv[3]);
  else if(strcmp(argv[1], "delete-ref")==0) deleteRef(argv[2]);
  else if(strcmp(argv[1], "add")==0) {
    for(int i = 2; i < argc; i++){
      myGitAdd(argv[i]);
    }
  }
  else if(strcmp(argv[1], "clear-add")==0) {
    remove(ADD);
    assert(file_exists(ADD) == 0);
  }
  else if(strcmp(argv[1], "list-add") == 0) list_add();
  else if(strcmp(argv[1], "commit")==0) {
    assert(argc == 3 || (argc == 5 && strcmp(argv[3], "-m") == 0));
    myGitCommit(argv[2], argc == 5 ? argv[4] : NULL);
  } else if(strcmp(argv[1], "get-current-branch")==0) {
    assert(argc == 2);
    char* s = getCurrentBranch();
    printf("%s", s);
    free(s);
  } else if(strcmp(argv[1], "branch")==0) {
    assert(argc == 3);
    if(branchExists(argv[2])) {
      err("La branche existe déjà.\n");
    } else createBranch(argv[2]);
  } else if(strcmp(argv[1], "branch-print")==0) {
    assert(argc == 3);
    if(!branchExists(argv[2])) {
      err("La branche n'existe pas.\n");
    } else printBranch(argv[2]);
  } else if(strcmp(argv[1], "checkout-branch")==0) {
    assert(argc == 3);
    if(!branchExists(argv[2])) {
      err("La branche n'existe pas.\n");
    } else myGitCheckoutBranch(argv[2]);
  } else if(strcmp(argv[1], "checkout-commit")==0) {
    assert(argc == 3);
    myGitCheckoutCommit(argv[2]);
  } else if(strcmp(argv[1], "merge") == 0) {
    assert(argc == 4);
    commandMerge(argv[2], argv[3]);
  } else err("On ne comprend pas\n");
  return 0;
}