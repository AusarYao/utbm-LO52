%{
  int nb_char = 0,
      nb_word = 0,
      nb_lig = 0;
%}
alpha [a-z A-Z 0-9]
mot {alpha}+
%%
{alpha} {++nb_char;}
{mot}   {++nb_word; nb_char += yyleng;}
\n      {++nb_lig;}
%%
int main(){
  yylex();

  return;
}
