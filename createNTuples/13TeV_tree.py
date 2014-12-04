import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

###parsing stuff###

options = VarParsing.VarParsing()

options.register ('globalTag','START53_V27',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"global tag")
options.register ('reportEvery',1000,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"report every")
options.register ('maxEvents',-1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register ('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register ('outputFile','def_out',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register ('isMC',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is MC")

options.register ('genFilter','none',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"gen Filter")
options.register ('genFilterString','none',VarParsing.VarParsing.multiplicity.list,VarParsing.VarParsing.varType.string,"gen Filter selection string")
options.register ('genFilterMassLow',0,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"gen Filter mass range low")
options.register ('genFilterMassHigh',0,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"gen Filter mass range high")
options.register ('genFilterInvert',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"invert gen Filter")

options.register ('includePDF',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes pdf weights info for event")
options.register ('PDF','cteq66',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")

options.register ('inputScript','TtZAnalysis.Configuration.samples.mc.TTJets_MassiveBinDECAY_TuneZ2star_8TeV_madgraph_tauola_Summer12_DR53X_PU_S10_START53_V7A_v1_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")

options.register ('json','nojson',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"json files")

options.register ('isSignal',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is SignalMC")

options.register ('wantSummary',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"prints trigger summary")

options.register ('debug',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"enters debug mode")


import sys


if hasattr(sys, "argv"):
    options.parseArguments()

globalTag=options.globalTag              # START53_V11
reportEvery=options.reportEvery          # 1000
maxEvents=options.maxEvents              # -1. overwrites the one specified in input script. be careful!!
outputFile=options.outputFile            # def_out
isMC=options.isMC                        # True
realdata = True                          # just alternative boolean
if isMC:
    realdata=False

genFilter=options.genFilter              # 'none'
genFilterString=options.genFilterString  # 'none'
genFilterInvert=options.genFilterInvert  # False

includePDFWeights=options.includePDF     # False
PDF=options.PDF                          # CT10
inputScript=options.inputScript          # TtZAnalysis.Configuration.samples.mc.DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_Summer12-PU_S7_START52_V9-v2_cff
is2011=False
if "_7TeV_" in outputFile:
    is2011=True
    print 'running on 7TeV'
if is2011:
    inputScript='TtZAnalysis.Configuration.samples.mc.TTJets_MSDecays_central_TuneZ2_7TeV-madgraph-tauola_Summer11LegDR_PU_S13_START53_LV6_v1_cff'
json=options.json                        # parse absolute paths here!
isSignal=options.isSignal
wantSummary=options.wantSummary
debug=options.debug
useBHadrons=False #will be changes for top filter sequence!

if not isMC:
    isSignal=False


#print genFilterString

print "\n\n"

electronIsoCone="03"
useGsf = True
minleptons=2

print 'minimum 2 leptons required. Check tree filling cuts for kinLeptons!! Even for includereco=true'

# if sets are not in official paths
if includePDFWeights:
    import os
    newlha = os.environ['CMSSW_BASE']+'/src/TtZAnalysis/Data/data/PDFSets'
    #os.environ['LHAPATH']=newlha



pfpostfix='PFlow' #unfortunately some things don't seem to work otherwise.. pfjets get lost somehow produces LOADS of overhead...

                          

##########
#########
########
######
####
## GENERAL CMSRUN CONFIGURATION: Input, output, message logger, json file
#

process = cms.Process("Yield")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')


#Options
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(wantSummary))
process.GlobalTag.globaltag = globalTag + '::All'




#Message Logger Stuff
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
#process.MessageLogger.suppressWarning=['particleFlowDisplacedVertexCandidate','The interpolated laser correction is <= zero! (0). Using 1. as correction factor.']
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery

#process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( inputFiles ), skipEvents=cms.untracked.uint32( skipEvents ) )
process.out    = cms.OutputModule("PoolOutputModule", outputCommands =  cms.untracked.vstring(), fileName = cms.untracked.string( outputFile + '_PatTuple') )

################# Input script (some default one. Will be overwritten, so has no effect but otherwise job submission (via crab) reports problem)

process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( '/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S10_START53_V7A-v1/0000/A89D210D-1BE2-E111-9EFB-0030487F1797.root' ))

if not inputScript=='':
    process.load(inputScript)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32 (maxEvents) )
if options.skipEvents > 0:
    process.source.skipEvents = cms.untracked.uint32(options.skipEvents)

#########JSON###
# limit to json file (if passed as parameter)

if realdata and not (options.json=="nojson"):
    import os
    jsonpath = options.json 
    print "using json:"
    print jsonpath
    import FWCore.PythonUtilities.LumiList as LumiList
    process.source.lumisToProcess = LumiList.LumiList(filename = jsonpath).getVLuminosityBlockRange()



#### TFile Service for writing output
outFileName = outputFile + '.root'
process.TFileService = cms.Service("TFileService", fileName = cms.string(outFileName))
                                 

##########
#########
########
######
####
## GENERATOR LEVEL SELECTION AND WEIGHTS
#
process.pdfWeights = cms.EDProducer("PdfWeightProducer",
                                    PdfInfoTag = cms.untracked.InputTag("generator"),
                                    PdfSetNames = cms.untracked.vstring(PDF+".LHgrid"))
                                    
########## infos for normalization. Essential for ntuple analysis!

process.load('TtZAnalysis.TreeWriter.puinfo_cff')

process.PUInfo.includePDFWeights = includePDFWeights
process.PUInfo.pdfWeights = "pdfWeights:"+PDF
process.PUInfo.vertexSrc='offlinePrimaryVertices'

process.preCutPUInfo = process.PUInfo.clone()
process.preCutPUInfo.treeName = 'preCutPUInfo'

process.postCutPUInfo = process.PUInfo.clone()
process.postCutPUInfo.treeName = 'PUTreePostCut'
process.postCutPUInfo.includePDFWeights = False


process.preFilterSequence = cms.Sequence(process.preCutPUInfo)

### generator filters
# empty for now
# configure some filter...
# and add to preFilterSequence:
#process.preFilterSequence += process.somefilter

# add post generator cut infos (essential)
process.preFilterSequence += process.postCutPUInfo

    
##########
#########
########
######
####
## RECO LEVEL SELECTION
#      filters

# switch off reco level filters for signal
filtervertices=not isSignal

process.filtersSeq = cms.Sequence()

## one of the "standard" filters
process.noscraping = cms.EDFilter(
    "FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
    )

process.filtersSeq += process.noscraping

process.goodOfflinePrimaryVertices = cms.EDFilter( "PrimaryVertexObjectFilter" , 
                                                   filterParams = cms.PSet( minNdof = cms.double( 4. ) , 
                                                                            maxZ = cms.double( 24. ) , 
                                                                            maxRho = cms.double( 2. ) ) , 
                                                   filter = cms.bool( filtervertices) , 
                                                   src = cms.InputTag( 'offlinePrimaryVertices' ) )

process.filtersSeq += process.goodOfflinePrimaryVertices

    
##########
#########
########
######
####
## RECO LEVEL SELECTION
#      main sequence to create collections

# load default sequence
process.load("PhysicsTools.PatAlgos.patSequences_cff")

if isMC:
    jetCorr =('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute'])
else:
    jetCorr = ('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute', 'L2L3Residual'])

# add particle flow
from PhysicsTools.PatAlgos.tools.pfTools import *
usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=isMC, postfix=pfpostfix, jetCorrections=jetCorr, pvCollection=cms.InputTag('goodOfflinePrimaryVertices'),typeIMetCorrections=True) 


# switch off all top projections

getattr(process,'pfNoMuon'+pfpostfix).enable = False
getattr(process,'pfNoElectron'+pfpostfix).enable = False
getattr(process,'pfNoJet'+pfpostfix).enable = False
getattr(process,'pfNoTau'+pfpostfix).enable = False

# don't require isolation on cmsRun level
getattr(process,'pfIsolatedElectrons'+pfpostfix).isolationCut = 99999 
getattr(process,'pfIsolatedMuons'+pfpostfix).isolationCut = 99999 


 
##########
#########
########
######
####
## PREPARE OBJECTS FOR NTUPLE 
#      additionally: some loose selection on event

process.treeJets = process.selectedPatJets.clone()
process.treeJets.src="patJets"+pfpostfix
process.treeJets.cut = 'eta < 5 && pt>5' # unfortunately starting at 10 GeV are needed for MET rescaling 8GeV should be ok as corrected pt
### cut at uncorrected pt > 10 GeV on tree writer level! for MET rescaling - might be obsolete for 13 TeV (and when not using MET...)



process.kinMuons = process.selectedPatMuons.clone()
process.kinMuons.src = 'patMuons' + pfpostfix
process.kinMuons.cut = cms.string('pt > 8  && abs(eta) < 2.7')

process.kinElectrons = process.selectedPatElectrons.clone()
process.kinElectrons.src = 'patElectrons' + pfpostfix
process.kinElectrons.cut = cms.string( 'pt > 8  && abs(eta) < 2.7') # because of ECalP4 to be on the safe side


# don't let the ntuple files become tooo large, so filter on at least two oppo q elecs/muons or emu
# "SimpleCounter" is part of the TtZAnalysis framework! Might need to be modified but probably not
process.filterkinLeptons = cms.EDFilter("SimpleCounter",
                                        src = cms.VInputTag(cms.InputTag("kinMuons"),  
                                                            cms.InputTag("kinElectrons")), 
                                        requireOppoQ = cms.bool(True),
                                        minNumber = cms.uint32(minleptons),
                                        debug=cms.bool(debug)
                                        )

process.kinLeptonFilterSequence = cms.Sequence(process.kinMuons *
                                               process.kinElectrons *
                                               process.filterkinLeptons)

# insert filter sequence in path only if not signal
if not isSignal:
    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                           getattr(process,'patMuons'+pfpostfix) *
                                                           process.kinLeptonFilterSequence)

    

##########
#########
########
######
####
## ADD PAT TRIGGER.  not only for trigger studies but to write trigger info to ntuple
#     


from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTrigger.onlyStandAlone = False


##########
#########
########
######
####
## CONFIGURE NTUPLE INPUTS
#     


# for visibility, it is now done on config-file level
# the default configuration is here:
##process.load('TtZAnalysis.TreeWriter.treewriter_ttz_cff')

process.PFTree   = cms.EDAnalyzer('TreeWriterTtZ',

                          debugmode = cms.bool(debug),

                          #general input collections
                          treeName = cms.string('pfTree'),
                          muonSrc = cms.InputTag('patMuons' + pfpostfix),
                          elecGSFSrc = cms.InputTag('patElectrons' + pfpostfix), #just the same here to make it run. this can be prevented by a try{}catch(...){} in treewriter for getByLabel
                          elecPFSrc = cms.InputTag('patElectrons' + pfpostfix),
                          jetSrc = cms.InputTag('treeJets'),
                          btagAlgo = cms.string('combinedSecondaryVertexBJetTags'),
                          metSrc = cms.InputTag('patMETs'+pfpostfix),  #here also try, catch statements
                          mvaMetSrc = cms.InputTag('patMETs'+pfpostfix), 
                          metT1Src   =cms.InputTag('patMETs'+pfpostfix),
                          metT0T1TxySrc        =cms.InputTag('patMETs'+pfpostfix),
                          metT0T1Src           =cms.InputTag('patMETs'+pfpostfix),
                          metT1TxySrc          =cms.InputTag('patMETs'+pfpostfix),
                          vertexSrc = cms.InputTag('goodOfflinePrimaryVertices'),
                          
                          #block for extended information needed for efficiency studies
                          includeReco = cms.bool(False),
                          recoMuonSrc = cms.InputTag('muons'),
                          isPFMuonCand = cms.bool(False),
                          recoElecSrc  = cms.InputTag('gsfElectrons'),
                          isPFElecCand = cms.bool(False),
                          recoTrackSrc = cms.InputTag('generalTracks'),
                          recoSuCluSrc = cms.InputTag('superClusters'),
                          
                                                    
                          #block for trigger information
                          includeTrigger = cms.bool(False),
                          triggerResults = cms.InputTag('TriggerResults','','HLT'),
                          triggerEvent = cms.InputTag('patTriggerEvent'),
                          triggerObjects = cms.vstring(""),
                          
                          #block for event information.  Essential (PU)
                          PUInfo = cms.InputTag('addPileupInfo'),
                          includePDFWeights = cms.bool(includePDFWeights),
                          pdfWeights = cms.InputTag(''),
                          additionalWeights = cms.vstring(""), #can be any vectors of doubles
                          rhoIso = cms.InputTag("kt6PFJets","rho", "RECO"), #that one can probably go

                          includeGen = cms.bool(isSignal),
                          genParticles = cms.InputTag('genParticles'),
                          genJets = cms.InputTag('ak5GenJetsNoNu'),


                          ##improved jet-hadron matching block. Not implemented in the configuration above!
                          useBHadrons = cms.bool(False),
                          BHadronVsJet = cms.InputTag(""),
                          AntiBHadronVsJet = cms.InputTag(""),
                          BJetIndex     = cms.InputTag(""),
                          AntiBJetIndex = cms.InputTag(""),
                          BHadJetIndex = cms.InputTag(""),
                          AntiBHadJetIndex = cms.InputTag(""),
                          BHadrons = cms.InputTag(""),
                          AntiBHadrons = cms.InputTag(""),
                          BHadronFromTopB = cms.InputTag(""),
                          AntiBHadronFromTopB = cms.InputTag(""),
                          genBHadPlusMothers = cms.InputTag(""),
                          genBHadPlusMothersIndices = cms.InputTag(""),
                          genBHadIndex = cms.InputTag(""),
                          genBHadFlavour = cms.InputTag(""),
                          genBHadJetIndex = cms.InputTag(""),

                          #old switches, can be removed at some point
                          isSusy =  cms.bool(False),
                          isJPsi = cms.bool(False),

                          
                          )




## nice tool. It prints all collections that are available at a certain step. 
#  Here, the most important one is the step before filling the ntuple. Just uncomment
#  from the sequence if needed.
process.dump=cms.EDAnalyzer('EventContentAnalyzer')
## make tree sequence
process.treeSequence = cms.Sequence(process.patTrigger *
                                    process.patTriggerEvent *
                                    process.treeJets *
                                    #process.dump *
                                    process.PFTree)




##########
#########
########
######
####
## DEFINE PATH TO RUN (finally)
#     


process.path = cms.Path( 
    process.preFilterSequence *
    process.goodOfflinePrimaryVertices *
    process.filtersSeq *
    getattr(process,'patPF2PATSequence'+pfpostfix) *
    process.treeSequence
    )


process.outpath    = cms.EndPath()


