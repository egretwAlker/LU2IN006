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

void setMode(int mode, char* path);
WorkFile* createWorkFile(char* name);
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);
void freeWf(WorkFile* wf);
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int
mode);
char* wtts(WorkTree* wt);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(const char* file);
WorkTree* stwt(const char* s);
void freeWt(WorkTree* wt);
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, char* path);
WorkTree* getWtFromPath(char* path);
#endif