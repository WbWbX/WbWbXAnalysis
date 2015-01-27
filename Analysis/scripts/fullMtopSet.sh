
##really quick and dirty all extraction set

mtFromXsec2 *_nominal_syst.root --makepdfs -p "total step 8" # for controlPlots

#dont overthread
export OMP_NUM_THREADS=12

mtFromXsec2 *_nominal_syst.root --extract --makepdfs -p "total step 8"&
mtFromXsec2 *_nominal_syst.root --extract --makepdfs -p "m_lb min step 8"&
mtFromXsec2 *_nominal_syst.root --extract --makepdfs --normalize -p "m_lb min step 8"&

mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -p "m_lb min step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs --normalize -p "m_lb min step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs --normalize -p "m_lb min step 8"&

wait
#echo "******************************\ndone with total and mlbmin based extraction\n*****************"

mtFromXsec2 *_nominal_syst.root --extract --makepdfs -p "m_lb step 8"&
mtFromXsec2 *_nominal_syst.root --extract --makepdfs --normalize -p "m_lb step 8"&


mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -p "m_lb step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs --normalize -p "m_lb step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs --normalize -p "m_lb step 8"&



mtFromXsec2 *_nominal_syst.root mcfm_tota*.root --prefix mcfm_tota_ --extract --makepdfs --normalize -p "m_lb min step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_tota*.root --prefix mcfm_tota_ --extract --makepdfs --normalize -p "m_lb step 8"&






mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -K -p "m_lb min step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs -p "m_lb min step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -K -p "m_lb step 8"&
mtFromXsec2 *_nominal_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs -p "m_lb step 8"&

wait

#put everything in a tar

rm -f mtextract_*/summary.pdf*
#rm -f tota_mtextract_*/tota_summary.pdf*
rm -f fullMtopSet.tar
tar cf fullMtopSet.tar mtextract_* controlPlots

echo "done with fullMtopSet.sh, tar created"
