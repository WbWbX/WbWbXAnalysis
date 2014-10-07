#include <iostream>

#include <THtml.h>

#include "TUnfoldDensity.h"

/*
  This file is part of TUnfold.

  TUnfold is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TUnfold is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with TUnfold.  If not, see <http://www.gnu.org/licenses/>.
*/

// test program: create automatic documentation for the TUnfold package

int main(int argc,char * const argv[]) {
  THtml html;
  html.MakeClass("TUnfoldV17");
  html.MakeClass("TUnfoldSysV17");
  html.MakeClass("TUnfoldDensityV17");
  html.MakeClass("TUnfoldBinningV17");

  return 0;
}
