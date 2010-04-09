%{
%}
alpha [a-z A-Z 0-9]
mot {alpha}+
%%
bon   {printf("good");}
{mot} {printf(yytext);}
%%
int main(){
  yylex();

  return;
}
