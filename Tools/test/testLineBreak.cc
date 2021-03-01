

/*
 * Executable created with "createExecutableSkeleton"
 *
 * An option parser is automatically invoked.
 *    It supports any option as -o or -option or --option
 *    Multiple options (-a -b= -ab) are not foreseen and allowed (at the moment)
 *    The option -h and --help are reserved to display an automatically generated
 *    help text
 */

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include <iostream>
/*
 *  Test line break capabilities of textFormatter
 */
invokeApplication(){
    using namespace ztop;

    //
    /* code begins */
    std::string s="ifanextremelylongwordusesmuchmorethanonelineandthereforeneedstobesplitintomultiplelines. this is a long string, that should be split into multiple lines, but only at white spaces or within the word \
but afterwards the text \
continues as if nothing  happened and even more lines are added making it really generic in terms of an example text. I am out of \
ideas but this should suffice. ifanextremelylongwordusesmuchmorethanonelineandthereforeneedstobesplitintomultiplelines" ;
    
    size_t maxchars=80;
    size_t offset=10;
    
    textFormatter::debug=true;
    std::string split=textFormatter::splitIntoLines(s,maxchars,offset,1);

    std::cout << std::string(maxchars+offset, '_') <<std::endl;
    
    std::cout << "\n\nresult:\n" << split <<std::endl;
    
    
    
    /* code ends */
    //this function returns an int to the operating system
    return EXIT_SUCCESS;
}

