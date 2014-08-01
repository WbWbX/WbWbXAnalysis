#!/bin/zsh

./submit.sh pull_calib_mlbPD09 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.9"
./submit.sh pull_calib_mlbPD08 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.8"
./submit.sh pull_calib_mlbPD07 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.7"
./submit.sh pull_calib_mlbPD06 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.6"
./submit.sh pull_calib_mlbPD05 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.5"
./submit.sh pull_calib_mlbPD04 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.4"
./submit.sh pull_calib_mlbPD03 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.3"
./submit.sh pull_calib_mlbPD02 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.2"
./submit.sh pull_calib_mlbPD01 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.1"
./submit.sh pull_calib_mlbPD00 "-B -m Notoppt -i emu_8TeV_pseudodata251.7_config.txt --startdiv 0.0"
