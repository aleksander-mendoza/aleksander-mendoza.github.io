%define api.pure full
%lex-param {void *scanner}
%parse-param {void *scanner} {struct M * out}

%{
#include "glushkov.h"
#include "syntax_parser.h"
#include "lex.yy.h"
#include <stdio.h>
void yyerror (void *scanner, struct M * out, char const *msg);
%}

/*
//You can't use %union anymore
%union
{  
    int ival; 
};
//Instead use api.value.type
*/

%define api.value.type union

%token <char*> STRING
%token BEG
%token END
%token PLUS
%token STAR
%token COLON

%type<struct AST *> Union
%type<struct AST *> Concat
%type<struct AST *> Kleene
%type<struct AST *> Letter
%type<struct AST *> Output

%start Start

%%
Start:
    Union { 
        int sigmaSize = count($1);
        char * stack = malloc(sizeof(char)* sigmaSize);
        localize($1,0,stack);
        struct T t = f($1,sigmaSize);
        *out = TtoM(&t,stack,sigmaSize);
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

void yyerror (void *scanner, struct M *out, char const *msg) {
    fprintf(stderr, "--> %s\n", msg);
}

int main(void) {
    yyscan_t scanner;
    yylex_init(&scanner);
    struct M m = {.i=-1,.stateCount=-1,.delta=NULL,.F=0};
    yyset_in(STDIN_FILENO, scanner);
    int output_code = yyparse(scanner, &m);
    if(output_code==0){
        printM(&m);
    }else{
        printf("Syntax error\n"); 
    }
    freeMContents(&m);
    yylex_destroy(scanner);

    
}