
##really quick and dirty all extraction set


mtFromXsec2 *_nominal_syst.root  --extract --makepdfs -p "total step 8"  --pseudo 10000 > totpull.txt &
mtFromXsec2 *_nominal_syst.root  --extract --makepdfs -p "m_lb min step 8" --pseudo 10000 > mlbminpull.txt &
mtFromXsec2 *_nominal_syst.root  --extract --makepdfs --normalize -p "m_lb min step 8" --pseudo 10000 > mlbminnormpull.txt &
mtFromXsec2 *_nominal_syst.root  --extract --makepdfs -p "m_lb step 8" --pseudo 10000 > mlbpull.txt &
mtFromXsec2 *_nominal_syst.root  --extract --makepdfs --normalize -p "m_lb step 8" --pseudo 10000 > mlbnormpull.txt &

wait

echo "done with fullMtopSet.sh"


exit

/TBarToDilepton_tW-channel-DR_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM
/TBarToDilepton_tW-channel-DR_mass166_5_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM 
/TBarToDilepton_tW-channel-DR_mass178_5_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM 


/TToDilepton_tW-channel-DR_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM
/TToDilepton_tW-channel-DR_mass166_5_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM
/TToDilepton_tW-channel-DR_mass178_5_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM
