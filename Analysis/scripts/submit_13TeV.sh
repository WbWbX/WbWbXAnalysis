#!/bin/sh


channels=( #"ee"
    "emu"
    #"mumu"
);
## scale and match variations will be ignored for other top masses then 172.5.. (hardcoded below)
topmasses=( "172.5"

     # "171.5"
     # "173.5"
#agrohsje 
#    "175.5"
#    "169.5"

     # "178.5"
     # "166.5"
);
pdfeigenvectors=50 # POWHEG:NNPDF:50; MG5_aMC@NLO:NNPDF:51   # if set to 0, no variation

systs=("nominal"

	# for testing	
#    "TOPMASS_up"
#    "TOPMASS_down"

#    "P11_sysnominal"
#    "P11_sysnominal_CR_up"
#    "P11_sysnominal_CR_down"
#    "P11_sysnominal_UE_up"
#    "P11_sysnominal_UE_down"

#   "TT_GENPOWPY_up"
#   "TT_GENPOWPY_down"

# use later for powheg herwig, file ending should be powhpp
   # "TT_GENPOWHERW_up"
   # "TT_GENPOWHERW_down"


   #  "TT_GENMCATNLO_up"
   #  "TT_GENMCATNLO_down"

#    "TT_BJESNUDEC_down"
#    "TT_BJESNUDEC_up"

#    "TT_BJESRETUNE_down"
#    "TT_BJESRETUNE_up"



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

#    "JES_up"
#    "JES_down"

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
   # "BTAGHFS1_up"
   # "BTAGHFS1_down"
   # "BTAGHFS2_up"
   # "BTAGHFS2_down"
   # "BTAGLFS1_up"
   # "BTAGLFS1_down"
   # "BTAGLFS2_up"
   # "BTAGLFS2_down"
   # "BTAGPUR_up"
   # "BTAGPUR_down"


#     "TOPPT_up"
#     "TOPPT_down"

#    "TT_MATCH_down"
#    "TT_MATCH_up"
    "TT_SCALE_down"
    "TT_SCALE_up"


   #"Z_MATCH_down"
   #"Z_MATCH_up"
   #"Z_SCALE_down"
   #"Z_SCALE_up"
);
energies=("13TeV"
#"7TeV"
);



##############################
##############################
##############################
####### USER INPUT ENDS ######
##############################
##############################
##############################

thisscript=$(pwd)/$0
dirname=$1
addParameters=$2

analysisDir=$CMSSW_BASE/src/TtZAnalysis/Analysis/
templatesDir=$CMSSW_BASE/src/TtZAnalysis/Analysis/templates


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

sleep 3



JOBSONBATCH=$SGE_CELL
if [ ${INTERACTIVE_ANALYSIS_JOBS} ]
then
JOBSONBATCH=""
fi


cd $analysisDir
workdir=$(date +"%Y%m%d_%H%M")_${dirname}
if [[ $JOBSONBATCH ]] ;
then
    mkdir -p /nfs/dust/cms/user/$USER/AnalysisWorkdir
    if [[ ! -L "workdir" ]]
    then
	ln -s /nfs/dust/cms/user/$USER/AnalysisWorkdir workdir 
    fi
    cd workdir
    mkdir $workdir
else 
    mkdir -p $analysisDir/interactiveWorkdir  
    cd interactiveWorkdir
    mkdir $workdir
fi
if [[ -L "last" ]]
then
    rm last
fi
ln -s $workdir last
cd $workdir

echo "running in dir $workdir"
#make path absolute
workdir=`pwd`
BATCHDIR=$workdir/batch

#make directory tree

mkdir -p $workdir/scripts
mkdir -p $workdir/bin
mkdir -p $workdir/lib #not needed anymore
mkdir -p $workdir/data/analyse
mkdir -p $workdir/jobscripts
mkdir -p $workdir/stdout
mkdir -p $workdir/configs/analyse
mkdir -p $BATCHDIR

cp $thisscript scripts/

echo creating independent executable package...
echo "(code can be compiled while jobs are running)"
cd $workdir/bin
makePackage.sh analyse $workdir/bin/analyse& #can run in background..
cd $workdir

echo copying data files...
rsync -a $analysisDir/data/analyse data/ --exclude *_btags
cd $analysisDir/data/analyse
for d in *_btags
do
    tar czf $workdir/data/analyse/$d.tar.gz $d &
done
wait
cd $workdir/data/analyse
for f in *_btags.tar.gz #maybe there are even more
do
    tar xzf $f &
done
# do other stuff in the meantime...


cd $workdir
cp -r $analysisDir/configs/analyse configs/
cp -r $analysisDir/src .
cp $analysisDir/interface/MainAnalyzer.h src/
cp $analysisDir/bin/analyse.cc src/

echo -n "These files are just meant for reference (e.g. to check control plot configurations etc)" > src/README

cd $workdir


if [ $JOBSONBATCH ]
then
sed -e 's;##WORKDIR##;'${workdir}';g' < $templatesDir/check.sh > check.sh
else
sed -e 's;##WORKDIR##;'${workdir}';g' -e 's;##JOBSNOTONBATCH##;;g'< $templatesDir/check.sh > check.sh
fi
chmod +x check.sh

sed -e 's;##WORKDIR##;'${workdir}';g' < $templatesDir/reset_job.sh > reset_job.sh
chmod +x reset_job.sh

# wait for tars to be finished and delete
wait
for f in $workdir/data/analyse/*_btags.tar.gz
do
     rm -f $f &
done

echo "preparation done, start submission..."
cd $workdir
#check whether running on naf or wgs and do qsub or dirty "&"
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
		if [[ "${syst}" == *"P11_sysnominal"* ]] && [[ "${energy}" = "7TeV" ]] 
		    then
		    continue
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
		sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst} -e ${energy} -mt ${topmass} ${addParameters}/" -e "s;##WORKDIR##;${workdir};" < $templatesDir/job.sh > jobscripts/${outname}
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

