
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "total step 8" -f pol2 -o pol2 | grep __MTOP__ > total_pol2.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "total step 8" -f pol3 -o pol3 | grep __MTOP__ > total_pol3.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "total step 8" -f pol4 -o pol4 | grep __MTOP__ > total_pol4.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "total step 8" -f pol5 -o pol5 | grep __MTOP__ > total_pol5.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "total step 8" -f pol6 -o pol6 | grep __MTOP__ > total_pol6.txt&



mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb min step 8"  -f pol2 -o pol2 | grep __MTOP__ > min_pol2.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb min step 8"  -f pol3 -o pol3 | grep __MTOP__ > min_pol3.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb min step 8"  -f pol4 -o pol4 | grep __MTOP__ > min_pol4.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb min step 8"  -f pol5 -o pol5 | grep __MTOP__ > min_pol5.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb min step 8"  -f pol6 -o pol6 | grep __MTOP__ > min_pol6.txt&

wait

mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb min step 8" -f pol2 -o pol2 | grep __MTOP__ > min_normd_pol2.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb min step 8" -f pol3 -o pol3 | grep __MTOP__ > min_normd_pol3.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb min step 8" -f pol4 -o pol4 | grep __MTOP__ > min_normd_pol4.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb min step 8" -f pol5 -o pol5 | grep __MTOP__ > min_normd_pol5.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb min step 8" -f pol6 -o pol6 | grep __MTOP__ > min_normd_pol6.txt&



mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb step 8"  -f pol2 -o pol2 | grep __MTOP__ > star_pol2.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb step 8"  -f pol3 -o pol3 | grep __MTOP__ > star_pol3.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb step 8"  -f pol4 -o pol4 | grep __MTOP__ > star_pol4.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb step 8"  -f pol5 -o pol5 | grep __MTOP__ > star_pol5.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst -p "m_lb step 8"  -f pol6 -o pol6 | grep __MTOP__ > star_pol6.txt&

wait

mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb step 8" -f pol2 -o pol2 | grep __MTOP__ > star_normd_pol2.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb step 8" -f pol3 -o pol3 | grep __MTOP__ > star_normd_pol3.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb step 8" -f pol4 -o pol4 | grep __MTOP__ > star_normd_pol4.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb step 8" -f pol5 -o pol5 | grep __MTOP__ > star_normd_pol5.txt&
mtFromXsec2 *_nominal_syst.root --extract --nosyst --normalize -p "m_lb step 8" -f pol6 -o pol6 | grep __MTOP__ > star_normd_pol6.txt&
