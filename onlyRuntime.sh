#!/bin/sh
./getAdds.sh
cd $CMSSW_BASE/src
rm -f TtZAnalysis/Tools/interface/container*
rm -f TtZAnalysis/Tools/src/container*
rm -f TtZAnalysis/Tools/src/classes*
rm -f TtZAnalysis/plugins/*.h
rm -r -f CondFormats
