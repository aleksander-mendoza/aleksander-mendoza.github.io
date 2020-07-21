#include "formal_languages.h"

char * copyStr(char * str){
    if(str==NULL)return NULL;
    char * n = malloc(sizeof(char)*(strlen(str)+1));
    n[0]='\0';
    strcat(n,str);
    return n;
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
