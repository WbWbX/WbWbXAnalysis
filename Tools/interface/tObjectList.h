/*
 * tObjectList.h
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#ifndef TOBJECTLIST_H_
#define TOBJECTLIST_H_
#include "TObject.h"
#include <vector>
#include "TH1.h"

namespace ztop{
class tObjectList{
public:
    tObjectList(){TH1::AddDirectory(false);}
    ~tObjectList(){
        cleanMem();
    }
    tObjectList(const tObjectList& rhs){
        if(&rhs == this)
            return;
        cleanMem();
    }
    tObjectList & operator = (const tObjectList& rhs){
        if(&rhs == this)
            return *this;
        cleanMem();
        return *this;
    }


    void cleanMem(){
        for(size_t i=0;i<allobj_.size();i++){
            if(allobj_.at(i)){
                delete allobj_.at(i);
            }
        }
        allobj_.clear();
    }
    size_t size()const{return allobj_.size();}

protected:
    template<class obj>
    obj * addObject( obj *o){
        TObject *tobj=o;
        tobj->SetBit(kMustCleanup);
        allobj_.push_back(tobj);
        return o;
    }


private:
    std::vector<TObject *> allobj_;
};

}



#endif /* TOBJECTLIST_H_ */
