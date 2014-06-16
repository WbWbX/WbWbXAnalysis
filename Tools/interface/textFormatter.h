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

    void setTrim(const std::string& tr){trim_=tr;}
    void setComment(const std::string& c){comment_=c;}
    void setDelimiter(const std::string& d){delimiter_=d;}

    std::string & ltrim(std::string & str) const;
    std::string & rtrim(std::string & str) const;
    std::string & trim(std::string & str) const;
    std::string & trimcomments(std::string & str) const;



    std::vector<std::string> getFormatted(const std::string& in)const;


    std::string getFilename(const std::string& pathtofile)const;

    static bool debug;

protected:
    std::string trim_,comment_,delimiter_;


};

}




#endif /* TEXTFORMATTER_H_ */
