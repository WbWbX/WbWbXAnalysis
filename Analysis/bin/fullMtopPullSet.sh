
##really quick and dirty all extraction set


mtFromXsec2 *_nominal_syst.root --defmt 175.5 --extract --makepdfs -p "total step 8"  --pseudo 10000 &
mtFromXsec2 *_nominal_syst.root --defmt 175.5 --extract --makepdfs -p "m_lb min step 8" --pseudo 10000 &
mtFromXsec2 *_nominal_syst.root --defmt 175.5 --extract --makepdfs --normalize -p "m_lb min step 8" --pseudo 10000 &
mtFromXsec2 *_nominal_syst.root --defmt 175.5 --extract --makepdfs -p "m_lb step 8" --pseudo 10000 &
mtFromXsec2 *_nominal_syst.root --defmt 175.5 --extract --makepdfs --normalize -p "m_lb step 8" --pseudo 10000 &
#mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -p "m_lb min step 8"&

wait

echo "done with fullMtopSet.sh"
