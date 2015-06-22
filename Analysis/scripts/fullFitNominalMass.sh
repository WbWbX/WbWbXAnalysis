#!/bin/zsh
#(make sure the right shell will be used)
#$ -S /bin/zsh
#$ -l site=hh
#$ -l distro=sld6
#
#(the cpu time for this job)
#$ -l h_rt=48:00:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=4096M
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#$ -m a
#$ -cwd -V
#( -l h_stack=1536M) #try with small stack
#$ -P af-cms

mkdir -p stdout
exec > "stdout/fitTtBarXsec.stdout" 2>&1

m=172.5
fitTtBarXsec -i datasets_7_8_TeV.txt --topmass $m > mt$m.log 

mv xsecFit_graph8TeV.ztop mt172.5_graph8TeV.ztop
mv xsecFit_graph7TeV.ztop mt172.5_graph7TeV.ztop
