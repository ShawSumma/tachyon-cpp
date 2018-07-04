bool svecin(std::vector<std::string> v, std::string to){
  for (std::string s: v){
    if (!s.compare(to)){
      return true;
    };
  }
  return false;
}

namespace tach{
  namespace lex{
    int line = 1;
    struct token{
      std::string type;
      std::string value;
      int lineno;
      token(std::string co, std::string va){
        type = co;
        value = va;
        lineno = line;
      }
      operator std::string(){
        std::string sw = "                ";
        for (int i = 0; i < type.size(); i++){
          sw = sw.substr(1);
        }
        return type + sw + value;
      }
    };
    std::string operl = std::string("*/+-~!@#$%&*|\\<>/=");
    bool isoper(char c){
      return operl.find(std::string({c})) != -1;
    }
    std::vector<token> lex(std::string code){
      std::vector<token> tokens;
      int max_token = -1;
      int place = 0;
      int codesize = code.size();
      int tokent = 256;
      while (true){
        if (place >= codesize){
          return tokens;
        }
        char first = code.substr(place, place+1).c_str()[0];
        if (first == '\t' || first == ' '){
          place ++;
          continue;
        }
        if (first == '/' && code.substr(place+1, place+2).c_str()[0] == '/'){
          place += 2;
          while (code.substr(place, place+1).c_str()[0] != '\n'){
            place++;
          }
          continue;
        }
        if (first == '"'){
          std::string s;
          place ++;
          do {
            first = code.substr(place, place+1).c_str()[0];
            place ++;
          } while (first != '"');
          tokens.push_back(token("string", s));
          tokent = -1;
          max_token++;
          continue;
        }
        if (isalpha(first)){
          if (tokent != 0){
            tokens.push_back(token("name", std::string({first})));
            max_token++;
            tokent = 0;
          }
          else{
            tokens[max_token].value += first;
          }
          place++;
          continue;
        }
        if (isdigit(first)){
          if (tokent <= 2){
            tokens[max_token].value += first;
          }
          else{
            tokens.push_back(token("int", std::string({first})));
            max_token++;
            tokent = 1;
          }
          place++;
          continue;
        }
        if (first == '\n' || first == ';'){
          if (first == '\n'){
            line++;
          }
          tokent = 3;
          tokens.push_back(token("newline", std::string({first})));
          max_token++;
          place++;
          continue;
        }
        if (first == '('){
          tokent = 4;
          tokens.push_back(token("lparen", "("));
          max_token++;
          place++;
          continue;
        }
        if (first == ')'){
          tokent = 5;
          tokens.push_back(token("rparen", ")"));
          max_token++;
          place++;
          continue;
        }
        if (first == '{'){
          tokent = 6;
          tokens.push_back(token("lcurly", "{"));
          max_token++;
          place++;
          continue;
        }
        if (first == '}'){
          tokent = 7;
          tokens.push_back(token("rcurly", "}"));
          max_token++;
          place++;
          continue;
        }
        if (first == '['){
          tokent = 8;
          tokens.push_back(token("llist", "["));
          max_token++;
          place++;
          continue;
        }
        if (first == ']'){
          tokent = 9;
          tokens.push_back(token("rlist", "]"));
          max_token++;
          place++;
          continue;
        }
        if (first == ','){
          tokent = 10;
          tokens.push_back(token("comma", ","));
          max_token++;
          place++;
          continue;
        }
        if (isoper(first)){
          if (tokent != 11){
            tokens.push_back(token("oper", std::string({first})));
            max_token++;
            tokent = 11;
          }
          else{
            tokens[max_token].value += first;
          }
          place++;
          continue;
        }
        break;
      }
      std::cout << code.substr(place) << std::endl;
      std::cout << "cannot lex file" << std::endl;
      std::cout << "line " << line << " looks off" << std::endl;
      exit(0);
      return {};
    }
  }
}
