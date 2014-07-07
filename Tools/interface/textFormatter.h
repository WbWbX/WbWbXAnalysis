/*
 * textFormatter.h
 *
 *  Created on: May 6, 2014
 *      Author: kiesej
 */

#ifndef TEXTFORMATTER_H_
#define TEXTFORMATTER_H_

#include <string>
#include <vector>

namespace ztop{

class textFormatter {

public:

    textFormatter() :trim_(" \t"),comment_("#"),delimiter_(","){}

    /**
     * sets all chars that are used for trimming
     * can be more than one, e.g. "\t\n "
     */
    void setTrim(const std::string& tr){trim_=tr;}
    /**
     * defines one char that will serve as comment indicator.
     * all text following that char will be ignored until the next line starts
     */
    void setComment(const std::string& c){comment_=c;}
    /**
     * sets a delimiter for individual entries per line (e.g. "," or " ")
     */
    void setDelimiter(const std::string& d){delimiter_=d;}

    std::string & ltrim(std::string & str) const;
    std::string & rtrim(std::string & str) const;
    std::string & trim(std::string & str) const;
    std::string & trimcomments(std::string & str) const;


    /**
     * returns formatted text fragments according to the farmet rules defined previously
     */
    std::vector<std::string> getFormatted(const std::string& in)const;

    /**
     * returns file name with extension but without path
     */
    std::string getFilename(const std::string& pathtofile)const;
    /**
     * returns file extension or empty string if none
     */
    std::string getFileExtension(const std::string& pathtofile)const;
    /**
     * strips file of path and extension
     */
    std::string stripFileExtension(const std::string& pathtofile)const;
    /**
     * gets file directory
     */
    std::string getFileDir(const std::string& pathtofile)const;

    /**
     * switch for more output
     */
    static bool debug;

protected:
    std::string trim_,comment_,delimiter_;


};

}




#endif /* TEXTFORMATTER_H_ */
