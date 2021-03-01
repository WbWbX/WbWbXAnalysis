#!/bin/zsh

if [[ $CMSSW_VERSION == "" ]]
then
echo "Must be run in CMSSW environment"
exit -2
fi
if [ ! -d /nfs/dust/cms/user/$USER ]
then
echo "Must be run on the naf (with sonas access)"
exit -1
fi


mkdir -p /nfs/dust/cms/user/$USER/cmssw_full_tarballs

targetfile=/nfs/dust/cms/user/$USER/cmssw_full_tarballs/$CMSSW_VERSION.tar
excludefile=$CMSSW_BASE/src/WbWbXAnalysis/EclipseTools/.advanced_exclude_fromtar.txt 

if [ -e $targetfile ]
then
echo "${targetfile} already exists, exit."
exit
fi

cd $CMSSW_RELEASE_BASE
cd ..

tar -X $excludefile -chf $targetfile $CMSSW_VERSION

echo "Tarball for ${CMSSW_VERSION} created at ${targetfile}"