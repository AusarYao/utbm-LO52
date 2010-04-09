%{
  int nb_voy = 0,
      nb_con = 0,
      nb_pct = 0;
%}
voyelle [a e i o u y]
consonne [b-d f-h j-n p-t v-x z]
ponctu [.  ' " , ;]
%%
{voyelle} {++nb_voy;}
{consonne} {++nb_con;}
{ponctu} {++nb_pct;}
%%
int main(){
  yylex();

  return;
}
