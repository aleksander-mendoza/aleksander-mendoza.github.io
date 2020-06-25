%{

#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
int yylex();

int yyerror(char *s);

int yy_scan_string(char * s);

void printSpaces(int indentation){
    for(int i=0;i<indentation*2;i++){
        printf(" ");
    }
}


int count(struct AST * root){
    switch(root->type){
    case 0:
        return count(root->uni.lhs)+count(root->uni.rhs);
    case 1:
        return count(root->concat.lhs)+count(root->concat.rhs);
    case 2:
        return count(root->kleene.child);
    case 3:
        return count(root->output.child);
    case 4:
        return 1;
    case 5:
        return 0;
    }
}


int localize(struct AST * root, int offset, char * stack){
    switch(root->type){
    case 0:
        return localize(root->uni.rhs,
            localize(root->uni.lhs,offset,stack),stack);
    case 1:
        return localize(root->concat.rhs,
            localize(root->concat.lhs,offset,stack),stack);
    case 2:
        return localize(root->kleene.child,offset,stack);
    case 3:
        return localize(root->output.child,offset,stack);
    case 4:
        stack[offset] = root->letter.literal;
        root->letter.literal = (char)offset;
        return offset+1;
    case 5:
        return offset;
    }
}
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
void free1D(char ** mat,int size){
    for(int i=0;i<size;i++){
        free(mat[i]);
    }
    free(mat);
}
void free2D(char *** mat,int size){
    for(int i=0;i<size;i++){
        free1D(mat[i],size);
    }
    free(mat);
}
void free2DShallow(char *** mat,int size){
    for(int i=0;i<size;i++){
        free(mat[i]);
    }
    free(mat);
}
void freeMContents(struct M * m){
    if(m->i==-1)return;
    free1D(m->F,m->stateCount);
    for(int i=0;i<m->stateCount;i++){
        for(int j = 0;j<m->delta[i].len;j++){
            free(m->delta[i].ts[j].output);
        }
        free(m->delta[i].ts);
    }
    free(m->delta);
    m->i=-1;
}
char* outputFor(struct M * m, int sourceState, int targetState, int inputSymbol){
    for(int tran=0;tran<m->delta[sourceState].len;tran++){
        if(m->delta[sourceState].ts[tran].input==inputSymbol && 
            m->delta[sourceState].ts[tran].targetState==targetState){
            return m->delta[sourceState].ts[tran].output;
        }
    }
    return NULL;
}
struct T{
    char*** l;
    char** b;
    char** e;
    char* a;
};
void freeTContents(struct T * t,int size){
    free(t->a);
    free1D(t->b,size);
    free1D(t->e,size);
    free2D(t->l,size);
}
void printT(struct T * t, int size){
    
    for(int i=0;i<size;i++){
        printf("l:");
        for(int j=0;j<size;j++){
            printf("'%s' ",t->l[i][j]);
        }
        printf("\n");
    }
    printf("b:");
    for(int i=0;i<size;i++){
        printf("'%s' ",t->b[i]);
    }
    printf("\n");
    printf("e:");
    for(int i=0;i<size;i++){
        printf("'%s'(%p) ",t->e[i],t->e[i]);
    }
    printf("\n");
    printf("a:'%s'\n",t->a);
}
void printM(struct M * m){
    printf("Initial state=%d\n",m->i);
    printf("State count=%d\n",m->stateCount);
    printf("Moore output=");
    for(int i=0;i<m->stateCount;i++){
        printf(" '%s'",m->F[i]);
    }
    printf("\n");
    printf("Transitions:\n");
    for(int i=0;i<m->stateCount;i++){
        for(int j=0;j<m->delta[i].len;j++){
            printf("From %d to %d over '%c' printing \"%s\"\n",i,m->delta[i].ts[j].targetState,m->delta[i].ts[j].input,m->delta[i].ts[j].output);
        }
    }
}
char * run(struct M * m, char * input){
    int len = strlen(input);
    //we will use backtracking mechanism for
    //evaluation of all nondeterministic
    //superpositions of automaton
    int backtrack[len+1][m->stateCount];
    for(int state=0;state<m->stateCount;state++)
        for(int step=0;step<len+1;step++)
            backtrack[step][state]=-1;
    backtrack[0][m->i]=m->i;
    //first we need to propagate input forwards for each symbol 
    for(int step=1;step<=len;step++){
        char inputSymbol = input[step-1];
        for(int state=0;state<m->stateCount;state++){
            if(backtrack[step-1][state]>-1){
                for(int tran=0;tran<m->delta[state].len;tran++){
                    // remember that there might me multiple nondeterministic
                    // transitions for given state and inputSymbol
                    if(m->delta[state].ts[tran].input==inputSymbol){
                        int targetState = m->delta[state].ts[tran].targetState;
                        if(backtrack[step][targetState]>-1){
                            printf("Nondeterminism at step %d in state %d",step,targetState);
                            exit(1);
                        }
                        backtrack[step][targetState] = state; // this will allow us to
                        //backtrack later and determine printed output
                    }
                }
            }
        }
    }
    //now we need to check if any of the accepting states is reached
    int acceptedState = -1;
    for(int state=0;state<m->stateCount;state++){
        if(backtrack[len][state]>-1){
            if(acceptedState==-1){
                acceptedState = state;
            }else{
                printf("Nondeterminism at final step in states %d and %d",acceptedState,state);
                exit(1);
            }
        }
    }
    if(acceptedState==-1){
        //no state accepted, so we return NULL (empty set) as output
        return NULL;
    }
    //now we need to backtrack and collect output printed along each transition
    int sizeOfOutput = strlen(m->F[acceptedState]);
    int backtrackedState = acceptedState;
    for(int step = len;step>0;step--){
        int sourceState = backtrack[step][backtrackedState];
        sizeOfOutput += strlen(outputFor(m,sourceState,backtrackedState,input[step-1]));
        backtrackedState = sourceState;
    }
    char * output = malloc(sizeof(char)*sizeOfOutput+1);
    output[0]='\0';
    strcat(output,m->F[acceptedState]);
    backtrackedState = acceptedState;
    for(int step = len;step>0;step--){
        int sourceState = backtrack[step][backtrackedState];
        char * stepOutput = outputFor(m,sourceState,backtrackedState,input[step-1]);
        int stepOutputLen = strlen(stepOutput);
        memmove(output + stepOutputLen, output, strlen(output) + 1);
        memcpy(output,stepOutput,stepOutputLen);
        backtrackedState = sourceState;
    }
    return output;
}
char * copyStr(char * str){
    if(str==NULL)return NULL;
    char * n = malloc(sizeof(char)*(strlen(str)+1));
    n[0]='\0';
    strcat(n,str);
    return n;
}
struct M TtoM(struct T * t,char * stack, int sigmaSize){
    struct M m;
    m.stateCount = sigmaSize+1;//there is one state for every symbols in 
    //Sigma plus one extra initial state 
    m.i = sigmaSize;
    m.F = malloc(sizeof(char*)*m.stateCount);
    for(int i=0;i<sigmaSize;i++){
        m.F[i] = copyStr(t->e[i]);
    }
    m.F[sigmaSize] = copyStr(t->a);
    m.delta = malloc(sizeof(struct Transitions)*m.stateCount);
    // delta stores an array of transitions for each state
    for(int i=0;i<sigmaSize;i++){
        int transitionCount = 0;//first find out how many outgoing
        //transitions originate in state i
        for(int j=0;j<sigmaSize;j++)if(t->l[i][j])transitionCount++;
        //then allocate array of transitions
        m.delta[i].len = transitionCount;
        struct Transition * tr = m.delta[i].ts = malloc(sizeof(struct Transition)*transitionCount);
        //then collect all the transitions 
        for(int j=0,k=0;j<sigmaSize;j++){
            char* output = copyStr(t->l[i][j]);
            if(output){
                tr[k].targetState = j; //j is the target state of transition
                tr[k].input = stack[j]; // stack[j] tells us the input label of transition
                tr[k].output = output;
                k++;
            }
        }
    }
    //now it's time to connect initial state with the rest of states
    int initialTransitionCount = 0;

    for(int j=0;j<sigmaSize;j++)if(t->b[j])initialTransitionCount++;
    
    m.delta[sigmaSize].len = initialTransitionCount;
    struct Transition * initialTr = m.delta[sigmaSize].ts = 
        malloc(sizeof(struct Transition)*initialTransitionCount);

    for(int j=0,k=0;j<sigmaSize;j++){
        char* output = copyStr(t->b[j]);
        if(output){
            initialTr[k].targetState = j; 
            initialTr[k].input = stack[j];
            initialTr[k].output = output;
            k++;
        }
    }
    //lastly you could sort all transitions by input in increasing order,
    //which would later allow you to implement binary search. I want to
    //keep things simple so I won't do it here.
    return m;
}

char * concat(char * x,char * y){
    if(x==NULL || y==NULL)return NULL;
    char * n = malloc(sizeof(char)*(strlen(x)+strlen(y)+1));
    strcpy(n,x);
    strcat(n,y);
    return n;
}
char * epsilon(){
    char * n = malloc(sizeof(char)*1);
    n[0]='\0';
    return n;
} 
char ** empty(int sigmaSize){
    char ** n = malloc(sizeof(char*)*sigmaSize);
    for(int i=0;i<sigmaSize;i++){
        n[i] = NULL;
    }
    return n;
}
char ** singleton(char inputSymbol,char * outputString, int sigmaSize){
    char ** n = empty(sigmaSize);
    n[inputSymbol] = outputString;
    return n;
}
char** setConcatStr(char ** x,int size,char * y){
    char ** n = malloc(sizeof(char*)*size);
    for(int i = 0;i<size;i++){
        n[i] = concat(x[i],y);
    }
    return n;
}
char** strConcatSet(char * x,int size,char ** y){
    char ** n = malloc(sizeof(char*)*size);
    for(int i = 0;i<size;i++){
        n[i] = concat(x,y[i]);
    }
    return n;
}
char*** concatProd(char ** x,char ** y,int size){
    char *** n = malloc(sizeof(char**)*size);
    for(int i = 0;i<size;i++){
        n[i] = malloc(sizeof(char*)*size);
    }
    for(int i = 0;i<size;i++){
        for(int j = 0;j<size;j++){
            n[i][j] = concat(x[i],y[j]);
        }
    }
    return n;
}
char*** empty2D(int size){
    char *** n = malloc(sizeof(char**)*size);
    for(int i = 0;i<size;i++){
        n[i] = malloc(sizeof(char*)*size);
        for(int j = 0;j<size;j++){
            n[i][j] = NULL;
        }
    }
    return n;
}
char* unionSingleton(char * lhs,char * rhs){
    if(lhs){
        if(rhs){
            printf("Nondeterminism!");
            exit(1);
        }else{
            return lhs;
        }
    }else{
        return rhs;    
    }
}
char** unionInPlaceLhs(char ** lhs,char ** rhs,int size){
    for(int i=0;i<size;i++){
        lhs[i]=unionSingleton(lhs[i],rhs[i]);
    }
    return lhs;
}
char*** union2DInPlaceLhs(char *** lhs,char *** rhs,int size){
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            lhs[i][j]=unionSingleton(lhs[i][j],rhs[i][j]);
        }
    }
    return lhs;
}

struct T f(struct AST * root, int sigmaSize){
    switch(root->type){
    case 0:{
        struct T x = f(root->uni.lhs,sigmaSize);
        struct T y = f(root->uni.rhs,sigmaSize);
        x.l = union2DInPlaceLhs(x.l,y.l,sigmaSize);
        x.e = unionInPlaceLhs(x.e,y.e,sigmaSize);
        x.b = unionInPlaceLhs(x.b,y.b,sigmaSize);
        x.a = unionSingleton(x.a,y.a);
        free2DShallow(y.l,sigmaSize);
        free(y.e);
        free(y.b);
        return x;
    }
    case 1:{
        struct T x = f(root->concat.lhs,sigmaSize);
        struct T y = f(root->concat.rhs,sigmaSize);
        char *** oldxl = x.l;
        x.l = union2DInPlaceLhs(concatProd(x.e,y.b,sigmaSize),union2DInPlaceLhs(x.l,y.l,sigmaSize),sigmaSize);
        char ** oldxb = x.b;
        x.b = unionInPlaceLhs(strConcatSet(x.a,sigmaSize,y.b),x.b,sigmaSize);
        char ** oldxe = x.e;
        x.e = unionInPlaceLhs(setConcatStr(x.e,sigmaSize,y.a),y.e,sigmaSize);
        x.a = concat(x.a,y.a);
        free2DShallow(oldxl,sigmaSize);
        free2DShallow(y.l,sigmaSize);
        free(y.e);
        free1D(y.b,sigmaSize);
        free1D(oldxe,sigmaSize);
        free(oldxb);
        return x;
    }
    case 2:{
        struct T x = f(root->kleene.child,sigmaSize);
        if(x.a && x.a[0]!='\0'){
            printf("Nondeterminism!");
            exit(1);
        }
        char *** oldxl = x.l;
        x.l = union2DInPlaceLhs(concatProd(x.e,x.b,sigmaSize),x.l,sigmaSize);
        free2DShallow(oldxl,sigmaSize);
        return x;
    }
    case 3:{
        struct T x = f(root->output.child,sigmaSize);
        char ** oldxe = x.e;
        x.e = setConcatStr(x.e,sigmaSize,root->output.outStr);
        free1D(oldxe,sigmaSize);
        char * oldxa = x.a;
        x.a = concat(x.a,root->output.outStr);
        free(oldxa);
        return x;
    }
    case 4:{
        struct T x;
        x.l = empty2D(sigmaSize);
        x.b = singleton(root->letter.literal,epsilon(),sigmaSize);
        x.e = singleton(root->letter.literal,epsilon(),sigmaSize);
        x.a = NULL;
        return x;
    }
    case 5:{
        struct T x;
        x.l = empty2D(sigmaSize);
        x.b = empty(sigmaSize);
        x.e = empty(sigmaSize);
        x.a = epsilon();
        return x;
    }
    }
}
void freeAST(struct AST * root){
    switch(root->type){
    case 0:
        freeAST(root->uni.lhs);
        freeAST(root->uni.rhs);
        free(root);
        return;
    case 1:
        freeAST(root->concat.lhs);
        freeAST(root->concat.rhs);
        free(root);
        return;
    case 2:
        freeAST(root->kleene.child);
        free(root);
        return;
    case 3:
        freeAST(root->output.child);
        free(root->output.outStr);
        free(root);
        return;
    case 4:
        free(root);
        return;
    case 5:
        free(root);
        return;
    }
}
void inductivePrint(struct AST * root,int size,int indentation){
    struct T t = f(root,size);
    printT(&t,size);
    printSpaces(indentation);
    switch(root->type){
    case 0:
        printf("+\n");
        inductivePrint(root->uni.lhs,size,indentation+2);
        inductivePrint(root->uni.rhs,size,indentation+2);
        return;
    case 1:
        printf(".\n");
        inductivePrint(root->concat.lhs,size,indentation+2);
        inductivePrint(root->concat.rhs,size,indentation+2);
        return;
    case 2:
        printf("*\n");
        inductivePrint(root->kleene.child,size,indentation+2);
        return;
    case 3:
        printf(":%s\n",root->output.outStr);
        inductivePrint(root->output.child,size,indentation+2);
        return;
    case 4:
        printf("%c %d\n", root->letter.literal,root->letter.literal);
        return;
    case 5:
        printf("epsilon\n");
        return;
    }
}
struct M bisonOutput = {.i=-1,.stateCount=-1,.delta=NULL,.F=0};
/*
*/
%}

%union
{
    char * strval;
    struct AST * ast;
};

%token <strval> STRING
%token BEG
%token END
%token PLUS
%token STAR
%token COLON

%type<ast> Union
%type<ast> Concat
%type<ast> Kleene
%type<ast> Letter
%type<ast> Output

%start Start

%%
Start:
    Union { 
        int sigmaSize = count($1);
        char * stack = malloc(sizeof(char)* sigmaSize);
        localize($1,0,stack);
        struct T t = f($1,sigmaSize);
        freeMContents(&bisonOutput);
        bisonOutput = TtoM(&t,stack,sigmaSize);
        freeTContents(&t,sigmaSize);
        free(stack);
        freeAST($1);
    }
    | 
    ;

Union:
    Concat PLUS Union { $$ = mkUnion($1,$3); }
    | Concat {$$ = $1;}
    ;

Concat:
    Kleene Concat { $$ = mkConcat($1,$2); }
    | Kleene {$$ = $1;}
    ;

Kleene: 
    Output {$$ = $1;}
    | Output STAR { $$ = mkKleene($1); }
    ;

Output: 
    Letter COLON STRING { $$ = mkOutput($1, $3);}
    | Letter { $$ = $1; }
    ;

Letter:
    STRING { 
        $$ = mkEpsilon();
        for(int i=0;($1)[i]!=0;i++){
            $$ = mkConcat($$,mkLetter(($1)[i])); 
        }
    }
    | BEG Union END  { $$ = $2; }
    ;

%%

int yyerror(char *s) {
  printf("yyerror : %s\n",s);
}
EMSCRIPTEN_KEEPALIVE
void compile(char * string){
    yy_scan_string(string);
    yyparse();
    printM(&bisonOutput);
}
EMSCRIPTEN_KEEPALIVE
int testInt(){
    return 69420;
}
EMSCRIPTEN_KEEPALIVE
char * run_global(char * input){
    char * c = run(&bisonOutput,input);
    printf("Out=%s\n",c);
    return c;
}

int main(void) {
    
    
}