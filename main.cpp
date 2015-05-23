#include "node.h"
#include <iostream>

extern std::unique_ptr<StatementList> program; /* the top level root node of our final AST */
extern int yydebug;
extern int yyparse();

using namespace std;



int main(int argc, char **argv)
{
  cout << "BEFORE PARSE" << endl;
  yydebug = true;
  yyparse();
  cout << "AFTER PARSE" << endl;

  for(auto & statement_ptr : *program) {
    cerr << *statement_ptr << endl;
  }

  return 0;
}
