%{
  #define YYDEBUG 1
	#include "node.h"
  #include <cstdio>
  #include <cstdlib>
	std::unique_ptr<StatementList> program; /* the top level root node of our final AST */

	extern int yylex();
	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }

/* #define YYPRINT(file, type, value)   yyprint (file, type, value)
static void
yyprint ( FILE *file, int type, YYSTYPE value) {
  if (type == TBOOL || type == TIDENTIFIER || type == TTNAME || type == TDOUBLE || type == TINTEGER) {
  fprintf (file, " %s", (*value.string).c_str());
  }
} */
%}

/* Represents the many different ways we can access our data */
%union {
	NExpression *expr;
  ExpressionList * exprs;
	NStatement *stmt;
  StatementList * stmts;
	NIdentifier *ident;
  NPrimitiveType	* name_of_type;
	std::string *string;
  NPattern * pattern;
  PatternList * patterns;
  CaseToExpr * case_to_expr;
  CaseToExprList* case_list; 
  NDefinition * def;
  NType * type;
	int intval;
	int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */

%token <intval> TINTEND
%token <string> TTNAME TIDENTIFIER TINTEGER TDOUBLE TBOOL
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TCOMMA 
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TBLANK TTYPEOF TCASE TARROW TENDL
/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <name_of_type> name_of_type
%type <type> type 
%type <pattern> func_pattern_arg
%type <patterns> func_pattern_args
%type <expr> expr numeric 
%type <exprs> call_args
%type <stmt> stmt type_decl func_bind func_case
%type <stmts> stmts program
%type <case_to_expr> case
%type <case_list> case_list
%type <def> func_pattern
%type <token> equ_op cmp_op add_op mul_op
/* Operator precedence for mathematical operators */

%right TARROW
%left TCEQ TCNE
%left TCLT  TCLE  TCGT  TCGE
%left TPLUS TMINUS
%left TMUL TDIV


%start program
%verbose

%printer { fprintf(yyoutput, "%s", $$->c_str()); } TDOUBLE TINTEGER TTNAME TIDENTIFIER



%%

program 
  : stmts {$$ = $1; program.reset($$); }
  | { $$ = new StatementList(); program.reset($$);}
  ;

stmts 
  : stmt { $$ = new StatementList(); $$->emplace_back($<stmt>1); }
  | stmts stmt { $1->emplace_back($<stmt>2); }
  | stmts TENDL
  ;

stmt 
  : type_decl | func_bind | func_case ;

type_decl 
  : ident TTYPEOF type TENDL
    { 
      std::reverse($3->types().begin(),$3->types().end());
      $$ = new NTypeDeclaration($1, $3);
    }

type
  : TLPAREN type TRPAREN { $$ = $2;}
  | type TARROW type 
    {
      $$ = $3;
      $$->types().emplace_back($1);
    }
  | name_of_type
    { 
      $$ = new NType();
      $$->types().emplace_back($1);
    }
  ;



func_pattern_args 
  : func_pattern_args  func_pattern_arg { $1->emplace_back($2); }
  | func_pattern_arg { $$ = new PatternList(); $$->emplace_back($1); }
  ;

func_pattern_arg 
  : TBLANK {$$ = new NBlank();}
  | numeric {$$ = dynamic_cast<NPattern*>($1);}
  | ident {$$ = dynamic_cast<NPattern*>($1);}
  ;

case 
  : TCASE expr TEQUAL expr TENDL
    {
      $$ = new CaseToExpr(std::unique_ptr<NExpression>($2), std::unique_ptr<NExpression>($4));
    }
  ;

case_list 
  : case 
    {
      $$ = new CaseToExprList();
      $$->emplace_back(std::move(*$1));
      delete $1;
    }
  | case_list TINTEND case 
    {
      $1->emplace_back(std::move(*$3));
      delete $3;
    }
  ;

func_pattern 
  : ident func_pattern_args
    {
      $$ = new NDefinition($1, $2); 
    }
  | ident
    {
      $$ = new NDefinition($1, new PatternList());
    }
  ;

func_bind  
  : func_pattern  TEQUAL expr TENDL
    {
      $$ = new NFunctionPatternDef($1, $3);
    }
  ;

func_case  
  : func_pattern case_list
    {
      $$ = new NFunctionCaseDef($1, $2);
    }
  ;
 
ident 
  : TIDENTIFIER 
    { 
      $$ = new NIdentifier(std::move(*$1));
      delete $1; 
    }
  ;
name_of_type
  : TTNAME 
    {
      $$ = new NPrimitiveType(std::move(*$1));
      delete $1;
    }
  ;

numeric 
  : TINTEGER 
    {
      $$ = new NInteger(atol($1->c_str()));
      delete $1; 
    }
  | TDOUBLE 
    {
      $$ = new NDouble(atof($1->c_str()));
      delete $1; 
    }
  | TBOOL  
    {
      $$ = new NBool(*$1 == "true");
      delete $1; 
    }
  ;

expr 
  : ident TLPAREN call_args TRPAREN 
    {
      $$ = new NFunctionCall($1, $3);
    }
  | ident {$$ = $<expr>1;}
  | numeric
  | expr equ_op expr { $$ = new NBinaryOperator($1, $2, $3); } %prec TCEQ
  | expr cmp_op expr { $$ = new NBinaryOperator($1, $2, $3); } %prec TCLT
  | expr add_op expr { $$ = new NBinaryOperator($1, $2, $3); } %prec TPLUS
  | expr mul_op expr { $$ = new NBinaryOperator($1, $2, $3); } %prec TMUL
  | TLPAREN expr TRPAREN { $$ = $2; }
  ;

call_args : /*blank*/  { $$ = new ExpressionList(); }
  | expr { $$ = new ExpressionList(); $$->emplace_back($1); }
  | call_args TCOMMA expr  { $1->emplace_back($3); }
  ;

equ_op: TCEQ | TCNE ;
cmp_op: TCLT  | TCLE  | TCGT  | TCGE;
add_op: TPLUS | TMINUS;
mul_op: TMUL | TDIV;


%%
