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
pdfeigenvectors=26  # 26 # if set to 0, no variation

systs=("nominal"		

    "P11_sysnominal"
    "P11_sysnominal_CR_up"
    "P11_sysnominal_CR_down"
    "P11_sysnominal_UE_up"
    "P11_sysnominal_UE_down"

   "TT_GENPOWPY_up"
   "TT_GENPOWPY_down"


   # "TT_GENPOWHERW_up"
   # "TT_GENPOWHERW_down"


   #  "TT_GENMCATNLO_up"
   #  "TT_GENMCATNLO_down"

    "TT_BJESNUDEC_down"
    "TT_BJESNUDEC_up"

    "TT_BJESRETUNE_down"
    "TT_BJESRETUNE_up"



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

   # "JES_up"
   # "JES_down"

#######JES groups according to toplhcwg use INSTEAD of global JES_up/down

    "JES_AbsoluteMPFBias_up"   

    "JES_FlavorPureGluon_up"
    "JES_FlavorPureQuark_up"
    "JES_FlavorPureCharm_up"
    "JES_FlavorPureBottom_up"

    "JES_RelativeFSR_up"

    "JES_AbsoluteStat_up"
    "JES_AbsoluteScale_up"
    "JES_HighPtExtra_up"
    "JES_SinglePionECAL_up"
    "JES_SinglePionHCAL_up"
    "JES_Time_up"
    "JES_RelativeJEREC1_up"
    "JES_RelativeJEREC2_up"
    "JES_RelativeJERHF_up"
     "JES_RelativePtBB_up"
    "JES_RelativePtEC1_up"
    "JES_RelativePtEC2_up"
    "JES_RelativePtHF_up"
    "JES_RelativeStatEC2_up"
    "JES_RelativeStatHF_up"
    "JES_PileUpDataMC_up"
    "JES_PileUpBias_up"

    "JES_PileUpPtBB_up"
    "JES_PileUpPtEC_up"
    "JES_PileUpPtHF_up"


##down

      "JES_AbsoluteMPFBias_down"	

    "JES_FlavorPureGluon_down"
    "JES_FlavorPureQuark_down"
    "JES_FlavorPureCharm_down"
    "JES_FlavorPureBottom_down"

    "JES_RelativeFSR_down"

    "JES_AbsoluteStat_down"
    "JES_AbsoluteScale_down"
    "JES_HighPtExtra_down"
    "JES_SinglePionECAL_down"
    "JES_SinglePionHCAL_down"
    "JES_Time_down"
    "JES_RelativeJEREC1_down"
    "JES_RelativeJEREC2_down"
    "JES_RelativeJERHF_down"
    "JES_RelativePtBB_down"
    "JES_RelativePtEC1_down"
    "JES_RelativePtEC2_down"
    "JES_RelativePtHF_down"
    "JES_RelativeStatEC2_down"
    "JES_RelativeStatHF_down"
    "JES_PileUpDataMC_down"
    "JES_PileUpBias_down"

    "JES_PileUpPtBB_down"
    "JES_PileUpPtEC_down"
    "JES_PileUpPtHF_down"


   # "JEC_residuals_up"
   # "JEC_residuals_down"

########


    "BTAGH_up"
    "BTAGH_down"
    "BTAGL_up"
    "BTAGL_down"
    
#####csv rew section
   #"BTAGHFS1_up"
   #"BTAGHFS1_down"
   #"BTAGHFS2_up"
   #"BTAGHFS2_down"
   #"BTAGLFS1_up"
   #"BTAGLFS1_down"
   #"BTAGLFS2_up"
   #"BTAGLFS2_down"
   #"BTAGPUR_up"
   #"BTAGPUR_down"


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
#systs=("nominal")

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




########add pdf variations#######

if [[ $pdfeigenvectors != "0" ]] && [[ $pdfeigenvectors ]]
then
    systs=( "${systs[@]}" "PDF_sysnominal" );
    for ((i = 1; i <= $pdfeigenvectors; i++));
    do
	systs=( "${systs[@]}" "PDF_sysnominal_PDF${i}_down" )
	systs=( "${systs[@]}" "PDF_sysnominal_PDF${i}_up" )
    done
fi
echo running on systematics: 
echo ${systs[@]}
echo 

dir=$(date +"%Y%m%d_%H%M")_${dirname} 


echo "running in dir $dir"

analysisDir=$CMSSW_BASE/src/TtZAnalysis/Analysis/

JOBSONBATCH=$SGE_CELL
if [ ${INTERACTIVE_ANALYSIS_JOBS} ]
then
JOBSONBATCH=""
fi


cd $analysisDir
#mkdir -p workdir
if [[ $JOBSONBATCH ]] ;
then
    mkdir -p /nfs/dust/cms/user/$USER/AnalysisWorkdir
    if [[ ! -L "workdir" ]]
    then
	ln -s /nfs/dust/cms/user/$USER/AnalysisWorkdir workdir 
    fi
    cd workdir
    mkdir $dir
else 
    mkdir -p $analysisDir/interactiveWorkdir  
    cd interactiveWorkdir
    mkdir $dir
fi
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

cp -r $analysisDir/*_btags .
#cp $analysisDir/*_btags.root .
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

mkdir -p $BATCHDIR


if [ $JOBSONBATCH ]
then
sed -e 's;##WORKDIR##;'${workdir}';g' < $analysisDir/check_temp.sh > check.sh
else
sed -e 's;##WORKDIR##;'${workdir}';g' -e 's;##JOBSNOTONBATCH##;;g'< $analysisDir/check_temp.sh > check.sh
fi
chmod +x check.sh

sed -e 's;##WORKDIR##;'${workdir}';g' < $analysisDir/reset_job_temp.sh > reset_job.sh
chmod +x reset_job.sh

#check wheter running on naf or wgs and do qsub or dirty "&"
for (( i=0;i<${#channels[@]};i++)); do
    for (( l=0;l<${#topmasses[@]};l++)); do
	for (( j=0;j<${#systs[@]};j++)); do
	    for (( k=0;k<${#energies[@]};k++)); do

		channel=${channels[${i}]}
		topmass=${topmasses[${l}]}
		syst=${systs[${j}]}
		energy=${energies[${k}]}

##dont run on scale,top pt and matching syst with other top mass
		if [[ "${syst}" == *"SCALE"* ]] ||  [[ "${syst}" == *"MATCH"* ]] || [[ "${syst}" == *"TOPPT"* ]] ||  [[ "${syst}" == *"_sysnominal"* ]] ||  [[ "${syst}" == *"TT_GEN"* ]]  ||  [[ "${syst}" == *"BJESNUDEC"* ]]  ||  [[ "${syst}" == *"BJESRETUNE"*  ]]
		then
		    if [[ "${topmass}" != "172.5" ]] 
		    then
			continue
		    fi
		fi

#### dont submit b variations if effieicnes are derived
		if [[ "${addParameters}" == *"-B"* ]] && [[ "${syst}" == *"BTAG"* ]]
		then
		    echo "not running btag systematics when btag SF are derived"
		    continue
		fi
		
##here do qsub or dirty &
		outname=${channel}_${energy}_${topmass}_${syst};
	   # array=( "${array[@]}" "jack" )
		outnames=( "${outnames[@]}" "${outname}" ); 
		sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst} -e ${energy} -mt ${topmass} ${addParameters}/" -e "s;##WORKDIR##;${workdir};" < $analysisDir/job.sh > jobscripts/${outname}
		chmod +x jobscripts/${outname}
		if [[ $JOBSONBATCH ]] ;
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
