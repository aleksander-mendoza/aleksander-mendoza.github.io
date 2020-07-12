#ifndef H_GLUSHKOV
#define H_GLUSHKOV
#include "ast.h"

struct Transition{
    char input;
    int targetState;
    char* output;
};
struct Transitions{
    struct Transition * ts;
    int len;
};
struct M{
    int i;
    int stateCount;
    struct Transitions * delta;
    char** F;
};
struct T{
    char*** l;
    char** b;
    char** e;
    char* a;
};

int count(struct AST * root);
int localize(struct AST * root, int offset, char * stack);
void inductivePrint(struct AST * root,int size,int indentation);
void freeAST(struct AST * root);
struct T f(struct AST * root, int sigmaSize);
struct M TtoM(struct T * t,char * stack, int sigmaSize);
char * run(struct M * m, char * input);
void printM(struct M * m);
void freeTContents(struct T * t,int size);
void freeMContents(struct M * m);
#endif
