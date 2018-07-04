namespace tach{
  namespace vm{
    namespace builtins{
      obj print(obj stack){
        obj ret;
        for (int i = 0; i < stack.size; i++){
          std::cout << i << std::endl;
        }
        return ret;
      }
      map_obj create(){
        map_obj ret;
        ret.names.append("print");
        ret.vals.append(print);
       return ret;
     }
    }
  }
}
