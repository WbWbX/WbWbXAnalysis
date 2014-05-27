#!/bin/sh

## dont forget to confige submit.sh before!!



./submit.sh ${1}_ClosureMtC166.5 "-i emu_8TeV_mtC166.5_config.txt -B ${2}"
./submit.sh ${1}_ClosureMtC169.5 "-i emu_8TeV_mtC169.5_config.txt -B ${2}"
./submit.sh ${1}_ClosureMtC171.5 "-i emu_8TeV_mtC171.5_config.txt -B ${2}"
./submit.sh ${1}_ClosureMtC172.5 "-i emu_8TeV_mtC172.5_config.txt -B ${2}"
./submit.sh ${1}_ClosureMtC173.5 "-i emu_8TeV_mtC173.5_config.txt -B ${2}"
./submit.sh ${1}_ClosureMtC175.5 "-i emu_8TeV_mtC175.5_config.txt -B ${2}"
./submit.sh ${1}_ClosureMtC178.5 "-i emu_8TeV_mtC178.5_config.txt -B ${2}"
