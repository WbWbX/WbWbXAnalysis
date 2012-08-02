#!/bin/sh

# emu

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run2011A-08Nov2011-v3/AOD&instance=cms_dbs_prod_global' -O MuEG_Run2011A-08Nov2011-v3_44_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run2011B-19Nov2011-v3/AOD&instance=cms_dbs_prod_global' -O MuEG_Run2011B-19Nov2011-v3_cff.py --no-check-certificate

#double e

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run2011A-08Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleElectron_Run2011A-08Nov2011-v1_44_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run2011B-19Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleElectron_Run2011B-19Nov2011-v1_44_cff.py --no-check-certificate

#double mu

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run2011A-08Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleMu_Run2011A-08Nov2011-v1_44_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run2011B-19Nov2011-v1/AOD&instance=cms_dbs_prod_global' -O DoubleMu_Run2011B-19Nov2011-v1_44_cff.py --no-check-certificate

#met (not at desy)

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run2011B-03Feb2012-v1/AOD&instance=cms_dbs_prod_global' -O MET_Run2011B-03Feb2012-v1_44_cff.py --no-check-certificate

#single e

#single mu

# for double ratio lepton efficiencies have low priority
#can be estimated by varying cut

