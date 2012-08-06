#!/bin/sh


<<<<<<< update_samplefiles2011.sh
wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run2011A-08Nov2011-v3/AOD&instance=cms_dbs_prod_global' -O MuEG_Run2011A-08Nov2011-v3_cff.py --no-check-certificate
=======
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
    wget $address -O ${savename}_cff.py --no-check-certificate
  #  echo $address
    echo $savename
>>>>>>> 1.2

<<<<<<< update_samplefiles2011.sh
wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run2011B-19Nov2011-v3/AOD&instance=cms_dbs_prod_global' -O MuEG_Run2011B-19Nov2011-v3_cff.py --no-check-certificate

#double e

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run2011A-08Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleElectron_Run2011A-08Nov2011-v1_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run2011B-19Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleElectron_Run2011B-19Nov2011-v1_cff.py --no-check-certificate

#double mu

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run2011A-08Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleMu_Run2011A-08Nov2011-v1_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run2011B-19Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleMu_Run2011B-19Nov2011-v1_cff.py --no-check-certificate

#met (not at desy)

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run2011B-03Feb2012-v1/AOD&instance=cms_dbs_prod_global' -O MET_Run2011B-03Feb2012-v1_cff.py --no-check-certificate

#single e

#single mu

# for double ratio lepton efficiencies have low priority
#can be estimated by varying cut
=======
done 
>>>>>>> 1.2

