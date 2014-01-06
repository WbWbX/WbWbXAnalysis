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

namespace ztop{
class tObjectList{
public:


    void cleanMem(){
        for(size_t i=0;i<allobj_.size();i++){
            if(allobj_.at(i)) delete allobj_.at(i);
        }
    }

protected:
    template<class obj>
    obj * addObject(obj *o){
        TObject *tobj=o;
        allobj_.push_back(tobj);
        return o;
    }


private:
    std::vector<TObject *> allobj_;
};

}



#endif /* TOBJECTLIST_H_ */
