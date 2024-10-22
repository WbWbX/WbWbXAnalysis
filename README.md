
### Instructions

* setup a not-so-old CMSSW env (tested with CMSSW_11_1_7)
* clone `https://gitlab.cern.ch/cms-desy-top/TopAnalysis` into `$CMSSW_BASE/src`
* go to `$CMSSW_BASE/src/TopAnalysis/ZTopUtils` and build the subpackage with `scram b -j16`
* go back to `$CMSSW_BASE/src` and clone `https://github.com/WbWbX/WbWbXAnalysis`
* run `./prepareEnv.sh` twice inside `$CMSSW_BASE/src/WbWbXAnalysis` (TUnfold lib fails the first time)
* go to `$CMSSW_BASE/src/WbWbXAnalysis/Tools` and build the subpackage with `scram b -j16`


### Previous info

This package contains the WbWbXAnalysis Framework.

(For the impatient user: A doxygen documentation can be created with WbWbXAnalysis/Doxygen/create_docu.sh <optional output directory>)


The structure is as follows:

WbWbXAnalysis/Tools

This is the most generic part of the Framework. It incorporates the key classes and is kept (and should be kept) completely independent of any specific physics analysis. As core, it contains several root-histogram replacement classes. Their key feature is handling of systematic uncertainties and coherent treatment of correlations when operations are performed on them. Amongst these classes there are simple histograms (histo1D/2D), but also histograms for 1D unfolding (histo1DUnfold). Histogram stacks (histoStack) summarize these for several possible contributions. Plotting is performed with plotter classes that configured by style files that are read in by the plotters. The necessary tools for unfolding are also included (based on TUnfold) and directly interfaced to the histogram classes, therefore handling uncertainties automatically. These histograms or graphs can be fitted using fitting classes, or directly converted to templates as functions of all systematic variations (variateHisto1D).
Other small tools and classes are also located here, e.g. to parse options on command line, to read files with several formats, to format text or to easily fill LaTeX tables from C++ etc.
This sub-packge should not depend on the other sub-packages

WbWbXAnalysis/Analysis

Here, more specific analysis tools are located and the ntuple based analysis should take place here. A generic analyser class "MainAnalyzer" implements configuration file read-in, parallelization of jobs, monitoring of child processes and managing of output files. Inherit from this class for analysis specific configurations.
Several scale factor classes, and analysis specific classes are placed here.

WbWbXAnalysis/Efficiencies

Code to determine trigger and lepton efficiencies in MC and data.
WbWbXAnalysis /DataFormats
Classes to create objects on ntuple level and helpers to use them in the analysis (e.g. to construct mother-daughter relations, or easily apply cuts on collections in a performant way). These objects are accessible in an interactive root session. This package also contains wrappers that interface the data formats to other tools, e.g. located in the TopAnalysis /ZTopUtils package shared with the

WbWbXAnalysis/TreeWriter

Package that should only contain the tree writer to fill the ntuple

WbWbXAnalysis /SimpleCounter

Very minimalistic tool for CMSSW config file level. Could be merged with TreeWriter

WbWbXAnalysis /Workarounds

Workarounds for some mis-coded CMSSW functions on python-config file level.

WbWbXAnalysis /createNtuple

The CMS configuration file to create the ntuple and configuration files are located here. Furthermore, scripts to easily run jobs can be found in the subpackage. Json files also go here.

WbWbXAnalysis /plugins

mostly obsolete

WbWbXAnalysis /EclipseTools

Tools for Eclipse

WbWbXAnalysis /Configuration

Sample _cff files (file lists to run on) for testing only. The corresponding file lists to create ntuples are created dynamically based on CMS-DBS.

WbWbXAnalysis /Data

Historically data files were located here, and some still are. New files should go in the subpackage they are used in.tbipackage.



WbWbXAnalysis /Obsolete

Place to put classes/code that is not used anymore or not used yet (with an unspecified date of implementation).
If the code put here is indeed obsolete, it has to have some parts that might be usable in the future.
Don't just trash here!
The rest of the code MUST NOT have any dependence to this package.



For details, see
https://twiki.cern.ch/twiki/bin/viewauth/CMS/DesyTtZAnalysisFramework
