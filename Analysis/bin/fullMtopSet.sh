
##really quick and dirty all extraction set


mtFromXsec2 *_syst.root --extract --makepdfs -p "total step 8"
mtFromXsec2 *_syst.root --extract --makepdfs -p "m_lb step 8"
mtFromXsec2 *_syst.root --extract --makepdfs -p "m_lb min step 8"
mtFromXsec2 *_syst.root --extract --makepdfs --normalize -p "m_lb step 8"
mtFromXsec2 *_syst.root --extract --makepdfs --normalize -p "m_lb min step 8"

mtFromXsec2 *_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -p "m_lb step 8"
mtFromXsec2 *_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -p "m_lb min step 8"
mtFromXsec2 *_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -K -p "m_lb step 8"
mtFromXsec2 *_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs -K -p "m_lb min step 8"
mtFromXsec2 *_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs --normalize -p "m_lb step 8"
mtFromXsec2 *_syst.root mcfm_todk*.root --prefix mcfm_todk_ --extract --makepdfs --normalize -p "m_lb min step 8"

mtFromXsec2 *_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs --normalize -p "m_lb step 8"
mtFromXsec2 *_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs --normalize -p "m_lb min step 8"
mtFromXsec2 *_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs -p "m_lb step 8"
mtFromXsec2 *_syst.root mcfm_lord*.root --prefix mcfm_lord_ --extract --makepdfs -p "m_lb min step 8"
