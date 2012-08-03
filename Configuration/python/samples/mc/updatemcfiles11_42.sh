#!/bin/bash
#enter full DAS filename (without AODSIM)

filenames=('TTJets_TuneZ2_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'TTJets_TuneZ2_mass169_5_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'TTJets_TuneZ2_mass175_5_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'TTjets_TuneZ2_matchingdown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'TTjets_TuneZ2_matchingup_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'TTjets_TuneZ2_scaledown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'TTjets_TuneZ2_scaleup_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'DYJetsToLL_M-10To50_TuneZ2_7TeV-madgraph/Fall11-PU_S6_START44_V9B-v1' #not at desy
'DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
'ZJetsToLL_TuneZ2_matchingdown_7TeV-madgraph-tauola/Fall11-PU_S6_START44_V9B-v1' #not at desy
''
)

entries=${#filenames[@]}

pre='https://cmsweb.cern.ch/das/makepy?dataset=/'
post='/AODSIM&instance=cms_dbs_prod_global'

for (( i=0;i<$entries;i++)); do

    address=$pre${filenames[${i}]}$post
   # savename=`echo ${filenames[${i}]} | awk '{split($0,a,"/"); print a[1]}'`
    savename=`echo ${filenames[${i}]} | sed 's/\//_/g'`   #too long
    wget $address -O ${savename}_cff.py --no-check-certificate
  #  echo $address
    echo $savename

done 

