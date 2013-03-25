#!/bin/sh

pileupJson=$1
analysisJson=$2

#default

pileupCalc.py --minBiasXsec 69400 --inputLumiJSON $pileupJson --maxPileupBin 50 --numPileupBins 50 --calcMode true -i $analysisJson ${analysisJson}_PU.root

#up

pileupCalc.py --minBiasXsec 72870 --inputLumiJSON $pileupJson --maxPileupBin 70 --numPileupBins 70 --calcMode true -i $analysisJson ${analysisJson}_PU_up.root

#down

pileupCalc.py --minBiasXsec 65930 --inputLumiJSON $pileupJson --maxPileupBin 70 --numPileupBins 70 --calcMode true -i $analysisJson ${analysisJson}_PU_down.root

#up: 72870
#down 65930
