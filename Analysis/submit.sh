#!/bin/sh

dirname=$1
dobtag=$2

if [[ $dobtag ]]
then
echo "preparing b-efficiencies -> will be safed in channel_energy_syst_btags.root. need to be merged afterwards"
fi

channels=( #"ee"
#"mumu"
"emu"
);
systs=("nominal"
#"PU_up"
#"PU_down"
#"JER_up"
#"JER_down"
#"JES_up"
#"JES_down"
#"BTAGH_up"
#"BTAGH_down"
#"BTAGL_up"
#"BTAGL_down"
#"TT_MATCH_down"
#"TT_MATCH_up"
#"TT_SCALE_down"
#"TT_SCALE_up"
#"Z_MATCH_down"
#"Z_MATCH_up"
#"Z_SCALE_down"
#"Z_SCALE_up"
);
energies=("8TeV"
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

mkdir $dir
cd $dir
cp ../submit.sh .
cp ../analyse.exe .
cp -r ../lib .
cp ../*inputfiles.txt .
cp ../*testfiles.txt .
cp ../*btags.root .
cp ../mergeSyst.exe .

mkdir source
cd source 
cp ../../src/eventLoop.h .
cp ../../src/MainAnalyzer.cc .
cp ../../interface/MainAnalyzer.h .
cp ../../analyse.C .
cp ../../mergeSyst.cc .
cd ..
mkdir jobscripts
workdir=`pwd`


sed -e 's;##WORKDIR##;'${workdir}';g' < ../check_temp.sh > check.sh
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
	    if [[ $dobtag ]] ;
	    then
		sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-b -c ${channel} -s ${syst}/" -e "s/##WORKDIR##/${dir}/" < ../job.sh > jobscripts/${outname}
	    else
		sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst}/" -e "s/##WORKDIR##/${dir}/" < ../job.sh > jobscripts/${outname}
	    fi
	    chmod +x jobscripts/${outname}
	    if [[ $SGE_CELL ]] ;
	    then
		qsub jobscripts/${outname}
	    else
		all=`ps ax | grep -E 'analyse.exe' | wc -l`
		defunct=`ps ax | grep -E 'analyse.exe' | grep -E 'defunct' | wc -l`
		running=`expr $all - $defunct`
		while [ $running  -gt 10 ]; do
		    sleep 2;
		    all=`ps ax | grep -E 'analyse.exe' | wc -l`
		    defunct=`ps ax | grep -E 'analyse.exe' | grep -E 'defunct' | wc -l`
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
