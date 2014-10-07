#!/bin/sh
#
#  This file is part of TUnfold.
#  Package version number 17.2
#
#  TUnfold is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  TUnfold is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with TUnfold.  If not, see <http://www.gnu.org/licenses/>.
#
#
classname=${1%%.*}
classname=${classname%%$2}
sedcommand="$2$3.sed"
rm -f $sedcommand
if [[ -z $3 ]] ; then
    echo "s/TUnfold"$2"/TUnfold/" > $sedcommand
    echo "s/TUnfoldSys"$2"/TUnfoldSys/" >> $sedcommand
    echo "s/TUnfoldBinning"$2"/TUnfoldBinning/" >> $sedcommand
    echo "s/TUnfoldDensity"$2"/TUnfoldDensity/" >> $sedcommand
    echo "s/TUnfoldBinningXML"$2"/TUnfoldBinningXML/" >> $sedcommand
    grep -v "#define "$classname" " $1 | sed -f ${sedcommand} | sed -f ${sedcommand} | sed -f ${sedcommand}
else
    echo "s/TUnfold"$2"/TUnfold"$3"/" > $sedcommand
    echo "s/TUnfoldSys"$2"/TUnfoldSys"$3"/" >> $sedcommand
    echo "s/TUnfoldBinning"$2"/TUnfoldBinning"$3"/" >> $sedcommand
    echo "s/TUnfoldDensity"$2"/TUnfoldDensity"$3"/" >> $sedcommand
    echo "s/TUnfoldBinningXML"$2"/TUnfoldBinningXML"$3"/" >> $sedcommand
    sed -f ${sedcommand} <$1  | sed -f ${sedcommand} | sed -f ${sedcommand}
fi
