%{

  #include <stdlib.h>
  #include "exo1.tab.h"

  extern YYSTYPE yylval;

%}

blancs [ \t]+
chiffre [0-9]
entier [+-]?[1-9]{chiffre}*|0
double {entier}('.'{entier}+)?

%%

  {blancs} {}
  {reel} {yylval = atof(yytext); return NOMBRE;}
  .      {return yytext[0];}
  \n     {return FIN;}

%%
