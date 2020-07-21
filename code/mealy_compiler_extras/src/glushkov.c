#include "formal_languages.h"
#include "glushkov.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
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
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
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
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
}

void inductivePrint(struct AST * root,int size,int indentation){
    struct T t = f(root,size);
    printT(&t,size);
    printSpaces(indentation);
    switch(root->type){
    case AST_UNION:
        printf("+\n");
        inductivePrint(root->uni.lhs,size,indentation+2);
        inductivePrint(root->uni.rhs,size,indentation+2);
        return;
    case AST_CONCAT:
        printf(".\n");
        inductivePrint(root->concat.lhs,size,indentation+2);
        inductivePrint(root->concat.rhs,size,indentation+2);
        return;
    case AST_KLEENE:
        printf("*\n");
        inductivePrint(root->kleene.child,size,indentation+2);
        return;
    case AST_OUTPUT:
        printf(":%s\n",root->output.outStr);
        inductivePrint(root->output.child,size,indentation+2);
        return;
    case AST_WEIGHT_AFTER:
    	printf("_ %d\n",root->weightAfter.weight);
        inductivePrint(root->weightAfter.child,size,indentation+2);
        return;
    case AST_WEIGHT_BEFORE:
    	printf("_ %d\n",root->weightBefore.weight);
        inductivePrint(root->weightBefore.child,size,indentation+2);
    case AST_RANGE:
        printf("[%d-%d]  [%c-%c]\n", 
        	root->range.from,root->range.to,
        	root->range.from,root->range.to);
        return;
    case AST_EPSILON:
        printf("epsilon\n");
        return;
    }
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
}


struct AST * substituteVars(struct Vars * root, struct hashmap_s * idToVars){
	switch(root->type){
    case VARS_UNION:
        return mkUnion(substituteVars(root->uni.lhs,idToVars),
        	substituteVars(root->uni.rhs,idToVars));
    case VARS_CONCAT:
        return mkConcat(substituteVars(root->concat.lhs,idToVars),
        	substituteVars(root->concat.rhs,idToVars));
    case VARS_KLEENE:
        return mkKleene(substituteVars(root->kleene.child,idToVars));
    case VARS_OUTPUT:
        return mkOutput(substituteVars(root->output.child,idToVars),
        	copyStr(root->output.outStr));
    case VARS_WEIGHT_AFTER:
        return mkWeightAfter(substituteVars(root->weightAfter.child,idToVars),
        	root->weightAfter.weight);
    case VARS_WEIGHT_BEFORE:
        return mkWeightBefore(substituteVars(root->weightBefore.child,idToVars),
        	root->weightBefore.weight);
    case VARS_RANGE:
        return mkRange(root->range.from,root->range.to);
    case VARS_VAR:{
    	char * id = root->var.id;
    	struct AST * defined = hashmap_get(idToVars,id,strlen(id));
    	if(defined==NULL){
    		fprintf(stderr,"Undefined variable %s\n",id);
    		exit(1);
    	}
        return copyAST(defined);
    }
    case VARS_EPSILON:
        return mkEpsilon();
    }
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
}