#!/bin/sh

channels=("ee"
"mumu"
);
systs=("nominal"
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

dir=analysis_$(date +%F_%H:%M)

mkdir $dir
cd $dir
cp ../analyse.exe .
cp -r ../lib .
cp ../*inputfiles.txt .
cp ../analyse.C .
#cp ../job.sh .
mkdir jobscripts
workdir=`pwd`

#check wheter running on naf or wgs and do qsub or dirty "&"
for (( i=0;i<${#channels[@]};i++)); do
    for (( j=0;j<${#systs[@]};j++)); do
	for (( k=0;k<${#energies[@]};k++)); do

	    channel=${channels[${i}]}
	    syst=${systs[${j}]}
	    energy=${energies[${k}]}

##here do qsub or dirty &
	    outname=${channel}_${energy}_${syst};
	    sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst} -o ${outname}/" -e "s/##WORKDIR##/${dir}/" < ../job.sh > jobscripts/${outname}_job.sh
	    chmod +x jobscripts/${outname}_job.sh
	    ./jobscripts/${outname}_job.sh &
	done
    done
done
