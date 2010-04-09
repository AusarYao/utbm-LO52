%{
  #define YYSTYPE double
  double yylval;
  double mem;
%}

  %right '='
  %left '+'
  %left '-'
  %left '*'
  %left '/'

  %start line

  %union {int val_ent, double val_reel;}
  %token <val_ent> ENTIER
  %token <val_reel> DOUBLE
  %token FIN
  %token RCL
  %token STO

  %type <val_reel> exp

%%

  ligne: exp FIN {printf("=%.2lf"' $1);};

  exp: exp '+' exp {$$ = $1 + $2;}
     | exp '-' exp {$$ = $1 - $2;}
     | exp '*' exp {$$ = $1 * $2;}
     | exp '/' exp {$$ = $1 / $2;}
     | exp '^' exp {$$ = pow($1, $2);}
     | STO exp     {$$ = $2; mem = $2;}
     | RCL exp     {$$ = mem;}
     | '(' exp ')' {$$ = $1;}
     | DOUBLE      {$$ = $1;}
     | ENTIER      {$$ = $1;};
%%
