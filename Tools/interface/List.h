#ifndef List_h
#define List_h

#include <vector>

namespace top{

template <class T>
  class List{    //initially just for containers, but might be used otherwise..
  public: 
    List(){list_.clear();};
    ~List(){};
    void push_back(T * cont){list_.push_back(cont);}
    typename std::vector<T *> getList(){return list_;}
    
  private:
    typename std::vector<T *> list_;
    

  };

}
#endif
