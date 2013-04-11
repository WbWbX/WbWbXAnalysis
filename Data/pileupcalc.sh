#!/bin/sh

pileupJson=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/PileUp/pileup_latest.txt
analysisJson=$1

#default

output=`echo ${analysisJson##*/}`

pileupCalc.py --minBiasXsec 69400 --inputLumiJSON $pileupJson --maxPileupBin 50 --numPileupBins 50 --calcMode true -i $analysisJson ${output}_PU.root

#up

pileupCalc.py --minBiasXsec 72870 --inputLumiJSON $pileupJson --maxPileupBin 70 --numPileupBins 70 --calcMode true -i $analysisJson ${output}_PU_up.root

#down

pileupCalc.py --minBiasXsec 65930 --inputLumiJSON $pileupJson --maxPileupBin 70 --numPileupBins 70 --calcMode true -i $analysisJson ${output}_PU_down.root

#up: 72870
#down 65930
