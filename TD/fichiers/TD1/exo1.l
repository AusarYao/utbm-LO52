%{
%}
hello bonjour
good bon
%%
{hello} {printf("hello";}
{good} {printf("good";}
%%
int main(){
  yylex();

  return;
}
