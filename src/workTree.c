#include "misc.h"
#include "fsop.h"
#include "hashFunc.h"
#include "workTree.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


WorkFile* createWorkFile(const char* name) {
    WorkFile* new = (WorkFile*)malloc(sizeof(WorkFile));
    new->name = strdup(name);
    new->hash = NULL;
    new->mode = 0;
    return new;
}

WorkFile* createWorkFile2(const char* name, const char* hash, int mode) {
    WorkFile* new = (WorkFile*)malloc(sizeof(WorkFile));
    new->name = strdup(name);
    new->hash = hash == NULL ? NULL : strdup(hash);
    new->mode = mode;
    return new;
}

/**
 * @brief hash should not be NULL
 * 
 * @param wf 
 * @return char* 
 */
char* wfts(WorkFile* wf) {
    assert(wf->name != NULL);
    char buf[MAXL];
    sprintf(buf,"%s\t%s\t%d", wf->name, wf->hash?wf->hash:"NULL", wf->mode);
    return strdup(buf);
}

WorkFile* stwf(const char* ch) {
    char buf1[MAXL], buf2[MAXL]; int buf3;
    sscanf(ch, "%s\t%s\t%d", buf1, buf2, &buf3);
    return createWorkFile2(buf1, buf2, buf3);
}

WorkTree* htwt(const char* hash) {
    assert(hashValid(hash));
    char* s = hashToPathExt(hash, ".t");
    WorkTree* res = stwt(s);
    free(s);
    return res;
}

void freeWf(WorkFile* wf) {
    free(wf->name); // free(NULL), nothing happens
    free(wf->hash);
    free(wf);
}

WorkTree* initWorkTree() {
    WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
    wt->size = WTS;
    wt->tab = (WorkFile*)malloc(sizeof(WorkFile)*(szt)(wt->size));
    wt->n = 0;
    return wt;
}

/**
 * @brief Return the position of file of folder named name in wt, -1 if not found
 */
int inWorkTree(WorkTree* wt, const char* name) {
    for(int i=0; i<wt->n; ++i) {
        if(strcmp(wt->tab[i].name, name) == 0)
            return i;
    }
    return -1;
}

int appendWorkTree(WorkTree* wt, const char* name, const char* hash, int mode) {
    if(wt->size == wt->n) {
        err("WorkTree full error\n");
        return -1;
    }
    if(inWorkTree(wt, name) != -1) return 0;
    // protestation of not having double pointers
    WorkFile *w = createWorkFile2(name, hash, mode);
    wt->tab[(wt->n)++] = *w; free(w);
    return 0;
}

char* wtts(WorkTree* wt) {
    char buf[MAXL]; buf[0] = 0;
    char* p = buf;
    for(int i=0; i<wt->n; ++i) {
        char *s = wfts(&(wt->tab[i]));
        sprintf(p, "%s\n", s);
        p += strlen(p);
        free(s);
    }
    return strdup(buf);
}

WorkTree* stwt(const char* s) {
    WorkTree* wt = initWorkTree();
    char buf[MAXL];
    while(*s) {
        sscanf(s, "%[^\n]", buf);
        WorkFile *wf = stwf(buf);
        assert(appendWorkTree(wt, wf->name, wf->hash, wf->mode) == 0);
        freeWf(wf);
        s += strlen(buf)+1;
    }
    return wt;
}

int wttf(WorkTree* wt, const char* file) {
    char* s = wtts(wt);
    stf(s, file);
    free(s);
    return 0;
}

WorkTree* ftwt(const char* file) {
    char* s = fts(file);
    WorkTree* wt = stwt(s);
    free(s);
    return wt;
}

void freeWt(WorkTree* wt) {
    for(int i=0;i<wt->n;++i) {
        free(wt->tab[i].name);
        free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);
}

/**
 * @brief Take a snapshot of WorkTree wt with extension .t
 * @return char* the hash of the string representation of wt
 */
char* blobWorkTree(WorkTree* wt) {
    char* s = wtts(wt);
    char* hash = blobStringExt(s, ".t");
    free(s);
    return hash;
}

WorkTree* getWtFromPath(const char* path) {
    WorkTree* newWt = initWorkTree();
    List* l = listdir(path);
    for(Cell* c = *l; c; c = c->next) {
        // err("f: %s\n", c->data);
        appendWorkTree(newWt, c->data, NULL, 0);
    }
    freeList(l);
    return newWt;
}

char* concatPaths(const char* p1, const char* p2) {
    int n = (int)strlen(p1)+(int)strlen(p2)+2;
    char* s = malloc(sizeof(char)*(szt)n);
    strcpy(s, p1);
    append(s, "/");
    append(s, p2);
    return s;
}

/**
 * @brief This is an adhoc fonction to saveWorkTree, hash pointer points to the new hash, take the ownership of hash
 */
void updateWf(WorkFile* wf, char* hash, int mode) {
    free(wf->hash);
    wf->hash = hash;
    wf->mode = mode;
}

/**
 * @brief Create a snapshot of the files and folders of wt at path
 * and fill in the hash and mode entry
 */
char* saveWorkTree(WorkTree* wt, const char* path) {
    for(int i=0;i<wt->n;++i) {
        char* fp = concatPaths(path, wt->tab[i].name);
        if(isDir(fp)) {
            WorkTree* newWt = getWtFromPath(fp);
            updateWf(wt->tab + i, saveWorkTree(newWt, fp), getChmod(fp));
            freeWt(newWt);
        } else {
            blobFile(fp);
            updateWf(wt->tab+i, sha256file(fp), getChmod(fp));
        }
        free(fp);
    }
    return blobWorkTree(wt);
}

int isWorkTree(const char* hash) {
    char* s = hashToPathExt(hash, ".t");
    int res = file_exists(s);
    free(s);
    return res;
}

void restoreWorkTree(WorkTree* wt, const char* path) {
    char* cmd = newconcat("mkdir -p ", path);
    assert(system(cmd) == 0);
    free(cmd);
    for(int i=0; i<wt->n; ++i) {
        char* fp = concatPaths(path, wt->tab[i].name);
        char* hp;
        // err("name: %s, %s\n", wt->tab[i].name, wt->tab[i].hash);
        if(isWorkTree(wt->tab[i].hash)) {
            hp = hashToPathExt(wt->tab[i].hash, ".t");
            WorkTree* nwt = ftwt(hp);
            restoreWorkTree(nwt, fp);
            setMode(wt->tab[i].mode, fp);
            freeWt(nwt);
        } else {
            hp = hashToPath(wt->tab[i].hash);
            cp(fp, hp);
            setMode(wt->tab[i].mode, fp);
        }
        free(fp);
        free(hp);
    }
}