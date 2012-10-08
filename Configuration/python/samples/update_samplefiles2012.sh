#!/bin/sh

filenames=('DoubleElectron/Run2012A-13Jul2012-v1'
'DoubleMu/Run2012A-13Jul2012-v1'
'MuEG/Run2012A-13Jul2012-v1'
'DoubleElectron/Run2012A-recover-06Aug2012-v1'
'DoubleMu/Run2012A-recover-06Aug2012-v1'
'MuEG/Run2012A-recover-06Aug2012-v1'
'DoubleElectron/Run2012B-13Jul2012-v1'
'DoubleMu/Run2012B-13Jul2012-v4'
'MuEG/Run2012B-13Jul2012-v1'
'DoubleElectron/Run2012C-24Aug2012-v1'
'DoubleMu/Run2012C-24Aug2012-v1'
'MuEG/Run2012C-24Aug2012-v1'
'SingleMu/Run2012A-13Jul2012-v1'
'SingleElectron/Run2012A-13Jul2012-v1'
'MET/Run2012A-13Jul2012-v1'
'SingleMu/Run2012B-13Jul2012-v1'
'SingleElectron/Run2012B-13Jul2012-v1'
'MET/Run2012B-13Jul2012-v1'
'SingleElectron/Run2012C-24Aug2012-v1'
'SingleMu/Run2012C-24Aug2012-v1'
'MET/Run2012C-24Aug2012-v1'
)


entries=${#filenames[@]}

pre='https://cmsweb.cern.ch/das/makepy?dataset=/'
post='/AOD&instance=cms_dbs_prod_global'

for (( i=0;i<$entries;i++)); do

    address=$pre${filenames[${i}]}$post
    # savename=`echo ${filenames[${i}]} | awk '{split($0,a,"/"); print a[1]}'`
    savename=`echo ${filenames[${i}]} | sed 's/\//_/g'`   #too long
    wget $address -O ${savename}_cff.py --no-check-certificate
  #  echo $address
    echo $savename

done 
