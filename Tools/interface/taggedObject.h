/*
 * taggedObject.h
 *
 *  Created on: Mar 26, 2014
 *      Author: kiesej
 */

#ifndef TAGGEDOBJECT_H_
#define TAGGEDOBJECT_H_

#include <vector>

namespace ztop{

/**
 * a class to add tags to objects.
 * Just inherit from it.
 * If you add a tag enum, please call it <yourtag>_tag and
 * add it to the end of the list to ensure backward compatibility
 */

class taggedObject{
public:
    taggedObject():sorted_(false){}
    ~taggedObject(){}
    /*
     * If you need to add a tag to any object, use these here
     * If tag you need doesn't exist, yet, add it add the very end
     */
    enum tags{
        isLHDiscr_tag,
        dontDivByBW_tag,
        dontAddSyst_tag
    };


    ////////access functions



    void addTag(const ztop::taggedObject::tags &tag);
    void removeTag(const ztop::taggedObject::tags &tag);

    void addTagsFrom(const taggedObject*);

    void clearTags(){tags_.clear();}

    bool hasTag(const ztop::taggedObject::tags& tag)const;
    bool hasTag()const;

    bool tagsAreEqual(const taggedObject*)const;

private:

    std::vector<ztop::taggedObject::tags> tags_;
    bool sorted_;

};

}



#endif /* TAGGEDOBJECT_H_ */
