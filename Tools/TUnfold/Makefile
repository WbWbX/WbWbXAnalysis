#
#  This file is part of TUnfold.
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
ROOTCONFIG   := $(ROOTSYS)/bin/root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
ROOTCINT=$(ROOTSYS)/bin/rootcint

CXXFLAGS=-isystem $(shell $(ROOTCONFIG) --incdir) -I$(ROOTSYS)/htmldoc -I. -O2 -g -Wall -Wshadow -W -Woverloaded-virtual  $(ROOTCFLAGS)
LDFLAGS=$(ROOTLDFLAGS) -L. -Wl,-rpath .

ROOTLIBS     := -lXMLParser $(ROOTLIBS)

## version number Vx.y (e.g. x=17 y=0)
## code version Vx (e.g. x=17)
## header files TUnfold*.h
## source files TUnfold*.cxx
## macro files testUnfold*.C
##
## classes have the code version in the class name
##  e.g. TUnfoldV17
##
## in the header file, #define statements are added
## such that the user can use the conventional class names
##  e.g. #define TUnfold TUnfoldV17
##
## when exported to root, the code version is removed from the class name

TUNFOLDVERSION :=$(shell grep TUnfold_VERSION TUnfold.h)
TUNFOLDVERSION :=$(filter-out \#define TUnfold_VERSION,$(TUNFOLDVERSION))
TUNFOLDVERSION :=$(shell echo $(TUNFOLDVERSION))
TUNFOLDCODEVER :=$(subst ., ,$(TUNFOLDVERSION))
TUNFOLDCODEVER :=$(word 1,$(TUNFOLDCODEVER))
HEADER=$(shell ls TUnfold*.h | grep -v Dict)
SRC=$(shell ls TUnfold*$(TUNFOLDCODEVER).cxx)
MACRO=$(shell ls testUnfold*.C)

## this is for changing the code version
TUNFOLDNEWVERSION:=V17.3
TUNFOLDNEWCODEVER :=$(subst ., ,$(TUNFOLDNEWVERSION))
TUNFOLDNEWCODEVER :=$(word 1,$(TUNFOLDNEWCODEVER))
DIRNEW:=../TUnfold_$(TUNFOLDNEWVERSION)
HEADERNEW:=$(subst TUnfold,$(DIRNEW)/TUnfold,$(HEADER))
SRCNEW:=$(subst $(TUNFOLDCODEVER),$(TUNFOLDNEWCODEVER),$(SRC))
SRCNEW:=$(subst TUnfold,$(DIRNEW)/TUnfold,$(SRCNEW))
##
## library for stand-alone test

LIB=unfold
LB=lib$(LIB).so
LB0=lib$(LIB)0.so
CINTOBJC=$(SRC:%.cxx=%Dict.o)
COBJC=$(SRC:%.cxx=%.o)

##
## macro examples for stand-alone tests

BINSRC:=$(subst Unfold,unfold,$(MACRO))
BIN:=$(BINSRC:%.C=%)

##
## figures included in the manual
#
FIGURES=$(shell ls tunfold_manual_fig*.eps)

##
## tar file including all files
##

VERSIONTAR:=TUnfold_$(TUNFOLDVERSION).tgz

##
## source tree for root
## (with version number from class names)
##
ROOTSOURCES=$(subst TUnfold,hist/hist/src/TUnfold,$(SRC:%$(TUNFOLDCODEVER).cxx=%.cxx))
ROOTHEADERS= $(subst TUnfold,hist/hist/inc/TUnfold,$(HEADER))
ROOTMACROS= $(subst testUnfold,tutorials/math/testUnfold,$(MACRO))

##
## tar file including the root source tree
##

ROOTTAR:=TUnfold_$(TUNFOLDVERSION)_for_root.tgz

## shortcuts
##
##  depend
##  clean
##  lib
##  tar
##  roottar
##  manual

depend:  $(SRC) $(HEADER)
	makedepend -V -I. -oDict.o $(SRC) $(HEADER)
	makedepend -V -I. $(BINSRC) $(MACRO) -a

clean:
	rm -f $(LB) $(LB0) *.o *Dict.* $(BIN) $(BINSRC) $(BIN:%=%.ps) *\~ tunfold_manual.pdf tunfold_manual.dvi
	rm -rf hist tutorials

lib: $(LB) $(LB0)

bin: $(BIN)

tar: $(VERSIONTAR)

roottar: $(ROOTTAR)

manual: tunfold_manual.pdf

## create PDF of the user's manual

tunfold_manual.dvi: tunfold_manual.tex $(FIGURES)
	latex $< -o $@
	latex $< -o $@

tunfold_manual.pdf: tunfold_manual.dvi
	dvipdf $<

## extract root documentation

docu: docu.C $(LB) $(LB0)
	$(CXX) $(CXXFLAGS) $< -o  $@ $(LDFLAGS) -l$(LIB) \
	$(ROOTLIBS) -lHtml
	./docu
	rm docu

# create tar file including all files beloning to this version

$(VERSIONTAR): $(HEADER) $(SRC) $(MACRO) docu.C altercodeversion.sh Makefile README COPYING tunfold_manual.tex $(FIGURES) testUnfold6binning.xml
	tar cvfz $@ $+

# create root source tree

$(ROOTTAR): $(ROOTSOURCES) $(ROOTHEADERS) $(ROOTMACROS)
	tar cvfz $@ $+

$(ROOTHEADERS): hist/hist/inc/%.h : %.h altercodeversion.sh
	mkdir -p hist/hist/inc
	altercodeversion.sh $< $(TUNFOLDCODEVER) > $@

$(ROOTSOURCES): hist/hist/src/%.cxx : %$(TUNFOLDCODEVER).cxx altercodeversion.sh
	mkdir -p hist/hist/src
	altercodeversion.sh $< $(TUNFOLDCODEVER) > $@

$(ROOTMACROS): tutorials/math/%.C : %.C
	mkdir -p tutorials/math
	cp $< $@

# create new version
# (some manual changes are required in addition)

newversion: copyoldversion $(HEADERNEW) $(SRCNEW)

copyoldversion:
	make $(VERSIONTAR)
	mkdir $(DIRNEW)
	cp $(VERSIONTAR) $(DIRNEW)/$(VERSIONTAR)
	cd $(DIRNEW) ; tar xvfz $(VERSIONTAR)
	rm $(DIRNEW)/$(VERSIONTAR)
	mkdir $(DIRNEW)/tmpheader
	rm $(DIRNEW)/*.h
	rm $(DIRNEW)/*.cxx

$(HEADERNEW): $(HEADER)
	mkdir -p $(DIRNEW)
	altercodeversion.sh $(subst $(DIRNEW)/,,$@) $(TUNFOLDCODEVER) $(TUNFOLDNEWCODEVER) > $@

$(SRCNEW): $(SRC)
	mkdir -p $(DIRNEW)
	altercodeversion.sh $(subst $(TUNFOLDNEWCODEVER),$(TUNFOLDCODEVER),$(subst $(DIRNEW)/,,$@)) $(TUNFOLDCODEVER) $(TUNFOLDNEWCODEVER) > $@

# Root cint preprocessor

%Dict.cxx: %.cxx
	rm -f $@ $(@:.cxx=.h)
	$(ROOTCINT) $@ -c $<

# library of TUnfold classes

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

$(LB): $(CINTOBJC)
	$(CXX) $(CXXFLAGS) -shared -o $(LB) $(CINTOBJC)

$(LB0): $(COBJC)
	$(CXX) $(CXXFLAGS) -shared -o $(LB0) $(COBJC)

# create code to compile example macros stand-alone

$(BINSRC): Makefile
	rm -f $@
	echo "#include <TError.h>" > $@
	echo "/* This code is generated automatically, do not edit */" >> $@
	echo "void "$(patsubst %.C,%,$(subst unfold,Unfold,$@))"();" >> $@
	echo "int main() {" >>$@
	echo "gErrorIgnoreLevel=kInfo+1;" >>$@
	echo "gErrorAbortLevel=kError;" >>$@
	echo $(patsubst %.C,%,$(subst unfold,Unfold,$@))"();" >>$@
	echo "return 0;" >> $@
	echo "}" >> $@

# compile macros stand-alone

%.o: %.C
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

$(BIN): testunfold%: testunfold%.C $(LB) $(LB0) testUnfold%.o
	$(CXX) $(CXXFLAGS) $< -o  $@ testUnfold$(*).o $(LDFLAGS) -l$(LIB) \
	$(ROOTLIBS)

# DO NOT DELETE

TUnfoldBinningV17Dict.o: TUnfoldBinningXML.h TUnfoldBinning.h TUnfold.h
TUnfoldBinningXMLV17Dict.o: TUnfoldBinningXML.h TUnfoldBinning.h TUnfold.h
TUnfoldDensityV17Dict.o: TUnfoldDensity.h TUnfoldSys.h TUnfold.h
TUnfoldDensityV17Dict.o: TUnfoldBinning.h
TUnfoldSysV17Dict.o: TUnfoldSys.h TUnfold.h
TUnfoldV17Dict.o: TUnfold.h
TUnfoldBinningDict.o: TUnfold.h
TUnfoldBinningXMLDict.o: TUnfoldBinning.h TUnfold.h
TUnfoldDensityDict.o: TUnfoldSys.h TUnfold.h TUnfoldBinning.h
TUnfoldSysDict.o: TUnfold.h

testUnfold1.o: TUnfoldDensity.h TUnfoldSys.h TUnfold.h TUnfoldBinning.h
testUnfold2.o: TUnfold.h
testUnfold3.o: TUnfoldDensity.h TUnfoldSys.h TUnfold.h TUnfoldBinning.h
testUnfold4.o: TUnfoldDensity.h TUnfoldSys.h TUnfold.h TUnfoldBinning.h
testUnfold5b.o: TUnfoldBinningXML.h TUnfoldBinning.h TUnfold.h
testUnfold5c.o: TUnfoldBinningXML.h TUnfoldBinning.h TUnfold.h
testUnfold5d.o: TUnfoldDensity.h TUnfoldSys.h TUnfold.h TUnfoldBinning.h
testUnfold6.o: TUnfoldBinningXML.h TUnfoldBinning.h TUnfold.h
