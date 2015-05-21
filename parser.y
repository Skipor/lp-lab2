%{
	#include "node.h"
        #include <cstdio>
        #include <cstdlib>
	std::unique_ptr<StatementList> program; /* the top level root node of our final AST */

	extern int yylex();
	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
%}

/* Represents the many different ways we can access our data */
%union {
	Node *node;
	NBlock *block;
	NExpression *expr;
	NStatement *stmt;
	NIdentifier *ident;
  StatementList * stmts;
	std::vector<NExpression*> *exprvec;
	std::string *string;
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
%token <token> TTYPEOF TCASE TARROW TENDL
/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> numeric expr
%type <varvec> func_pattern_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt 
%type <stmts> stmts
%type <token> comparison math_operator
/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV
%right TARROW

%start program

%%

program 
  : stmts {program.reset($1); }
  | {$$<stmts = nullptr; program.reset(new StatementList());}
  ;

stmts 
  : stmt { $$ = new StatementList(); $$->emplace_back($<stmt>1); }
  | stmts stmt { $1->emplace_back($<stmt>2); }
  ;

stmt 
  : type_decl | const_bind | func_bind | func_case
  ;

type_decl 
  : typename TTYPEOF type TENDL
    { 
      $3->types().reverse(); 
      $$ = new NTypeDeclaration($1, $3);
    }

type
  : TLPAREN type TRPAREN { $$ = $2;}
  | type TARROW type 
    {
      $$ = $3;
      $$->types().emplace_back($1);
    }
  | typename 
    { 
      $$ = new NType();
      $$->types().emplace_back($1);
    }
  ;


const_bind 
  : ident TEQUAL expr TENDL { $$ = new NVariableDeclaration(*$1, *$2, $4); }         
  ;

func_pattern_args 
  : func_pattern_args  func_pattern_arg { $1->push_back($<const_decl>3); }
  | func_pattern_arg { $1->push_back($<const_decl>3); }
  ;
func_pattern_arg 
  : TBLANK 
  | numeric 
  | TIDENTIFIER
    { 
      $$ = new VariableList();
      $$->push_back($<const_decl>1); 
    }
  ;

func_case 
  : TCASE expr TEQUAL TENDL
    {

    }
  ;

case_list 
  : func_case 
    {

    }
  | case_list TINTEND func_case TENDL
    {

    }
  ;

func_bind  
  : ident func_pattern_args case_list
    {

    }
  ;
 
extern_decl 
  : TEXTERN ident ident TLPAREN func_pattern_args TRPAREN
    {
      $$ = new NExternDeclaration(*$2, *$3, *$5); delete $5;
    }
  ;

ident 
  : TIDENTIFIER 
    { 
      $$ = new NIdentifier(std::move(*$1)); delete $1; 
    }
  ;
typename 
  : TTNAME 
    {
      $$ = new NPrimitiveType(std::move(*$1)); delete $1;
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
      $$ = new NMethodCall($1, $3);
      delete $3;
    }
  | ident { $<ident>$ = $1; }
  | numeric
    | expr math_operator expr { $$ = new NBinaryOperator($1, $2, $3); }
  | expr comparatison expr { $$ = new NBinaryOperator($1, $2, $3); }
    | TLPAREN expr TRPAREN { $$ = $2; }
  ;

call_args : /*blank*/  { $$ = new ExpressionList(); }
  | expr { $$ = new ExpressionList(); $$->push_back($1); }
  | call_args TCOMMA expr  { $1->push_back($3); }
  ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE;
math_operator : TMINUS | TPLUS | TDIV | TMUL ;

%%
