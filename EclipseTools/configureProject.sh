#!/bin/bash

# check for project

if [ ! -d $CMSSW_BASE/eclipse/.metadata ] || [ ! -f $CMSSW_BASE/src/.cproject ]
then
    echo "first set up a basic eclipse project using $CMSSW_BASE/runEclipse.sh and follow the instruction in README/twiki"
fi

## attach CMSSW specific things to the project file
## add includes


cd $CMSSW_BASE/src/TtZAnalysis/EclipseTools


#### build within eclipse

function escape_slashes {
    sed 's/\//\\\//g' 
}
function escape_ws {
    sed 's/\"/\\\"/g' 
}

function change_line {
    local OLD_LINE_PATTERN=$1; shift
    local NEW_LINE=$1; shift
    local INFILE=$1; shift
    local OUTFILE=$1; 

    local NEW=$(echo "${NEW_LINE}" | escape_slashes)
    sed 's/'"${OLD_LINE_PATTERN}"'.*/'"${NEW[@]}"'/' "${INFILE}" > $OUTFILE
}
function append_after_line {
     local OLD_LINE_PATTERN=$1; shift
    local NEW_LINE="${1}"; shift
    local INFILE=$1; shift
    local OUTFILE=$1;
    OLD_LINE_PATTERN=$(echo "${OLD_LINE_PATTERN}" | escape_slashes )
    local NEW=$(echo "${NEW_LINE}" | escape_slashes )
    local pattern="/${OLD_LINE_PATTERN}.*/ a\
    ${NEW}";
    sed ''"${pattern}"'' "${INFILE}" > $OUTFILE
}

#replace
# <builder buildPath="${workspace_loc:/testProj/Release}" id="cdt.managedbuild.target.gnu.builder.exe.release.1873982263" managedBuildOn="true" name="Gnu Make Builder.Release" superClass="cdt.managedbuild.target.gnu.builder.exe.release"/>
# with
# <builder arguments="b -j12" autoBuildTarget="all" buildPath="${workspace_loc:/testProj}" cleanBuildTarget="clean" command="scram" enableAutoBuild="false" enableCleanBuild="true" enabledIncrementalBuild="true" id="cdt.managedbuild.target.gnu.builder.exe.release.1873982263" incrementalBuildTarget=" " keepEnvironmentInBuildfile="false" managedBuildOn="false" name="Gnu Make Builder" parallelizationNumber="1" superClass="cdt.managedbuild.target.gnu.builder.exe.release"/>
REPLACEWITH=$( cat $CMSSW_BASE/src/TtZAnalysis/EclipseTools/buildconfigdiff | escape_ws )

change_line "<builder buildPath=" "${REPLACEWITH}" $CMSSW_BASE/src/.cproject .tmp
ADDINCLUDES=$( cat $CMSSW_BASE/src/TtZAnalysis/EclipseTools/includeconfigdiff | escape_ws )

append_after_line "name=\"GCC C++ Compiler\"" "${ADDINCLUDES}\n" .tmp .tmp2
rm -f .tmp
append_after_line "${ADDINCLUDES}" "<listOptionValue builtIn=\"false\" value=\"${CMSSW_RELEASE_BASE}/src\"/>\n<listOptionValue builtIn=\"false\" value=\"${ROOTSYS}/include\"/>\n</option>" .tmp2 .tmp3 
cp $CMSSW_BASE/src/.cproject $CMSSW_BASE/src/.cproject.bak
mv .tmp3 $CMSSW_BASE/src/.cproject
rm .tmp2
echo "eclipse project ready to be used"

