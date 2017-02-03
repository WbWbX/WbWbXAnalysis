import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import sys 
import os 

### parsing job options 

options = VarParsing.VarParsing()#'standard')


options.register('runOnAOD', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "run on AOD")
options.register('runOnMC',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"run on MC")
options.register('channel', 'emu', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which final state")
options.register('dataset', 'emu',VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which data stream")
options.register('isSignal',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is SignalMC")
options.register('includetrigger',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"include trigger info for all paths")
options.register('physProcess','top',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"which signal process")
options.register('maxEvents',-1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('inputScript','TopAnalysis.Configuration.MC.Spring15.miniAODv2.TT_TuneCUETP8M1_13TeV_powheg_pythia8_RunIISpring15MiniAODv2_74X_mcRun2_asymptotic_v2_ext3_v1_and_v1_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
#options.register('inputScript', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "python file with input source")
options.register('json','nojson',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"json files")
options.register('outputFile','def_out',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register('globalTag','',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"global tag")
options.register('genFilter','none',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"gen Filter")
options.register('genFilterString','none',VarParsing.VarParsing.multiplicity.list,VarParsing.VarParsing.varType.string,"gen Filter selection string")
options.register('genFilterMassLow',0,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"gen Filter mass range low")
options.register('genFilterMassHigh',0,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"gen Filter mass range high")
options.register('genFilterInvert',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"invert gen Filter")
options.register('includePDFWeights',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes pdf weights info for event")
options.register('PDF','CT10',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")
options.register('nominalPDF','1001',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"weight index of nominal pdf")
options.register('beginPDFVar','2001',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"first weight index for pdf variation")
options.register('endPDFVar','2052',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"last weight index for pdf variation")
options.register('reportEvery',1000,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"report every")
options.register('wantSummary',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"prints trigger summary")
options.register('debug',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"enters debug mode")
options.register('partonShower','pythia8',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,'parton shower used in MC')


print '\nStart generating ntuple\n***********************\n'


print 'Reading command line \n'

if hasattr(sys, "argv"):
    options.parseArguments()

runOnAOD=options.runOnAOD
if runOnAOD:
    print 'Configuration for data type: AOD'
else:
    print 'Configuration for data type: miniAOD'
runOnMC=options.runOnMC                     # True
isSignal=options.isSignal
if not runOnMC:
    isSignal=False     
dataset=options.dataset                     # emu
globalTag=options.globalTag                 # START53_V11
reportEvery=options.reportEvery             # 1000
maxEvents=options.maxEvents                 # -1. overwrites the one specified in input script. be careful!!
skipEvents=options.skipEvents
outputFile=options.outputFile               # def_out
genFilter=options.genFilter                 # 'none'
genFilterString=options.genFilterString     # 'none'
genFilterInvert=options.genFilterInvert     # False
includePDFWeights=options.includePDFWeights # False
PDF=options.PDF                             # CT10
nominalPDF=options.nominalPDF
beginPDFVar=options.beginPDFVar
endPDFVar=options.endPDFVar
inputScript=options.inputScript             # TtZAnalysis.Configuration.samples.mc.DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_Summer12-PU_S7_START52_V9-v2_cff
print 'Using input script: ', inputScript
json=options.json                           # parse absolute paths here!
print 'Using json path: ', json
wantSummary=options.wantSummary
debug=options.debug
useBHadrons=False                           #will be changes for top filter sequence!
partonShower= options.partonShower


print '\nDone with reading command line!\n'

minleptons=2

print 'Minimum 2 leptons required!'


# if sets are not in official paths
if includePDFWeights:
    newlha = os.environ['CMSSW_BASE']+'/src/TtZAnalysis/Data/data/PDFSets'


###############################################################
## Set Options for physics Objects
import TopAnalysis.Configuration.objectDefinitions13Tev.definitionOptions_cff as opt

pfpostfix = "PFlow"

# All Options are saved as dicts and have to be added in the definitionsOptions as empty dicts
opt.globalOptions = {'runOnAod':options.runOnAOD,
                 'signal':isSignal,
                 'isMC':runOnMC}
opt.primaryVertexOptions = {'inputCollectionAod':'offlinePrimaryVertices',  # Input Collection for AOD
                 'inputCollectionMiniAod':'offlineSlimmedPrimaryVertices', # Input Collection for MiniAOD
                 'outputCollection':''}       # The Collection which is Produced, is set in the apropriate cff File
opt.muonOptions={'inputCollectionAod':'selectedPatMuons'+pfpostfix,
                 'inputCollectionMiniAod': 'slimmedMuons',
                 'outputCollection':'',
                 'Cuts':False} #Only very loose cuts
opt.electronOptions={'inputCollectionAod':'selectedPatElectrons'+pfpostfix,
                 'inputCollectionMiniAod':'slimmedElectrons',
                 'outputCollection':'',
                 'idName':'Spring15-medium', # Name of the Id in the IdVector (on the Electrons)
                 'Cuts':False,
                 'MVA':False}

####################################################################
## Define the process

process = cms.Process("Yield")
process.options = cms.untracked.PSet( 
    wantSummary = cms.untracked.bool(wantSummary),
    allowUnscheduled = cms.untracked.bool(True) 
    )


####################################################################
## Define input 

if inputScript!='':
    process.load(inputScript)
else:
    print 'need an input script'
    exit(8889)

process.maxEvents = cms.untracked.PSet( 
    input = cms.untracked.int32 (maxEvents) 
)

if skipEvents > 0:
    process.source.skipEvents = cms.untracked.uint32(skipEvents)

if not runOnMC and not (json=="nojson"):
    import FWCore.PythonUtilities.LumiList as LumiList
    import FWCore.ParameterSet.Types as CfgTypes
    myLumis = LumiList.LumiList(filename = json).getCMSSWString().split(',')
    process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
    process.source.lumisToProcess.extend(myLumis)


####################################################################
## Configure message logger           


process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.threshold = 'INFO'
#process.MessageLogger.suppressWarning=['particleFlowDisplacedVertexCandidate','The interpolated laser correction is <= zero! (0). Using 1. as correction factor.']
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery


####################################################################
### Geometry and Detector Conditions  


process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration.StandardSequences.MagneticField_cff') # contains https://cmssdt.cern.ch/SDT/lxr/source/Configuration/StandardSequences/python/MagneticField_cff.py MagneticField_38T_cff

if globalTag != '':
    print "Setting global tag to the command-line value"
    process.GlobalTag.globaltag = cms.string(globalTag  )
else:
    print "Determine global tag automatically"
    if options.runOnMC:
        process.GlobalTag.globaltag = cms.string('80X_mcRun2_asymptotic_2016_TrancheIV_v7')
    else:
        process.GlobalTag.globaltag = cms.string('80X_dataRun2_2016SeptRepro_v6')
        
print "Using global tag: ", process.GlobalTag.globaltag


#jecFile=os.path.relpath( os.environ['CMSSW_BASE']+'/src/TtZAnalysis/Data/Run2/PY8_RunIISpring15DR74_bx50_MC.db')
#from CondCore.DBCommon.CondDBSetup_cfi import *
#process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
#    connect = cms.string('sqlite_file:'+jecFile),
#    toGet =  cms.VPSet(
#    cms.PSet(record = cms.string("JetCorrectionsRecord"),
#        tag = cms.string("JetCorrectorParametersCollection_PY8_RunIISpring15DR74_bx50_MC_AK4PF"),
#        Label = cms.untracked.string("AK4PF")),
#        cms.PSet(record = cms.string("JetCorrectionsRecord"),
#        tag =cms.string("JetCorrectorParametersCollection_PY8_RunIISpring15DR74_bx50_MC_AK4PFchs"),
#        Label = cms.untracked.string("AK4PFchs"))
#    )
#)
#process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")



####################################################################
## Configure TFileService    


outFileName = outputFile + '.root'
print 'Using output file ' + outFileName

process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string(outFileName))


####################################################################
## Weights 


## agrohsje mc systematic weights 
process.load("TopAnalysis.ZTopUtils.EventWeightMCSystematic_cfi")
process.nominal = cms.EDProducer("EventWeightMCSystematic",
                                 genEventInfoTag=cms.InputTag("generator"),
                                 lheEventInfoTag=cms.InputTag("externalLHEProducer"),
                                 weightID=cms.string("0000"), 
                                 printLHE=cms.bool(False)
                                 )
process.scaleUp = cms.EDProducer("EventWeightMCSystematic",
                                 genEventInfoTag=cms.InputTag("generator"),
                                 lheEventInfoTag=cms.InputTag("externalLHEProducer"),
                                 weightID=cms.string("1005"), 
                                 printLHE=cms.bool(False)                                 
                                 )
process.scaleDown = cms.EDProducer("EventWeightMCSystematic",
                                   genEventInfoTag=cms.InputTag("generator"),
                                   lheEventInfoTag=cms.InputTag("externalLHEProducer"),
                                   weightID=cms.string("1009"), 
                                   printLHE=cms.bool(False)
                                   )


if isSignal:=
    additionalWeights = cms.vstring("nominal","scaleUp","scaleDown")
elif runOnMC:
    additionalWeights = cms.vstring("nominal")
else: 
    additionalWeights=cms.vstring()



## PDF weights if required 
if includePDFWeights:
    process.pdfWeights = cms.EDProducer("EventWeightPDF",
                                        lheEventInfoTag=cms.InputTag("externalLHEProducer"),
                                        PDFName=cms.vstring(PDF),
                                        nominalWeightID=cms.vstring(nominalPDF),
                                        beginWeightID=cms.vstring(beginPDFVar),
                                        endWeightID=cms.vstring(endPDFVar),
                                        printLHE=cms.bool(False)
                                        )
#cms.EDProducer("PdfWeightProducer",
#PdfInfoTag = cms.untracked.InputTag("generator"),
#PdfSetNames = cms.untracked.vstring(PDF+".LHgrid"))

## Infos for proper normalization
process.load('TtZAnalysis.TreeWriter.puinfo_cff')

process.PUInfo.includePDFWeights = includePDFWeights
process.PUInfo.pdfWeights = "pdfWeights:"+PDF
if runOnAOD: 
    process.PUInfo.vertexSrc='offlinePrimaryVertices'
else:
    process.PUInfo.vertexSrc='offlineSlimmedPrimaryVertices'
    
process.preCutPUInfo = process.PUInfo.clone()
process.preCutPUInfo.treeName = 'preCutPUInfo'

process.postCutPUInfo = process.PUInfo.clone()
process.postCutPUInfo.treeName = 'PUTreePostCut'
process.postCutPUInfo.includePDFWeights = False


    
#################################################################### 
## Generator-level objects 
    

## Define names     
genParticleCollection = ''
genJetInputParticleCollection = ''

if runOnAOD:
    genParticleCollection = 'genParticles'
    genJetInputParticleCollection = 'genParticles'
else:
    genParticleCollection = 'prunedGenParticles'
    genJetInputParticleCollection = 'packedGenParticles'

genJetCollection = 'ak4GenJetsNoNuNoLepton'
genJetFlavourInfoCollection = 'ak4GenJetFlavourPlusLeptonInfos'

## Create objects 
## Details in: PhysicsTools/JetExamples/test/printJetFlavourInfo.cc, PhysicsTools/JetExamples/test/printJetFlavourInfo.py
## and in: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools#New_jet_flavour_definition  

## Supply PDG ID to real name resolution of MC particles
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")

## Input particles for gen jets (stable gen particles to be used in clustering, excluding electrons, muons and neutrinos from hard interaction)
from RecoJets.Configuration.GenJetParticles_cff import genParticlesForJets
process.genParticlesForJetsNoNuNoLepton = genParticlesForJets.clone(
    src = genJetInputParticleCollection,
    excludeResonances = True,
    excludeFromResonancePids = [11, 12, 13, 14, 16],
)

from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
process.ak4GenJetsNoNuNoLepton = ak4GenJets.clone(src = "genParticlesForJetsNoNuNoLepton")

# Ghost particle collection for matching to gen jets (b/c hadrons + leptons)
from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(particles = genParticleCollection)

# Flavour info: jet collection with all associated ghosts
# For the moment leptons need to be specified explicitely here, until lepton access can be made more generic in miniAOD
# This is only needed as long as the jetConstituents are not accessible directly in miniAOD, then it should be fixed
# by using the leptons from the constituents, instead of feeding them as ghosts into the jets 

from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos
process.ak4GenJetFlavourPlusLeptonInfos = ak4JetFlavourInfos.clone(
    jets = genJetCollection,
    leptons = cms.InputTag("selectedHadronsAndPartons", "leptons")
)


####################################################################
## Generator-level selection 


    
####################################################################
## Prefilter sequence

if runOnMC :
    filterProcess='PAT'
else : 
    filterProcess='RECO'

from TopAnalysis.ZTopUtils.NoiseFilter_cfi import noiseFilter
process.cscFilter=noiseFilter.clone(src = cms.InputTag('TriggerResults','',filterProcess),flag=cms.string("Flag_globalTightHalo2016Filter"))
process.vertexFilter=noiseFilter.clone(src = cms.InputTag('TriggerResults','',filterProcess),flag=cms.string("Flag_goodVertices"))
process.badSCFilter=noiseFilter.clone(src=cms.InputTag('TriggerResults','',filterProcess),flag=cms.string("Flag_eeBadScFilter"))
process.HBHENoiseFilter=noiseFilter.clone(src=cms.InputTag('TriggerResults','',filterProcess),flag=cms.string("Flag_HBHENoiseFilter"))
process.HBHENoiseIsoFilter=noiseFilter.clone(src=cms.InputTag('TriggerResults','',filterProcess),flag=cms.string("Flag_HBHENoiseIsoFilter"))
process.ecalDeadCellFilter=noiseFilter.clone(src=cms.InputTag('TriggerResults','',filterProcess),flag=cms.string("Flag_EcalDeadCellTriggerPrimitiveFilter"))

process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")

process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")



if runOnMC :
     process.prefilterSequence = cms.Sequence(
         process.HBHENoiseFilter*
         process.HBHENoiseIsoFilter*
         process.cscFilter*
         process.vertexFilter*
         process.ecalDeadCellFilter*
         process.BadChargedCandidateFilter*
         process.BadPFMuonFilter
     )


else:
     process.prefilterSequence=cms.Sequence(
         process.HBHENoiseFilter* 
         process.HBHENoiseIsoFilter*    
         process.cscFilter*
         process.vertexFilter*
         process.badSCFilter*
         process.ecalDeadCellFilter*
         process.BadChargedCandidateFilter*
         process.BadPFMuonFilter
     )

####################################################################
## Primary vertex filtering
process.load('TopAnalysis.Configuration.objectDefinitions13Tev.primaryVertex_cff')

    
####################################################################
## Full configuration for PF2PAT 


if runOnMC:
    jetCorr =('AK4PFchs', ['L1FastJet','L2Relative','L3Absolute'], 'None')
else:
    jetCorr = ('AK4PFchs', ['L1FastJet','L2Relative','L3Absolute', 'L2L3Residual'])


#default values for MiniAODs 
muonTag = 'slimmedMuons' 
metTag = 'slimmedMETs'
#redo jets to adjust b-tagging/JEC info  
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
process.pfCHS = cms.EDFilter("CandPtrSelector", 
                             src = cms.InputTag("packedPFCandidates"), 
                             cut = cms.string("fromPV"))
process.ak4PFJetsCHS = ak4PFJets.clone(src = 'pfCHS', doAreaFastjet = True)

###########################################################################
# Electron IDs are calculated in a function
import TopAnalysis.Configuration.objectDefinitions13Tev.electron_cff as electron
electron.electron(process)

#################################################################### 
## Use SwitchJetCollection in order to rerun the btagging


## b-tag infos
#bTagInfos = ['impactParameterTagInfos','secondaryVertexTagInfos']
## b-tag discriminators
bTagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags'
                      ]
## Jets, tracks, and vertices 
jetSource = 'ak4PFJetsCHS' 
pfCandidates = 'packedPFCandidates'
pvSource = opt.primaryVertexOptions['outputCollection']
svSource = cms.InputTag('slimmedSecondaryVertices')
electronSource=cms.InputTag(opt.electronOptions['inputCollectionMiniAod'])
muonSource=cms.InputTag(opt.muonOptions['inputCollectionMiniAod'])
    
####################################################################
## Lepton information and dilepton filter at reco level 

process.load('TopAnalysis.Configuration.objectDefinitions13Tev.muon_cff')

from PhysicsTools.PatAlgos.selectionLayer1.electronSelector_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import *

process.kinElectrons = selectedPatElectrons.clone(
    src = opt.electronOptions["outputCollection"],
    cut = 'pt > 8  && abs(eta) < 2.7' # because of ECalP4 to be on the safe side
    )        

process.muonEleMerge = cms.EDProducer("CandViewMerger",
                                     src = cms.VInputTag(cms.InputTag("kinElectrons"),cms.InputTag(opt.muonOptions['outputCollection']                                           ))
                                     )

## filter on at least two opposite charge elecs/muons or emu
process.filterkinLeptons = cms.EDFilter("SimpleCounter",
                                        src = cms.VInputTag(cms.InputTag("muonEleMerge")), 
                                        requireOppoQ = cms.bool(True),
                                        minNumber = cms.uint32(minleptons),
                                        debug=cms.bool(debug)
                                        )

#W stuff
process.filterkinLeptons.src = cms.VInputTag(cms.InputTag(opt.muonOptions['outputCollection']))
process.filterkinLeptons.minNumber=cms.uint32(1)
                                        
if not runOnMC and dataset !='MET':
    process.kinLeptonFilterSequence = cms.Sequence(process.filterkinLeptons)
else: 
    process.kinLeptonFilterSequence = cms.Sequence()

####################################################################
## Jet information 

jetColl= "slimmedJets"

process.treeJets = selectedPatJets.clone( 
    src=jetColl,   #'patJets'+pfpostfix, 
    cut='eta < 5 && pt>5 &&neutralHadronEnergyFraction < 0.99 && neutralEmEnergyFraction < 0.99 && (chargedMultiplicity+neutralMultiplicity) > 1 && muonEnergyFraction < 0.8 && ((abs(eta)<2.4 && chargedHadronEnergyFraction > 0.0 && chargedMultiplicity > 0.0 && chargedEmEnergyFraction < 0.99) || abs(eta)>2.4)') # unfortunately starting at 10 GeV are needed for MET rescaling 8GeV should be ok as corrected pt 
    ### cut at uncorrected pt > 10 GeV on tree writer level! for MET rescaling - might be obsolete for 13 TeV (and when not using MET...)
    

#################################################################### 
## Configure ntuple 
process.PFTree   = cms.EDAnalyzer('TreeWriterTtZ',

                                  debugmode = cms.bool(debug),
                                  runOnAOD  = cms.bool(runOnAOD),
                                  #general input collections
                                  treeName = cms.string('pfTree'),
                                  muonSrc = cms.InputTag(muonTag),
                                  keepElecIdOnly = cms.string("Spring15-medium"),
                                  elecGSFSrc = cms.InputTag(opt.electronOptions["outputCollection"]), #just the same here to make it run. this can be prevented by a try{}catch(...){} in treewriter for getByLabel
                                  elecPFSrc = cms.InputTag(opt.electronOptions["outputCollection"]),
                                  jetSrc = cms.InputTag('treeJets'),  #jetTag), # ('treeJets'),
                                  btagAlgo = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'), ###combinedSecondaryVertexBJetTags'),
                                  metSrc = cms.InputTag(metTag),  #here also try, catch statements
                                  mvaMetSrc = cms.InputTag(metTag), 
                                  metT1Src   =cms.InputTag(metTag),
                                  metT0T1TxySrc        =cms.InputTag(metTag),
                                  metT0T1Src           =cms.InputTag(metTag),
                                  metT1TxySrc          =cms.InputTag(metTag),
                                  vertexSrc = cms.InputTag(pvSource ),
                                  
                                  #block for extended information needed for efficiency studies
                                  includeReco = cms.bool(False),
                                  recoMuonSrc = cms.InputTag('muons'),
                                  isPFMuonCand = cms.bool(False),
                                  recoElecSrc  = cms.InputTag('gsfElectrons'),
                                  isPFElecCand = cms.bool(False),
                                  recoTrackSrc = cms.InputTag('generalTracks'),
                                  recoSuCluSrc = cms.InputTag('superClusters'),
                                                                    
                                  #block for trigger information
                                  includeTrigger = cms.bool(options.includetrigger),
                                  triggerResults = cms.InputTag('TriggerResults','','HLT'), #needed for both AOD and MiniAOD
                                  triggerEvent   = cms.InputTag('patTriggerEvent'), ### "selectedPatTrigger" for MiniAODs
                                  triggerObjects = cms.vstring(""),
                                  #triggerObjects = cms.vstring("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v","hltL3fL1sDoubleMu114L1f0L2f10OneMuL3Filtered17","hltDiMuonGlb17Glb8RelTrkIsoFiltered0p4","hltDiMuonGlb17Trk8RelTrkIsoFiltered0p4","hltL3pfL1sDoubleMu114L1f0L2pf0L3PreFiltered8","hltL2pfL1sDoubleMu114ORDoubleMu125L1f0L2PreFiltered0","hltDiMuonGlb17Trk8RelTrkIsoFiltered0p4","hltDiMuonGlbFiltered17TrkFiltered8"),
                                  
                                  #block for event information.  Essential (PU)
                                  PUInfo = cms.InputTag('slimmedAddPileupInfo'),
                                  includePDFWeights = cms.bool(includePDFWeights),
                                  pdfWeights = cms.InputTag("pdfWeights:"+PDF),
                                  additionalWeights = additionalWeights, #can be any vector of strings, generated by EDProducer: EventWeightMCSystematic
                                  #agrohsje fix  
                                  rhoIso = cms.InputTag("fixedGridRhoFastjetAll"), #dummy for 13 TeV AOD
                                  
                                  includeGen = cms.bool(isSignal),
                                  genParticles = cms.InputTag(genParticleCollection),
                                  # agrohsje fix later 
                                  genJets = cms.InputTag("ak4GenJetsNoNuNoLepton"), 
                                  #cms.InputTag(""), # in miniaod:slimmedGenJets, in aod:ak4GenJets
                                  genMode = cms.string(options.physProcess), 

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

                                  #setting PartonShower to Pythia8
                                  partonShower = cms.string(partonShower),
                                  )


####################################################################
## Path (filter and analyzer only)

process.dump=cms.EDAnalyzer('EventContentAnalyzer')

#if options.physProcess=='W':
#    process.PFTree.includeTrigger = cms.bool(False)

process.path = cms.Path(
    process.regressionApplication* 
    process.preCutPUInfo*
    process.fsFilterSequence*
    process.postCutPUInfo*
#    process.goodOfflinePrimaryVertices *   
    process.prefilterSequence * 
    #    process.dump *
    process.kinLeptonFilterSequence * # agrohsje 
    process.PFTree
    )

#process.outpath    = cms.EndPath()


####################################################################
## Signal catcher for more information on errors and to resume jobs    

if options.physProcess!='W':
    process.load("TopAnalysis.TopUtils.SignalCatcher_cfi")


