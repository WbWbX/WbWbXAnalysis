#!/bin/zsh

for d in naf_*
do
sed -i 's/h_vmem=1999M/h_vmem=3500M/g' $d/j_*.sh
sed -i 's/s_vmem=1699M/s_vmem=3000M/g' $d/j_*.sh
sed -i 's/h_rt=24:00:00/h_rt=54:00:00/g' $d/j_*.sh
done
