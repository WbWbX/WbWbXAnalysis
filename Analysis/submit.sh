#!/bin/sh


channels=( #"ee"
    "emu"
    #"mumu"
);
## scale and match variations will be ignored for other top masses then 172.5.. (hardcoded below)
topmasses=( "172.5"

    #"171.5"
    #"173.5"

    #"175.5"
    #"169.5"

    #"178.5"
    #"166.5"
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

   "BTAGH_up"
   "BTAGH_down"
   "BTAGL_up"
   "BTAGL_down"
   #"BTAGHFS1_up"
   #"BTAGHFS1_down"
   #"BTAGHFS2_up"
   #"BTAGHFS2_down"
   #"BTAGLFS1_up"
   #"BTAGLFS1_down"
   #"BTAGLFS2_up"
   #"BTAGLFS2_down"
   #"BTAGC1_up"
   #"BTAGC1_down"
   #"BTAGC2_up"
   #"BTAGC2_down"


   "TOPPT_up"
   "TOPPT_down"

   "TT_MATCH_down"
   "TT_MATCH_up"
   "TT_SCALE_down"
   "TT_SCALE_up"


   #"Z_MATCH_down"
   #"Z_MATCH_up"
   #"Z_SCALE_down"
   #"Z_SCALE_up"
);
energies=("8TeV"
#"7TeV"
);


#creates new subdir (analysis_timestamp) , copy source there and make dir inside (output).
# adds script there to get plots, merge etc as independent as possible from rest (will need container classes at least)


# defined all syst here in an array
# qsub stuff
# check in background whether files are done (sleep 150) --> switch: last option c
# check merges automatically if all succ done

# may want to have an additional script for pdf variations (to get all of them) needs level of communication between tree and this script...? how many variations..? or just do 40 and skip non existing, mark file in output and ignore in the following

dirname=$1
addParameters=$2
# if $3 == c, a check will be performed every 150 sec

####
#
#  additional parameters are passed to analyse. docu can be found in app_src/analyse.cc
#  specify syst, channels and energies to run on here (just (un)comment in arrays)
#
####



dir=$(date +"%Y%m%d_%H%M")_${dirname} 


echo "running in dir $dir"

analysisDir=$CMSSW_BASE/src/TtZAnalysis/Analysis/


cd $analysisDir
#mkdir -p workdir
mkdir -p /nfs/dust/cms/user/$USER/AnalysisWorkdir
if [[ ! -L "workdir" ]]
then
    ln -s /nfs/dust/cms/user/$USER/AnalysisWorkdir workdir 
fi
cd workdir
mkdir $dir
if [[ -L "last" ]]
then
rm last
fi
ln -s $dir last
cd $dir
workdir=`pwd`
cp $analysisDir/submit.sh .
cp $analysisDir/bin/analyse .
cp -r $analysisDir/lib .
cp $analysisDir/*config.txt .

cp $analysisDir/*btags.root .
cp $analysisDir/*discr.root .

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
    for (( l=0;l<${#topmasses[@]};l++)); do
	for (( j=0;j<${#systs[@]};j++)); do
	    for (( k=0;k<${#energies[@]};k++)); do

		channel=${channels[${i}]}
		topmass=${topmasses[${l}]}
		syst=${systs[${j}]}
		energy=${energies[${k}]}

##dont run on scale and matching syst with other top mass
		if [[ "${syst}" == *"SCALE"* ]] ||  [[ "${syst}" == *"MATCH"* ]]
		then
		    if [[ "${topmass}" != "172.5" ]] 
		    then
			continue
		    fi
		fi

##here do qsub or dirty &
		outname=${channel}_${energy}_${topmass}_${syst};
	   # array=( "${array[@]}" "jack" )
		outnames=( "${outnames[@]}" "${outname}" ); 
		sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst} -e ${energy} -mt ${topmass} ${addParameters}/" -e "s;##WORKDIR##;${workdir};" < $analysisDir/job.sh > jobscripts/${outname}
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
		    sleep 10;
		fi

###make the merge line

###what about check script?
	    done
	done
    done
done

function contcheck(){

$workdir/check.sh
while [ $? -ne 0 ]; 
do
    sleep 150
    echo
    echo time: $(date +"%H:%M")
    $workdir/check.sh
done

}

## continuous checking

if [[ $3 == "c" ]]
then
    echo "swiching to continuous checking mode"
    contcheck


else
    echo "to check the status run ${workdir}/check.sh"
fi
