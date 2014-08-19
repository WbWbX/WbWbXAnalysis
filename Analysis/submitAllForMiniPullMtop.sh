#!/bin/zsh

#psuedodatafile="emu_8TeV_pseudodata251.7_config.txt"
pseudodatafile="emu_8TeV_mtC172.5_config.txt"
#pseudodatafile="emu_8TeV_mtC166.5_config.txt"
outdir="pull_172.5_calib_newNormNew_mlbPD"

./submit.sh ${outdir}09 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.9"
./submit.sh ${outdir}08 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.8"
./submit.sh ${outdir}07 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.7"
./submit.sh ${outdir}06 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.6"
./submit.sh ${outdir}05 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.5"
./submit.sh ${outdir}04 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.4"
./submit.sh ${outdir}03 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.3"
./submit.sh ${outdir}02 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.2"
./submit.sh ${outdir}01 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.1"
./submit.sh ${outdir}00 "-B -m Notoppt -i ${pseudodatafile} --startdiv 0.0"
