%{
  #define YYSTYPE double
  double yylval;
  double mem;

  typedef struct symrec {
    char *nom;
    double var;
    struct symrec *next;
  } symrec;
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

  %union {double val, symrec *symptr;}
  %token <val> NUM
  %token <val> exp
  %token <symptr> VAR

%%

  ligne: exp FIN {printf("=%d"' $1);};

  exp: exp '+' exp {$$ = $1 + $2;}
     | exp '-' exp {$$ = $1 - $2;}
     | exp '*' exp {$$ = $1 * $2;}
     | exp '/' exp {$$ = $1 / $2;}
     | exp '^' exp {$$ = pow($1, $2);}
     | NUM         {$$ = $1;}
     | VAR         {$$ = $1->var;}
     | VAR '=' exp {$1->var = $3; $$ = $3;}
     | STO exp     {$$ = $2; mem = $2;}
     | VAR exp     {$$ = mem;}
     | '(' exp ')' {$$ = $1;}
     | DOUBLE      {$$ = $1;}
     | ENTIER      {$$ = $1;};
%%
