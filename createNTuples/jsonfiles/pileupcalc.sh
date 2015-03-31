#!/bin/sh

#pileupJson=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/PileUp/pileup_latest.txt
pileupJson=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/PileUp/pileup_2011_JSON_pixelLumi.txt
analysisJson=$1

#default

output=`echo ${analysisJson##*/}`


pileupCalc.py --minBiasXsec 68000 --inputLumiJSON $pileupJson --maxPileupBin 100 --numPileupBins 100 --calcMode true -i $analysisJson ${output}_PU.root
#up

pileupCalc.py --minBiasXsec 71400 --inputLumiJSON $pileupJson --maxPileupBin 100 --numPileupBins 100 --calcMode true -i $analysisJson ${output}_PU_up.root

#down

pileupCalc.py --minBiasXsec 64600 --inputLumiJSON $pileupJson --maxPileupBin 100 --numPileupBins 100 --calcMode true -i $analysisJson ${output}_PU_down.root

exit
# 2012 8 TeV

pileupCalc.py --minBiasXsec 69400 --inputLumiJSON $pileupJson --maxPileupBin 100 --numPileupBins 100 --calcMode true -i $analysisJson ${output}_PU.root

#up

pileupCalc.py --minBiasXsec 72870 --inputLumiJSON $pileupJson --maxPileupBin 100 --numPileupBins 100 --calcMode true -i $analysisJson ${output}_PU_up.root

#down

pileupCalc.py --minBiasXsec 65930 --inputLumiJSON $pileupJson --maxPileupBin 100 --numPileupBins 100 --calcMode true -i $analysisJson ${output}_PU_down.root

#up: 72870
#down 65930
