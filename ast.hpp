
std::string indent(std::string in){
  std::stringstream ss(in);
  std::string s, ret;
  while (getline(ss, s)){
    ret += " "+s+"\n";
  }
  return ret;

}



namespace tach{
  namespace tree{
    std::map<int, int>  pair(std::vector<std::string> x, std::vector<std::string> chs, int depth){
      std::map<int, int> hold;
      std::map<int, int> ret;
      int dep = 0;
      for (int pl = 0; pl < x.size(); pl++){
        std::string cur = x[pl];
        if (!cur.compare(chs[0])){
          dep ++;
          hold[dep] = pl;
        }
        else if (!cur.compare(chs[1])){
          if (depth == 0 || dep <= depth){
            if (dep < 0){
              std::cout << "ast could not pair " + chs[1] + " with " + chs[0] << std::endl;
              exit(0);
            }
            ret[hold[dep]] = pl;
          }
          dep --;
        }
      }
      if (dep > 0){
        std::cout << "ast could not pair " + chs[0] + " with " + chs[1] << std::endl;
        exit(0);
      }
      return ret;
    }
    std::map<int, int>  pair(std::vector<std::string> x, std::vector<std::string> chs){
      return pair(x, chs, 0);
    }
    struct node{
      std::vector<std::string> strs;
      std::vector<node> children;
      std::string type;
      node(){
        type = "node";
      }
      node(std::string t){
        type = t;
      }
      node(std::string t, std::vector<std::string> st){
        strs = st;
        type = t;
      }
      node(std::string t, std::vector<node> ch){
        children = ch;
        type = t;
      }
      node(std::string t, std::vector<std::string> st, std::vector<node> ch){
        children = ch;
        strs = st;
        type = t;
      }
      void append(node n){
        children.push_back(n);
      }
      void append(std::string s){
        strs.push_back(s);
      }
      int childs(){
        return children.size();
      }
      bool had_data(std::string ty, std::string dat){
        for (node n : children){
          if (!n.type.compare("token")){
            if (!n.strs[0].compare(ty) && !n.strs[1].compare(dat)){
              return true;
            }
          }
        }
        return false;
      }
      int last_index_of(std::string ty, std::string dat){
        for (int c = children.size()-1; c >= 0; c--){
          node n = children[c];
          if (!n.type.compare("token")){
            if (!n.strs[0].compare(ty) && !n.strs[1].compare(dat)){
              return c;
            }
          }
        }
        std::cout << "index does not exist" << std::endl;
        exit(0);
      }
      int index_of(std::string ty, std::string dat){
        for (int c = 0; c < children.size(); c++){
          node n = children[c];
          if (!n.type.compare("token")){
            if (!n.strs[0].compare(ty) && !n.strs[1].compare(dat)){
              return c;
            }
          }
        }
        std::cout << "index does not exist" << std::endl;
        exit(0);
      }
      bool contains_type(std::string ty){
        for (node n : children){
          if (!n.type.compare("token") && !n.strs[0].compare(ty)){
            return true;
          }
        }
        return false;
      }
      operator std::string(){
        std::string ret = "";
        for (std::string s: strs){
          ret += indent("string : \""+s+"\"\n");
        }
        for (node n: children){
          ret += indent(std::string(n));
        }
        return type+":\n"+indent(ret);
      }
    };
    node line(node in){
      if (in.childs() == 0){
        return node("empty");
      }
      if (in.childs() == 1){
        return in.children[0];
      }
      if (in.contains_type("oper")){
        std::vector<std::vector<std::string>> finds = {
            {"error"},
            {"."},
            {"**"},
            {"*", "/", "%"},
            {"+", "-"},
            {"<", ">", "<=", ">="},
            {"!=", "=="},
            {"||", "&&"},
            {"=", "+=", "-=", "*=", "/="},
        };
        std::string oper;
        bool ob = false;
        for (int i = finds.size()-1; i >= 0; i--){
          std::vector<std::string> order = finds[i];
          for (int oc = 0; oc < order.size(); oc++){
            oper = order[oc];
            if (in.had_data("oper", oper)){
              ob = true;
              break;
            }
          }
          if (ob){
            break;
          }
        }
        if (oper == "error"){
          std::cout << "unknown operator" << std::endl;
          exit(0);
        }
        std::vector<std::string> rtl_opers = {"=", "+=", "/=", "-=", "*="};
        bool is_ltr = svecin(rtl_opers, oper);
        int oper_ind = is_ltr ? in.index_of("oper", oper) : in.last_index_of("oper", oper);
        std::vector<node> pre;
        for (int i = 0; i < oper_ind; i++){
          pre.push_back(in.children[i]);
        }
        std::vector<node> post;
        for (int i = oper_ind+1; i < in.childs(); i++){
          post.push_back(in.children[i]);
        }
        node after = node("expr", {}, post);
        node before = node("expr", {}, pre);
        before = line(before);
        after = line(after);
        return node("oper", {oper}, {before, after});
      }
      node last_node = in.children[in.childs()-1];
      if (last_node.type == "tuple"){
        node perams = in.children[in.children.size()-1];
        perams.type = "perams";
        in.children.pop_back();
        node fn = line(in);
        node ret = node("function", {}, {fn, perams.children[0]});
        return ret;
      }
      else if (last_node.type == "list"){
        // std::cout << std::string(in.children[in.children.size()-1]) << std::endl;
        node sli = in.children[in.children.size()-1];
        sli.type = "expr";
        in.children.pop_back();
        node fn = line(in);
        node ret = node("get", {}, {fn, sli});
        return ret;
      }
      std::cout << "something went wrong in the ast" << std::endl;
      exit(0);
    }
    node tree(std::vector<lex::token> tokens){
      std::vector<std::string> types;
      std::vector<std::string> values;
      for (lex::token t: tokens){
        types.push_back(t.type);
        values.push_back(t.value);
      }
      std::map<std::string, std::map<int, int>> pairs = {
        {"paren", pair(values, {"(", ")"})},
        {"curly", pair(values, {"{", "}"})},
        {"square", pair(values, {"[", "]"})},
      };
      node ret = node("block", {}, {node("expr")});
      for (int pl = 0; pl < tokens.size(); pl++){
        if (pairs["paren"].count(pl) != 0){
          int jmp = pairs["paren"][pl];
          std::vector<lex::token> subtokens;
          for (int i = pl+1; i < jmp; i++){
            subtokens.push_back(tokens[i]);
          }
          node newnode("tuple", {}, {tree(subtokens)});
          ret.children[ret.childs()-1].append(newnode);
          pl = jmp;
        }
        else if (pairs["square"].count(pl) != 0){
          int jmp = pairs["square"][pl];
          std::vector<lex::token> subtokens;
          for (int i = pl+1; i < jmp; i++){
            subtokens.push_back(tokens[i]);
          }
          node newnode("list", {}, {tree(subtokens)});
          ret.children[ret.childs()-1].append(newnode);
          pl = jmp;
        }
        else if (pairs["curly"].count(pl) != 0){
          int jmp = pairs["curly"][pl];
          std::vector<lex::token> subtokens;
          for (int i = pl+1; i < jmp; i++){
            subtokens.push_back(tokens[i]);
          }
          node newnode("tuple", {}, {tree(subtokens)});
          ret.children[ret.childs()-1].append(newnode);
          pl = jmp;
        }
        else if (svecin({"newline", "comma"}, types[pl])){
          if (ret.children[ret.childs()-1].childs() != 0){
            ret.append(node("expr"));
          }
        }
        else {
          node newnode("token", {types[pl], values[pl]}, {});
          ret.children[ret.childs()-1].append(newnode);
        }
      }
      node aline;
      for (node n: ret.children){
        aline.append(line(n));
      }
      return aline;
    }
  }
}
