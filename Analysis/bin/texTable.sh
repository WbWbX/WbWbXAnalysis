#!/bin/bash


tmpfile=$(mktemp -t "")

export TEXINPUTS=/afs/desy.de/user/k/kiesej/latexPacks:$TEXINPUTS

echo "\documentclass[10pt,a4paper]{article} \usepackage[utf8]{inputenc} \usepackage{amsmath} \usepackage{amsfonts} \usepackage{amssymb} \usepackage{lmodern} \usepackage[T1]{fontenc} \usepackage{textcomp} \usepackage{longtable} \usepackage{underscore} \begin{document}"  >> $tmpfile

#echo "\begin{longtable}" >> $tmpfile
echo "\begin{center} " >> $tmpfile
cat $1  | sed -e "s/\begin{tabular}/\begin{longtable}/g" -e "s/\end{tabular}/\end{longtable}/g" >> $tmpfile
echo "\end{center} " >> $tmpfile
#echo "\end{longtable} " >> $tmpfile


echo "\end{document}"  >> $tmpfile
pdflatex -jobname="output" $tmpfile
rm $tmpfile
