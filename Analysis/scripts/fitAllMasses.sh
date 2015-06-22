#!/bin/sh
option=$1
for m in {166.5,172.5,178.5}
do
if [ $m == "172.5" ]
    then
        fitTtBarXsec -i datasets_7_8_TeV.txt --topmass $m $option > mt$m.log &
else
    fitTtBarXsec -i datasets_7_8_TeV.txt --onlytotal -o mt$m$extra --topmass $m$extra  > mt$m$extra.log &
fi
done
wait

mv xsecFit_graph8TeV.ztop mt172.5_graph8TeV.ztop
mv xsecFit_graph7TeV.ztop mt172.5_graph7TeV.ztop

echo ALL DONE

