#!/bin/sh

rqmod -c `qstat | grep Eqw | awk '{ print $1 }'`