#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stack>
#include <functional>

#include "lex.hpp"
#include "ast.hpp"
#include "tachyon.hpp"

namespace tach{
  namespace file{
    std::string read(std::string arg0){
      // cout << arg0 << endl;
      std::string s;
      std::ifstream f(arg0);
      std::string ret;
      while (std::getline(f, s)){
        ret += s + "\n";
      }
      return ret;
    }
  }
  void exec(std::string code){
    std::vector<lex::token> tokens = lex::lex(code);
    tree::node prog = tree::tree(tokens);
    tach::vm::vm_inst v(prog);
  }
}


int main(int argc, char** argv){
  std::vector<std::string> args;
  for (int i = 0; i < argc; i++){
    args.push_back(argv[i]);
  }
  tach::exec(tach::file::read(args[1]));
}
