
%{ 
#include "syntax_parser.h"
%}
%option reentrant bison-bridge noyywrap

letter          [^():\\+*]
string_start    ["]([^"\\]|\\.)*


%option noyywrap  

%% 

"+"     return(PLUS); 
"("     return(BEG);
")"     return(END);
":"     return(COLON);
"*"     return(STAR);
{string_start}["]   { int len = strlen(yytext);
                      yylval->STRING = malloc(len-2);
                      for(int i=0;i<len-2;i++){
                        yylval->STRING[i] = yytext[i+1]; 
                      }
                      return STRING; 
                    }
{string_start}      { /* Register unterminated string error */ }

