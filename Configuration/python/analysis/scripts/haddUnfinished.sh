#!/bin/zsh

if [ $1 ]; then

    echo "merging all in dir ${1}"

else
    echo "you have to specify a dirname"
    exit
fi

ALLINDIR=$1
TREENAME=`echo $ALLINDIR | cut -c5-`.root

echo "outfile: ${ALLINDIR}/${TREENAME}"


if [ -f $ALLINDIR/${TREENAME} ]; then;
    echo "File $ALLINDIR/${TREENAME} already exists -> skip";
    exit
fi

if ls $ALLINDIR/*.root &> /dev/null; then
    echo hadding $ALLINDIR
    hadd $ALLINDIR/${TREENAME} $ALLINDIR/*.root
    echo DONE
else
#if [ ! -f $ALLINDIR/*.root ]; then
    echo "File $ALLINDIR/${TREENAME} has no *.root files in it -> skip";
    exit
fi


