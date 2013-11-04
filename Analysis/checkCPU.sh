#!/bin/sh
LD_LIBRARY_PATH=/afs/desy.de/group/cms/perftool/.amd64_linux26/libunwind/lib/:$LD_LIBRARY_PATH

CPUPROFILE=$1.prof LD_PRELOAD=/afs/desy.de/group/cms/perftool/v2.0/lib/libprofiler.so analyse -c emu -s nominal -o test -b -S -i emu_8TeV_config_test.txt -o perfout

/afs/desy.de/group/cms/perftool/v2.0/bin/pprof --pdf analyse $1.prof >| $1.pdf
