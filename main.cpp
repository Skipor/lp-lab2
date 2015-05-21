#include "node.h"
#include <iostream>

using namespace std;


int main() {
  NPattern * p = new NBool(true);
  NBlank * b = dynamic_cast<NBlank * > (p);
  if(b != nullptr) {
    cout << "Is blank" << endl;
  }



  cout << "Hello, World!" << endl;
  return 0;
}