#!/bin/bash
#enter full DAS filename (without AODSIM)

filenames=('TTJets_TuneZ2_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy some t2
'TTJets_TuneZ2_mass169_5_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy ES US
'TTJets_TuneZ2_mass175_5_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy ES US
'TTjets_TuneZ2_matchingdown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy BE
'TTjets_TuneZ2_matchingup_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy BE
'TTjets_TuneZ2_scaledown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy RU BE
'TTjets_TuneZ2_scaleup_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy RU BE
'DYJetsToLL_M-10To50_TuneZ2_7TeV-madgraph/Fall11-PU_S6_START44_V9B-v1' #not at desy ES RWTH
'DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy some t2
'ZJetsToLL_TuneZ2_matchingdown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy UK
'ZJetsToLL_TuneZ2_matchingup_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy UK
'ZJetsToLL_TuneZ2_scaleup_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy UK IT
'ZJetsToLL_TuneZ2_scaledown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy UK IT
'WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy RWTH EE UK
'WWTo2L2Nu_TuneZ2_7TeV_pythia6_tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy BR US
'WZ_TuneZ2_7TeV_pythia6_tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy BE RWTH
'ZZ_TuneZ2_7TeV_pythia6_tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy RWTH BE
'Tbar_TuneZ2_tW-channel-DS_7TeV-powheg-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy EE UK
'T_TuneZ2_tW-channel-DS_7TeV-powheg-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy EE UK
)

entries=${#filenames[@]}

pre='https://cmsweb.cern.ch/das/makepy?dataset=/'
post='/AODSIM&instance=cms_dbs_prod_global'

for (( i=0;i<$entries;i++)); do

    address=$pre${filenames[${i}]}$post
   # savename=`echo ${filenames[${i}]} | awk '{split($0,a,"/"); print a[1]}'`
    savename=`echo ${filenames[${i}]} | sed 's/\//_/g'`   #too long
    savename=`echo ${filenames[${i}]} | sed 's/-/_/g'`   #too long
    wget $address -O ${savename}_cff.py --no-check-certificate
  #  echo $address
  #  echo $savename

done 

echo "\n\nList for requests:"
for (( i=0;i<$entries;i++)); do

echo "/"${filenames[${i}]}"/AODSIM"

done
