#ifndef H_FORMAL_LANGUAGES
#define H_FORMAL_LANGUAGES

char * copyStr(char * str);
char * concat(char * x,char * y);
char * epsilon();
char ** empty(int sigmaSize);
char ** singleton(char inputSymbol,char * outputString, int sigmaSize);
char** setConcatStr(char ** x,int size,char * y);
char** strConcatSet(char * x,int size,char ** y);
char*** concatProd(char ** x,char ** y,int size);
char*** empty2D(int size);
char* unionSingleton(char * lhs,char * rhs);
char** unionInPlaceLhs(char ** lhs,char ** rhs,int size);
char*** union2DInPlaceLhs(char *** lhs,char *** rhs,int size);
void free1D(char ** mat,int size);
void free2D(char *** mat,int size);
void free2DShallow(char *** mat,int size);
#endif