#!/bin/bash

filenames=('T_tW-channel-DR_TuneZ2star_8TeV-powheg-tauola/Summer12-PU_S7_START52_V9-v1' 
'Tbar_tW-channel-DR_TuneZ2star_8TeV-powheg-tauola/Summer12-PU_S7_START52_V9-v1' 
'DYJetsToLL_M-10To50filter_8TeV-madgraph/Summer12-PU_S7_START52_V9-v1' 
'DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/Summer12-PU_S7_START52_V9-v2' 
'WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball/Summer12-PU_S7_START52_V9-v1' 
'TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/Summer12-PU_S7_START52_V9-v2'
'ZZTo2L2Nu_TuneZ2star_8TeV_pythia6_tauola/Summer12-PU_S7_START52_V9-v1' 
'ZZ_TuneZ2star_8TeV_pythia6_tauola/Summer12-PU_S7_START52_V9-v1' 
'WZ_TuneZ2star_8TeV_pythia6_tauola/Summer12-PU_S7_START52_V9-v1' 
'WW_TuneZ2star_8TeV_pythia6_tauola/Summer12-PU_S7_START52_V9-v1' 
'WWJetsTo2L2Nu_TuneZ2star_8TeV-madgraph-tauola/Summer12-PU_S7_START52_V9-v1' 
'QCD_Pt_20_MuEnrichedPt_15_TuneZ2star_8TeV_pythia6/Summer12-PU_S7_START52_V9-v1' 
'QCD_Pt_20_30_EMEnriched_TuneZ2star_8TeV_pythia6/Summer12-PU_S7_START52_V9-v1' 
'QCD_Pt_30_80_EMEnriched_TuneZ2star_8TeV_pythia6/Summer12-PU_S7_START52_V9-v1' 
'QCD_Pt_80_170_EMEnriched_TuneZ2star_8TeV_pythia6/Summer12-PU_S7_START52_V9-v1'
'DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/Summer12_DR53X-PU_S10_START53_V7A-v1')

#mcproduction='Summer12-PU_S7_START52_V9-v1'
#version='v1'

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

