#!/bin/sh

filenames=('MuEG/Run2011A-08Nov2011-v3' # all 4_4_X  #Cern,Fr
'MuEG/Run2011B-19Nov2011-v3' #'
'DoubleElectron/Run2011A-08Nov2011-v1' #at desy
'DoubleElectron/Run2011B-19Nov2011-v1' #at desy
'DoubleMu/Run2011A-08Nov2011-v1' #at desy
'DoubleMu/Run2011B-19Nov2011-v1' #at desy
'MET/Run2011B-03Feb2012-v1' # TW AACH
#''  #all 4_2_X
#''
)

entries=${#filenames[@]}

pre='https://cmsweb.cern.ch/das/makepy?dataset=/'
post='/AOD&instance=cms_dbs_prod_global'

for (( i=0;i<$entries;i++)); do

    address=$pre${filenames[${i}]}$post
    # savename=`echo ${filenames[${i}]} | awk '{split($0,a,"/"); print a[1]}'`
    savename=`echo ${filenames[${i}]} | sed 's/\//_/g'`   #too long
    savename=`echo ${filenames[${i}]} | sed 's/-/_/g'`   #too long
    wget $address -O ${savename}_cff.py --no-check-certificate
  #  echo $address
    echo $savename

done 

