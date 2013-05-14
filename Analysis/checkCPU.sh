#!/bin/sh

CPUPROFILE=$1.prof LD_PRELOAD=/afs/desy.de/group/cms/perftool/v2.0/lib/libprofiler.so analyse.exe -c ee -s nominal -b -o perfout

/afs/desy.de/group/cms/perftool/v2.0/bin/pprof --pdf analyse.exe $1.prof >| $1.pdf
