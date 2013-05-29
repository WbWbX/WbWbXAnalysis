#!/bin/sh
${CMSSW_BASE}/src/TtZAnalysis/getAdds.sh

echo 'checking out release V08-07-50 - check if newest one'

addpkg DataFormats/PatCandidates V06-04-38
addpkg PhysicsTools/PatAlgos     V08-07-50
addpkg PhysicsTools/PatUtils     V03-09-18-03
addpkg CommonTools/ParticleFlow  V00-03-05-10
addpkg FWCore/GuiBrowsers        V00-00-60


echo 'checked out release V08-07-50 - check if newest one'
echo 'deleting tools not compiling in CMSSW_4_4_X and not needed for sending jobs'


rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/container*
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/eff*
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/classes.h
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/classes_def.xml
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/interface/container*
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/interface/eff*

rm -rf ${CMSSW_BASE}/src/TtZAnalysis/Analysis
rm -rf ${CMSSW_BASE}/src/TtZAnalysis/Tools

rm -rf ${CMSSW_BASE}/src/TopAnalysis/TopUtils/plugins/*eight*
rm -rf ${CMSSW_BASE}/src/TopAnalysis/TopUtils/plugins/*vent*
rm -rf ${CMSSW_BASE}/src/TopAnalysis/TopUtils/plugins/*Seal*
rm -rf ${CMSSW_BASE}/src/TopAnalysis/TopUtils/python/particleFlowSetup_cff.py


rm -rf ${CMSSW_BASE}/src/RecoLocalTracker/SubCollectionProducers
rm -rf ${CMSSW_BASE}/src/RecoMET
rm -rf ${CMSSW_BASE}/src/CommonTools/RecoAlgos
rm -rf ${CMSSW_BASE}/src/DPGAnalysis/Skims
rm -rf ${CMSSW_BASE}/src/DPGAnalysis/SiStripTools
rm -rf ${CMSSW_BASE}/src/DataFormats/TrackerCommon


rm -f ${CMSSW_BASE}/src/RecoMET/METFilters/plugins/EcalDeadCellBoundaryEnergyFilter.cc
rm -f ${CMSSW_BASE}/src/RecoMET/METFilters/plugins/HcalLaserEventFilter.cc
rm -f ${CMSSW_BASE}/src/RecoMET/METFilters/plugins/EcalLaserCorrFilter.cc
