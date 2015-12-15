#!/bin/zsh


if [ ! $1 ]
then
echo "USAGE: createExecutableSkeleton <output name>"
fi


#rm $1
cat >> $1 << EOF


/*
 * Executable created with "createExecutableSkeleton"
 *
 * An option parser is automatically invoked.
 *    It supports any option as -o or -option or --option
 *    Multiple options (-a -b= -ab) are not foreseen and allowed (at the moment)
 *    The option -h and --help are reserved to display an automatically generated
 *    help text
 */

#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"

/*
 *  Document the purpose shortly
 */
invokeApplication(){
    using namespace ztop;
    parser->setAdditionalDesciption("\\
      Description of purpose and usage of this executable.\n \\
      This will be displayed if the help option is called (-h --help)\n");
    /* options begin */
    
    //example
    const bool option = parser->getOpt<bool>("b",false,"Description of the option (will be displayed in help). default: false");
    
    
    /* options end */
    
    // retrieve everything that is not an option (e.g. input files)
    std::vector<std::string> tmpv=parser->getRest<std::string>();
    //will display help and exit if help option is called
    parser->doneParsing();
    
    /* parsing ends */
    /* code begins */
    
    
    
    
    
    
    
    /* code ends */
    //this function returns an int to the operating system
    return EXIT_SUCCESS;
}

EOF