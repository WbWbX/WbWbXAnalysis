#!/bin/sh

dirname=$1
addParameters=$2

####
#
#  additional parameters are passed to analyse. docu can be found in app_src/analyse.cc
#  specify syst, channels and energies to run on here (just (un)comment in arrays)
#
####


channels=( "ee"
    "emu"
    "mumu"
);
systs=("nominal"

    "TRIGGER_up"
    "TRIGGER_down"
    "ELECSF_up"
    "ELECSF_down"
    "MUONSF_up"
    "MUONSF_down"

    "ELECES_up"
    "ELECES_down"
    "MUONES_up"
    "MUONES_down"

    "PU_up"
    "PU_down"

   "JER_up"
   "JER_down"
   "JES_up"
   "JES_down"

# "BTAGH_up"
# "BTAGH_down"
# "BTAGL_up"
#"BTAGL_down"

   "TT_MATCH_down"
   "TT_MATCH_up"
   "TT_SCALE_down"
   "TT_SCALE_up"

   "MT_3_down"
   "MT_3_up"
   "MT_6_down"
   "MT_6_up"


#"Z_MATCH_down"
#"Z_MATCH_up"
#"Z_SCALE_down"
#"Z_SCALE_up"
);
energies=("8TeV"
#"7TeV"
);


#create new subdir (analysis_timestamp) , copy source there and make dir inside (output).
# add script there to get plots, merge etc as independent as possible from rest (will need container classes at least)


# define all syst here in an array
# qsub stuff
# check in background whether files are done (sleep 30)
# if all done go to dir (if not yet there) analysis_timestamp and merge stuff to have all syst there

# may want to have an additional script for pdf variations (to get all of them) needs level of communication between tree and this script...? how many variations..? or just do 40 and skip non existing, mark file in output and ignore in the following



dir=${dirname}_$(date +%F_%H:%M)__GI #ensure git ignore for these folders


echo "running in dir $dir"

analysisDir=$CMSSW_BASE/src/TtZAnalysis/Analysis/


cd $analysisDir
mkdir -p workdir
cd workdir
mkdir $dir
cd $dir
workdir=`pwd`
cp $analysisDir/submit.sh .
cp $analysisDir/bin/analyse .
cp -r $analysisDir/lib .
cp $analysisDir/*config.txt .
cp $analysisDir/*btags.root .

mkdir source
cd source 
cp $analysisDir/src/eventLoop.h .
cp $analysisDir/src/MainAnalyzer.cc .
cp $analysisDir/interface/MainAnalyzer.h .
cp $analysisDir/app_src/analyse.cc .

cd $workdir
mkdir jobscripts
mkdir stdout

BATCHDIR=$workdir/batch

if [[ $SGE_CELL ]] ;
then
    mkdir -p $BATCHDIR
fi


sed -e 's;##WORKDIR##;'${workdir}';g' < $analysisDir/check_temp.sh > check.sh
chmod +x check.sh

#check wheter running on naf or wgs and do qsub or dirty "&"
for (( i=0;i<${#channels[@]};i++)); do
    for (( j=0;j<${#systs[@]};j++)); do
	for (( k=0;k<${#energies[@]};k++)); do

	    channel=${channels[${i}]}
	    syst=${systs[${j}]}
	    energy=${energies[${k}]}

##here do qsub or dirty &
	    outname=${channel}_${energy}_${syst};
	   # array=( "${array[@]}" "jack" )
	    outnames=( "${outnames[@]}" "${outname}" ); 
	    sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst} -e ${energy} ${addParameters}/" -e "s;##WORKDIR##;${workdir};" < $analysisDir/job.sh > jobscripts/${outname}
	    chmod +x jobscripts/${outname}
	    if [[ $SGE_CELL ]] ;
	    then
		cd $BATCHDIR
		qsub $workdir/jobscripts/${outname}
		cd $workdir
	    else
		all=`ps ax | grep -E 'analyse' | wc -l`
		defunct=`ps ax | grep -E 'analyse' | grep -E 'defunct' | wc -l`
		running=`expr $all - $defunct`
		while [ $running  -gt 10 ]; do
		    sleep 2;
		    all=`ps ax | grep -E 'analyse' | wc -l`
		    defunct=`ps ax | grep -E 'analyse' | grep -E 'defunct' | wc -l`
		    running=`expr $all - $defunct`
		done
		echo "starting ${outname}"
		./jobscripts/${outname} &
		sleep 2;
	    fi

###make the merge line

###what about check script?

	done
    done
done
