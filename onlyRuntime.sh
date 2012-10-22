#!/bin/sh
./getAdds.sh
cd $CMSSW_BASE/src
rm TtZAnalysis/Tools/interface/container*
rm TtZAnalysis/Tools/src/container*
rm TtZAnalysis/Tools/src/classes*
rm TtZAnalysis/plugins/*.h
