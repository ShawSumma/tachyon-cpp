namespace tach{
  namespace vm{
    struct obj;
    struct map_obj;
  }
}
#include "builtins.cpp"
namespace tach{
  namespace vm{
    using namespace tree;
    struct vm_inst;
    struct obj{
      std::vector<int> ints;
      std::vector<std::string> strs;
      std::vector<std::function<obj(obj)>> funs;
      std::vector<obj> objs;
      std::vector<int> tys;
      std::vector<int> places;
      int size = 0;
      obj(){}
      void append(int x){
        places.push_back(ints.size());
        ints.push_back(x);
        tys.push_back(0);
        size++;
      }
      void append(std::string x){
        places.push_back(strs.size());
        strs.push_back(x);
        tys.push_back(1);
        size++;
      }
      void append(std::function<obj(obj)> x){
        places.push_back(funs.size());
        funs.push_back(x);
        tys.push_back(2);
        size++;
      }
      void append(obj x){
        places.push_back(objs.size());
        objs.push_back(x);
        tys.push_back(3);
        size++;
      }
      void pop(){
        size--;
        switch ((tys[size])){
          case 0:
            ints.pop_back();
            return;
          case 1:
            strs.pop_back();
            return;
          case 2:
            funs.pop_back();
            return;
          case 3:
            objs.pop_back();
            return;
        }
      }
      void build_list(int c){
        obj r;
        for (int i = 0; i < c; i++){
          switch ((tys[--size])){
            case 0:
              r.append(ints[size]);
              break;
            case 1:
              r.append(strs[size]);
              break;
            case 2:
              r.append(funs[size]);
              break;
            case 3:
              r.append(objs[size]);
              break;
          }
        }
        append(r);
      }
      int int_at(int p){
        if (tys[p] == 0){
          return ints[places[p]];
        }
        std::cerr << "not an integer at " << p << std::endl;
        exit(1);
      }
      std::string str_at(int p){
        if (tys[p] == 1){
          return strs[places[p]];
        }
        std::cerr << "not a string at " << p << std::endl;
        exit(1);
      }
      std::function<obj(obj)> fun_at(int p){
        if (tys[p] == 2){
          return funs[places[p]];
        }
        std::cerr << "not a function at " << p << std::endl;
        exit(1);
      }
      obj obj_at(int p){
        if (tys[p] == 3){
          return objs[places[p]];
        }
        std::cerr << "not an list at " << p << std::endl;
        exit(1);
      }
      obj push_to(obj n, int index){
        switch (tys[index]){
          case 0:
            n.append(ints[index]);
            return n;
          case 1:
            n.append(strs[index]);
            return n;
          case 2:
            n.append(funs[index]);
            return n;
          case 3:
            n.append(objs[index]);
            return n;
        }
        std::cerr << "unknow push_to type " << tys[index] << std::endl;
        exit(1);
      }
      void call(int c){
        size--;
        if (tys[size] == 2){
          build_list(c);
          funs[size](objs[size-1]);
        }
        std::cerr << "can only call fucntion" << std::endl;
        exit(1);
      }
      bool operator ==(obj other){
        if (other.size != size){
          return false;
        }
        for (int i = 0; i < size; i++){
          if (tys[i] != other.tys[i]){
            return false;
          }
        }
        for (int i = 0; i < size; i++){
          if (places[i] != other.places[i]){
            return false;
          }
        }
        return true;
      }
    };
    struct map_obj{
      obj names;
      obj vals;
      map_obj(){};
      obj add_at(int ind, obj n){
        for (int i = 0; i < vals.size; i++){
          if(names.tys[i] == 0 && names.int_at(i) == ind){
            return names.push_to(n, i);
          }
        }
        std::cerr << "could not find index " << ind << std::endl;
        exit(1);
      }
      // obj add_at(string ind, obj n){
      //   for (int i = 0; i < vals.size; i++){
      //     if(names.tys[i] == 1 && names.str_at(i) == ind){
      //       return names.push_to(n, i);
      //     }
      //   }
      //   std::cerr << "could not find index " << ind << std::endl;
      //   exit(1);
      // }
      // obj add_at(function<obj(obj)> ind, obj n){
      //   for (int i = 0; i < vals.size; i++){
      //     function<obj(obj)> f = names.fun_at(i);
      //     if(names.tys[i] == 2 && &f == &ind){
      //       return names.push_to(n, i);
      //     }
      //   }
      //   std::cerr << "could not find index <function " << &ind << ">" << std::endl;
      //   exit(1);
      // }
      // obj add_at(obj ind, obj n){
      //   for (int i = 0; i < vals.size; i++){
      //     if(names.tys[i] == 3 && names.obj_at(i) == ind){
      //       return names.push_to(n, i);
      //     }
      //   }
      //   std::cerr << "could not find index <object " << &ind << ">" << std::endl;
      //   exit(1);
      // }
    };
    std::map<std::string, int> operc = {
      {"+", 1},
      {"-", 2},
      {"*", 3},
      {"/", 4},
      {"<", 5},
      {">", 6},
      {"<=", 7},
      {">=", 8},
      {"!=", 9},
      {"==", 10},
      {"||", 11},
      {"&&", 12},
    };
    struct vm_inst{
      std::vector<int> bytecode;
      obj consts;
      map_obj builtins;
      std::map<int, std::string> opnames = {
          {1, "binary operator"},
          {2, "push const"},
          {3, "load const"},
          {4, "call"},
          {5, "get emlemet"},
          {6, "store"},
      };
      std::map<std::string, map<std::string, obj>> between;
      void exec(int from, int to, string name){
        map_obj locals;
        for (int i = from; i < bytecode.size(); i += 2){
          switch (byteocde[i]){
            // run the opcodes
          }
        }
      }
      void exec(){
        exec(0, bytecode.size(), "main");
      }
      int full_compile(node part){
        // std::cout << string(part) << std::endl;
        int sz = bytecode.size();
        if (part.type == "expr" || part.type ==  "peram" || part.type == "node"){
          for (node n: part.children){
            compile(n);
          }
          return sz-bytecode.size();
        }
        if (part.type == "oper"){
          if (part.strs[0] == "="){
            compile(part.children[1]);
            bytecode.push_back(6);
            bytecode.push_back(0);
          }
          else{
            compile(part.children[1]);
            compile(part.children[0]);
            bytecode.push_back(1);
            bytecode.push_back(operc[part.strs[0]]);
          }
          return sz-bytecode.size();
        }
        if (part.type == "token"){
          std::string name = part.strs[0];
          if (name == "int"){
            consts.append(stoi(part.strs[1]));
          }
          else if (name == "name"){
            consts.append(part.strs[1]);
            bytecode.push_back(3);
            bytecode.push_back(consts.size-1);
          }
          else if (name == "string"){
            consts.append(part.strs[1]);
          }
          else{
            std::cerr << "unknow token " << name << std::endl;
            exit(1);
          }
          bytecode.push_back(2);
          bytecode.push_back(consts.size-1);
          return sz-bytecode.size();
        }
        if (part.type == "function"){
          int size = 0;
          compile(part.children[0]);
          for (node n : part.children[1].children){
            if (n.type != "empty"){
              compile(n);
              size++;
            }
          }
          bytecode.push_back(4);
          bytecode.push_back(size);
          return sz-bytecode.size();
        }
        if (part.type == "get"){
          int size = 0;
          compile(part.children[0]);
          compile(part.children[1].children[0]);
          bytecode.push_back(5);
          bytecode.push_back(0);
          return sz-bytecode.size();
        }
        if (part.type == "empty"){
          return 0;
        }
        std::cerr << "cant compile ast emlemet " + part.type + "\n";
        exit(0);
      }
      int compile(node n){
        int s = -full_compile(n);
        return s;
      }
      vm_inst(node part){
        builtins = builtins::create();
        compile(part);
        for (int i = 0; i < bytecode.size(); i += 2){
          std::string opname = opnames[bytecode[i]];
          std::string spaces = "                    ";
          spaces = spaces.substr(opname.size());
          std::cout << opname << spaces << bytecode[i] << " " << bytecode[i+1] << std::endl;
        }
        exec();
      }
    };
  }
}
#include "funcs.cpp"
