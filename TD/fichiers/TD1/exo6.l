%{
  int nb_parg = 0,
      nb_pard = 0;
%}
%%
"(" {++nb_parg;}
")" {++nb_pard; if(nb_pard > nb_parg) yyterminate();}
%%
int main(){
  yylex();

  if(nb_pard != nb_parg)
    printf("Erreur de parenthese\n");

  return;
}
