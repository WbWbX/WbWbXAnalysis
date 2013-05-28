#!/bin/sh
${CMSSW_BASE}/src/TtZAnalysis/getAdds.sh

echo 'checking out release V08-07-50 - check if newest one'

addpkg DataFormats/PatCandidates V06-04-38
addpkg PhysicsTools/PatAlgos     V08-07-50
addpkg PhysicsTools/PatUtils     V03-09-18-03
addpkg CommonTools/ParticleFlow  V00-03-05-10
addpkg FWCore/GuiBrowsers        V00-00-60


echo 'checked out release V08-07-50 - check if newest one'
echo 'deleting tools not compiling in CMSSW_4_4_X'


rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/container*
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/eff*
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/classes.h
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/src/classes_def.xml
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/interface/container*
rm -f ${CMSSW_BASE}/src/TtZAnalysis/Tools/interface/eff*

rm -f ${CMSSW_BASE}/src/RecoMET/METFilters/plugins/EcalDeadCellBoundaryEnergyFilter.cc
rm -f ${CMSSW_BASE}/src/RecoMET/METFilters/plugins/HcalLaserEventFilter.cc
rm -f ${CMSSW_BASE}/src/RecoMET/METFilters/plugins/EcalLaserCorrFilter.cc
