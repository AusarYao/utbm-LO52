%{
  #include <math.h>
  #define YYSTYPE double
  double yylval;
%}

  %start line
  %token NOMBRE
  %token FIN

%%

  ligne: exp FIN {printf("=%.2lf"' $1);};

  exp: exp exp '+' {$$ = $1 + $2;}
     | exp exp '-' {$$ = $1 - $2;}
     | exp exp '*' {$$ = $1 * $2;}
     | exp exp '/' {$$ = $1 / $2;}
     | exp exp '^' {$$ = pow($1, $2)}
     | NOMBRE      {$$ = $1;};

%%
