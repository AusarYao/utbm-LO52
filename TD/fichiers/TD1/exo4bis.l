%{
  int nb_char = 0,
      nb_word = 0,
      nb_lig = 0;
%}
char [^ \t\n]
mot {char}+
%%
c     {++nb_char;}
{mot} {++nb_word; nb_char += yyleng;}
\n    {++nb_lig;}
%%
int main(){
  yylex();

  return;
}
