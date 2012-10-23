#!/bin/bash

filenames=('TTTo2L2Nu2B_8TeV-powheg-pythia6/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_FullLeptMGDecays_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_scaledown_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_scaleup_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_matchingdown_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_matchingup_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_mass161_5_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1'
'TTJets_mass184_5_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1')

#mcproduction='Summer12-PU_S7_START52_V9-v1'
#version='v1'

entries=${#filenames[@]}

pre='https://cmsweb.cern.ch/das/makepy?dataset=/'
post='/AODSIM&instance=cms_dbs_prod_global'

for (( i=0;i<$entries;i++)); do

    address=$pre${filenames[${i}]}$post
    # savename=`echo ${filenames[${i}]} | awk '{split($0,a,"/"); print a[1]}'`
    savename=`echo ${filenames[${i}]} | sed 's/\//_/g'`   #too long
    savename=`echo ${savename} | sed 's/-/_/g'`   #too long
    wget $address -O ${savename}_cff.py --no-check-certificate
  #  echo $address
    echo $savename

done 

