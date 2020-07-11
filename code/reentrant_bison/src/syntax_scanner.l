
%{ 
#include "syntax_parser.h"
%}
/*This is necessary to generate reentrant Flex scanner*/
%option reentrant 
/* It changes yylex() to yylex(void *) */
/* Now yylex(void *) should be called like this:
  void * scanner;
  yylex_init (& scanner);
  yylex (scanner);
   ...some stuff...
  yylex_destroy (scanner); 
*/

/*To use Flex with reentrant Bison set this:*/
%option bison-bridge noyywrap
/*Which causes yylex to have type:
int yylex ( YYSTYPE * lvalp, yyscan_t scanner ); */

blanks          [ \t\n]+ 
number          [0-9]+


%option noyywrap  

%% 

{blanks}    {  }

"+"	    return(PLUS); 
"("	    return(BEG);
")"	    return(END);
"*"	    return(MUL);

{number}	{ 
       
       char * end = yytext+strlen(yytext); 
       /*
       // You can't use yylval like this anymore in reentrant parser
       yylval.ival = strtol(yytext,  &end, 10);
       // Now yylval is a pointer is a pointer to YYSTYPE and instead of ival we have NUM
       // See syntax_parser.y for details
       */
       yylval->NUM = strtol(yytext,  &end, 10);
       return(NUM);
       
}
