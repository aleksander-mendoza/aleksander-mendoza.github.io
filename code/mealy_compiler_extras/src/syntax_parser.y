//This is necessary to tell bison to generate reentrant parser
%define api.pure full
//This specifies additional arguments to yylex()
%lex-param {void *scanner}
//This specifies additional arguments to yyparse()
%parse-param {void *scanner} {struct BisonOutput *out}
/*We use 
  void *scanner
but in reality it is 
  YYSTYPE *scanner
Unfortunately using YYSTYPE would introduce circular 
dependency between flex and bison header so we use void* instead
(it is a common work-around used in many flex&bison programs)
*/
%{

/*This struct will be used by syntax_parser.h, 
so make sure you define it before #include "syntax_parser.h" */
struct BisonOutput{
    int out; 
    //add here any fields you wish
}; /*This will be later used to return output from bison 
in the code associated with Start rule in grammar.
*/
//You have to include this first (contains definition of YYSTYPE )
#include "syntax_parser.h"
// and then include this (contains yyscan_t, but for us it's just a void*)
#include "lex.yy.h"
#include <stdio.h>


void yyerror (void *scanner, struct BisonOutput *out, char const *msg);
%}

/*
//You can't use %union anymore
%union
{  
    int ival; 
};
//Instead use api.value.type
*/

%define api.value.type union /* Generate YYSTYPE from types of tokens. 
In this case there is only one <int> type and it belongs to NUM. 
Hence Bison will generate the following union:

union YYSTYPE{
    // This is instead of ival
    int NUM; 
    // The rest is not really important for us, but Bison needs it
    int Addition; 
    int Multiplication; 
    int Atomic;
}
*/

%token <int> NUM
%token BEG
%token END
%token PLUS
%token MUL

%type<int> Addition
%type<int> Multiplication
%type<int> Atomic

%start Start

%%
Start:
   Addition { 
        /*We can use 
          BisonOutput *out 
        because we added it in %parse-param*/
        out->out = $1;
       
   }
   | /*this matches empty string*/
   ;

Addition:
    Multiplication PLUS Addition { 
     $$ = $1 + $3; 
    }
    | Multiplication 
    ;

Multiplication:
    Atomic MUL Multiplication { $$ = $1 * $3; }
    | Atomic
    ;

Atomic:
    NUM { $$ = $1; }
    | BEG Addition END  { $$ = $2; }
    ;

%%

void yyerror (void *scanner, struct BisonOutput *out, char const *msg) {
    fprintf(stderr, "--> %s\n", msg);
}

int main(void) {
    yyscan_t scanner;
    yylex_init(&scanner);
    struct BisonOutput out;//this is used to received output back from bison
    yyset_in(STDIN_FILENO, scanner);
    int output_code = yyparse(scanner, &out);
    if(output_code==0){
        printf("Final result=%d\n",out.out); 
    }else{
        printf("Syntax error\n"); 
    }
    yylex_destroy(scanner);

    
}