%{

  #include <stdlib.h>
  #include "exo1.tab.h"

  extern YYSTYPE yylval;

%}

blancs    [ \t]+
chiffre   [0-9]
lettre    [a-zA-Z]
alphanum  [0-9a-zA-Z]
var       {lettre}{alphanum}*

%%

  {blancs} {}
  .        {return yytext[0];}
  {entier} {yylval.val = atoi(yytext); return NUM;}
  {var}    {symrec *s = getsymbole(yytext);
            if(!s) s = putsymbole(yytext);
            yyval.symptr = s;
            return VAR;}
  \n       {return FIN;}

%%
