#ifndef WORKTREE
#define WORKTREE

/**
 * @brief Represent a file or a folder/directory.
 * 
 */
typedef struct {
    char* name;
    char* hash;
    int mode;
} WorkFile;

typedef struct {
    WorkFile* tab;
    int size;
    int n;
} WorkTree;

WorkFile* createWorkFile(const char* name);
char* wfts(WorkFile* wf);
WorkFile* stwf(const char* ch);
void freeWf(WorkFile* wf);
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, const char* name);
int appendWorkTree(WorkTree* wt, const char* name, const char* hash, int mode);
char* wtts(WorkTree* wt);
int wttf(WorkTree* wt, const char* file);
WorkTree* htwt(const char* hash);
WorkTree* ftwt(const char* file);
WorkTree* stwt(const char* s);
void freeWt(WorkTree* wt);
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, const char* path);
WorkTree* getWtFromPath(const char* path);
void restoreWorkTree(WorkTree* wt, const char* path);
#endif