#!/bin/sh

channels=("ee"
"mumu"
);
systs=("nominal"
"JER_up"
"JER_down"
"JES_up"
"JES_down"
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
cp ../submit.sh .
cp ../analyse.exe .
cp -r ../lib .
cp ../*inputfiles.txt .
cp ../*testfiles.txt .
cp ../analyse.C .
cp ../*btags.root .
cp ../mergeSyst.cc .
cp ../mergeSyst.exe .
mkdir jobscripts
workdir=`pwd`

cp ../check_temp.sh check.sh
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
	    array=( "${array[@]}" "jack" )
	    outnames=( "${outnames[@]}" "${outname}" );
	    sed -e "s/##OUTNAME##/${outname}/" -e "s/##PARAMETERS##/-c ${channel} -s ${syst}/" -e "s/##WORKDIR##/${dir}/" < ../job.sh > jobscripts/${outname}
	    chmod +x jobscripts/${outname}
	    if [[ $SGE_CELL ]] ;
	    then
	echo	qsub jobscripts/${outname}
	    else
	echo	./jobscripts/${outname} &
	    fi

###make the merge line

###what about check script?

	done
    done
done
