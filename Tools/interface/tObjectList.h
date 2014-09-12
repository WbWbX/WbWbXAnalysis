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
#include <stdexcept>

namespace ztop{
/**
 * class to handle memory
 * should be used as base class for everything that deals with
 * tobjects on the heap
 */
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
    /**
     * Throws if object is 0
     */
    template<class obj>
    obj * addObject( obj *o){
        if(!o){
            throw std::runtime_error("tObjectList::addObject: trying to add NULL");
        }
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
