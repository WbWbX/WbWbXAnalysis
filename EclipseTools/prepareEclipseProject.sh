#!/bin/bash

if [ ! "$CMSSW_BASE" ]
then
echo "needs to be invoked from a CMSSW environment"
exit 1
fi

if [ ! $1 ]
then
echo "please specify your LOCAL! c++ include path (e.g. /usr/include/c++/4.2.1)"
exit 1
fi

templatesdir=$CMSSW_BASE/src/TtZAnalysis/EclipseTools/

echo "Eclipse will use a copy of the CMSSW release that needs to be located at ~/CMSSW/ on your local machine!"
echo "If it does not exist, please create it, otherwise come parts of the completion will not work"


allpaths="\${CWD}:~/CMSSW/$CMSSW_VERSION/src:${1}:\${ROOTSYS}/include"

cd $CMSSW_BASE

array=(${allpaths//:/ })


for i in "${!array[@]}"
do
    echo "adding include path: ${array[i]}"
   # sedcommands=( "${sedcommands[@]}" "<listOptionValue builtIn=false value=&quot;${array[i]}&quot;/> " );
 sedcommands=( "${sedcommands[@]}" "-e s%<!--listOptionValue%<listOptionValue% -e s%REPL-->%>% -e s%##INCLUDEPATH##%${array[i]}%" );
done
echo
#echo ${sedcommands[@]}

echo 
echo
sed ${sedcommands[@]} < $templatesdir.cproject > .cproject_temp
cp $templatesdir.project ./.project_temp
sed 's;##CMSSW_BASE##;'$CMSSW_BASE';g' < ${templatesdir}importEclipseProject.sh > importEclipseProject.sh

chmod +x importEclipseProject.sh

echo "run importEclipseProject.sh from your laptop with eclipse installed (via afs/sshfs...)"
echo "it will create a totally new workspace and import the project to it"

