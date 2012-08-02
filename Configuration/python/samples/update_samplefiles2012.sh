#!/bin/sh

# emu

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run20122012A-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O mueg_run2012A_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run20122012B-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O mueg_run2012B_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MuEG/Run20122012C-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O mueg_run2012C_prompt_cff.py --no-check-certificate


#double e

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run20122012A-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O ee_run2012A_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run20122012B-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O ee_run2012B_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleElectron/Run20122012C-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O ee_run2012C_prompt_cff.py --no-check-certificate

#double mu

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run20122012A-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O mumu_run2012A_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run20122012B-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O mumu_run2012B_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/DoubleMu/Run20122012C-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O mumu_run2012C_prompt_cff.py --no-check-certificate



#met

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run20122012A-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O met_run2012A_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run20122012B-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O met_run2012B_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run20122012C-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O met_run2012C_prompt_cff.py --no-check-certificate


#single e

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/SingleElectron/Run20122012A-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O singlee_run2012A_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/SingleElectron/Run20122012B-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O singlee_run2012B_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/SingleElectron/Run20122012C-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O singlee_run2012C_prompt_cff.py --no-check-certificate


#single mu

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/SingleMu/Run20122012A-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O singlemu_run2012A_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/SingleMu/Run20122012B-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O singlemu_run2012B_prompt_cff.py --no-check-certificate

wget 'https://cmsweb.cern.ch/das/makepy?dataset=/SingleMu/Run20122012C-PromptReco-v1/AOD&instance=cms_dbs_prod_global' -O singlemu_run2012C_prompt_cff.py --no-check-certificate
