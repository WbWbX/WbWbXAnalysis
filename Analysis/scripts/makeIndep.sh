#!/bin/sh



workdir=`pwd`
executable=$1
option=$2
if [ $option ]
then
    if [[ "${option}" != "all" ]]
    then
       echo "unrecognized option \"${option}\" "
       echo "available option: all = copy all CMSSW libs instead of TtZAnalysis and TopAnalysis only"
       exit 1
    fi
fi

execpath=`which $1`

BINDIR=$workdir/indep_$executable/bin/
LOCLIB=$workdir/indep_$executable/lib/

mkdir -p $BINDIR
mkdir -p $LOCLIB

cp $execpath $BINDIR/


CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
EXTLIBS=${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/

function copylibs(){

    linklibs=""
    libdir=$LOCLIB
    mkdir -p $libdir
    if [[ "${option}" == "all" ]]
    then
       cp ${CMSLIBS}lib*.so $libdir
    else
       cp ${CMSLIBS}lib*TtZAnalysis*.so $libdir
       cp ${CMSLIBS}lib*TopAnalysis*.so $libdir
    fi

    cp $EXTLIBS/libunfold.so $libdir 

}

copylibs


# create script to run:
outscript=run_indep_$1
touch $outscript
chmod +x $outscript


cat > $outscript << EOT

#!/bin/sh


export LD_LIBRARY_PATH=$LOCLIB:$LD_LIBRARY_PATH
$BINDIR/$executable "\$@"

EOT


echo "created independent script ${outscript} "
echo "with own TtZAnalysis libraries and executable in" 
echo "${workdir}/indep_${executable}"
echo "script can be run while compiling/testing other code"
