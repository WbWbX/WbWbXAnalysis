#!/bin/zsh -i

module use -a /afs/desy.de/group/cms/modulefiles/
module load cmssw

eval `scramv1 runtime -sh`
scram b -j20
