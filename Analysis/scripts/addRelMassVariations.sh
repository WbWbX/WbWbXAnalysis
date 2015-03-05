#!/bin/zsh


for f in emu_8TeV_*nominal_syst.root
do
addRelSystematics emu_8TeV_172.5_nominal_syst.root $f&
done
wait

for f in emu_7TeV_*nominal_syst.root
do
addRelSystematics emu_7TeV_172.5_nominal_syst.root $f&
done
wait

for f in emu_7TeV_*nominal_syst.root
do
addRelSystematics emu_8TeV_172.5_nominal_syst.root $f&
done
wait

echo merging of systematics done
