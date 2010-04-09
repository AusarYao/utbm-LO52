%{
  int nb_char = 0,
      nb_lig = 0;
%}
%%
.       {++nb_char;}
\n      {++nb_lig;}
<<EOF>> {printf("Nombre de lignes %d\n Nombre de char %d\n", nb_lig,
          nb_char);}
%%

int main(){
  yylex();

  return;
}
