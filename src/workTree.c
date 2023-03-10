#include "misc.h"
#include "workTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int getChmod ( const char * path ) {
    struct stat ret ;

    if (stat(path, &ret) == -1) {
        return -1;
    }

    return ( ret.st_mode & S_IRUSR ) |( ret.st_mode & S_IWUSR ) | ( ret.st_mode & S_IXUSR ) |
    /*owner*/
    ( ret.st_mode & S_IRGRP ) | ( ret.st_mode & S_IWGRP ) | ( ret.st_mode & S_IXGRP ) |
    /*group*/
    ( ret.st_mode & S_IROTH ) | ( ret.st_mode & S_IWOTH ) | ( ret.st_mode & S_IXOTH );
    /*other*/
}

void setMode ( int mode , char * path ) {
    char buff[100];
    sprintf(buff, "chmod %d %s", mode, path);
    system(buff);
}

WorkFile* createWorkFile(char* name) {
    WorkFile* new = (WorkFile*)malloc(sizeof(WorkFile));
    new->name = strdup(name);
    new->hash = NULL;
    new->mode = 0;
    return new;
}

WorkFile* createWorkFile2(char* name, char* hash, int mode) {
    WorkFile* new = (WorkFile*)malloc(sizeof(WorkFile));
    new->name = strdup(name);
    new->hash = strdup(hash);
    new->mode = mode;
    return new;
}

char* wfts(WorkFile* wf) {
    /*
        hash should not be NULL
    */
    if(wf->hash == NULL) {
        err("hash NULL error\n");
    }
    char buf[MAXL];
    sprintf(buf,"%s\t%s\t%d", wf->name, wf->hash, wf->mode);
    return strdup(buf);
}

WorkFile* stwf(char* ch) {
    char buf1[MAXL], buf2[MAXL]; int buf3;
    sscanf(ch, "%s\t%s\t%d", buf1, buf2, &buf3);
    return createWorkFile2(buf1, buf2, buf3);
    return createWorkFile(buf1);
}

void clearWf(WorkFile* wf) {
    free(wf->name);
    free(wf->hash);
    free(wf);
}

WorkTree* initWorkTree() {
    WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
    wt->size = WTS;
    wt->tab = (WorkFile*)malloc(sizeof(WorkFile)*(wt->size));
    wt->n = 0;
    return wt;
}

int inWorkTree(WorkTree* wt, char* name) {
    /*
        Return the position of file of folder named name in wt, -1 if not found
    */
    for(int i=0; i<wt->n; ++i) {
        if(strcmp(wt->tab[i].name, name) == 0)
            return i;
    }
    return -1;
}

int appendWorkTree(WorkTree* wt, char* name, char* hash, int
mode) {
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
    char buf[MAXL];
    int l = 0;
    for(int i=0; i<wt->n; ++i) {
        char *s = wfts(&(wt->tab[i]));
        sprintf(buf+l, "%s\n", s);
        l += strlen(s)+1;
        free(s);
    }
    return strdup(buf);
}

WorkTree* stwt(char* s) {
    WorkTree* wt = initWorkTree();
    char buf[MAXL];
    int p = 0, n = strlen(s);
    while(p < n) {
        int t = 0;
        while(p<n && s[p] != '\n') buf[t++] = s[p++];
        ++p; buf[t] = 0;
        WorkFile *wf = stwf(buf);
        if(appendWorkTree(wt, wf->name, wf->hash, wf->mode) == -1)
            err("Appending error\n");
        clearWf(wf);
    }
    return wt;
}

int wttf(WorkTree* wt, char* file) {
    FILE* f = fopen(file, "w");
    if(!f) {
        err("Error while opening the file\n");
        return -1;
    }
    fprintf(f, "%s", wtts(wt));
    fclose(f);
    return 0;
}

WorkTree* ftwt(char* file) {
    FILE* f = fopen(file, "r");
    if(!f) {
        err("Error while opening the file\n");
        return NULL;
    }
    char buf[MAXL];
    int m = fread(buf, MAXL-1, sizeof(char), f);
    buf[m] = 0;
    fclose(f);
    return stwt(buf);
}

void clearWt(WorkTree* wt) {
    for(int i=0;i<wt->n;++i) {
        free(wt->tab[i].name);
        free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);
}