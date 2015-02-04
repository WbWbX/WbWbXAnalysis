import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import sys 
import os 

### parsing job options 

options = VarParsing.VarParsing()#'standard')

options.register('runOnAOD', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "run on AOD")
options.register('runOnMC',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"run on MC")
options.register('channel', 'emu', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which final state")
options.register('isSignal',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is SignalMC")
options.register('maxEvents',-1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('inputScript','TopAnalysis.Configuration.Phys14.miniAOD.TT_Tune4C_13TeV_pythia8_tauola_Phys14DR_PU20bx25_tsg_PHYS14_25_V1_v1_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
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
options.register('PDF','cteq66',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")
options.register('reportEvery',1000,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"report every")
options.register('wantSummary',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"prints trigger summary")
options.register('debug',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"enters debug mode")

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
inputScript=options.inputScript             # TtZAnalysis.Configuration.samples.mc.DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_Summer12-PU_S7_START52_V9-v2_cff
print 'Using input script: ', inputScript
json=options.json                           # parse absolute paths here!
print 'Using json path: ', json
wantSummary=options.wantSummary
debug=options.debug
useBHadrons=False                           #will be changes for top filter sequence!

print '\nDone with reading command line!\n'

minleptons=2

print 'Minimum 2 leptons required!'


# if sets are not in official paths
if includePDFWeights:
    newlha = os.environ['CMSSW_BASE']+'/src/TtZAnalysis/Data/data/PDFSets'
    #os.environ['LHAPATH']=newlha





###############################################################
## Set Options for physics Objects
import TopAnalysis.Configuration.objectDefinitions13Tev.definitionOptions_cff as opt
# All Options are saved as dicts and have to be added in the definitionsOptions as empty dicts
opt.globalOptions = {'runOnAod':options.runOnAOD,
                 'signal':isSignal}
opt.primaryVertexOptions = {'inputCollectionAod':'offlinePrimaryVertices',  # Input Collection for AOD
                 'inputCollectionMiniAod':'offlineSlimmedPrimaryVertices', # Input Collection for MiniAOD
                 'outputCollection':''}       # The Collection which is Produced, is set in the apropriate cff File


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
### define output (agrohsje: check why needed) 

if runOnAOD:
    process.out = cms.OutputModule("PoolOutputModule", 
                                   outputCommands =  cms.untracked.vstring(), 
                                   fileName = cms.untracked.string(outputFile+'_PatTuple'))

####################################################################
### Geometry and Detector Conditions  


process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.MagneticField_cff') # contains https://cmssdt.cern.ch/SDT/lxr/source/Configuration/StandardSequences/python/MagneticField_cff.py MagneticField_38T_cff

if globalTag != '':
    print "Setting global tag to the command-line value"
    process.GlobalTag.globaltag = cms.string(globalTag + '::All')
else:
    print "Determine global tag automatically"
    if options.runOnMC:
        process.GlobalTag.globaltag = cms.string('PHYS14_25_V2::All')
        #agrohsje process.GlobalTag.globaltag = cms.string('PHYS14_50_V2::All') 
    else:
        process.GlobalTag.globaltag = cms.string('FT53_V21A_AN6::All')
        
print "Using global tag: ", process.GlobalTag.globaltag


####################################################################
## Configure TFileService    


outFileName = outputFile + '.root'
print 'Using output file ' + outFileName

process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string(outFileName))


####################################################################
## Weights 


## agrohsje test mc systematic weights 
##process.load("TopAnalysis.ZTopUtils.EventWeightMCSystematic_cfi")
#process.scaleUp = cms.EDProducer("EventWeightMCSystematic",
#                                 weightId=cms.string("1001")
#                                 )

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

## add PDF weights if required 
if includePDFWeights:
    process.pdfWeights = cms.EDProducer("PdfWeightProducer",
                                        PdfInfoTag = cms.untracked.InputTag("generator"),
                                        PdfSetNames = cms.untracked.vstring(PDF+".LHgrid"))
    
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

#agrohsje todo: add gen-level filters  
process.fsFilterSequence = cms.Sequence()
    
####################################################################
## Prefilter sequence


if runOnMC:
    process.prefilterSequence = cms.Sequence()
else:
    ## HCAL noise filter
    process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')
    process.HBHENoiseFilter.minIsolatedNoiseSumE = cms.double(999999.)
    process.HBHENoiseFilter.minNumIsolatedNoiseChannels = cms.int32(999999)
    process.HBHENoiseFilter.minIsolatedNoiseSumEt = cms.double(999999.)
    ## Beam scraping filter 
    process.scrapingFilter = cms.EDFilter("FilterOutScraping",
                                          applyfilter = cms.untracked.bool(True),
                                          debugOn = cms.untracked.bool(False),
                                          numtrack = cms.untracked.uint32(10),
                                          thresh = cms.untracked.double(0.25)
                                          )
    ## ECAL laser correction filter 
    process.load("RecoMET.METFilters.ecalLaserCorrFilter_cfi")
    
    
    process.prefilterSequence = cms.Sequence(
        process.HBHENoiseFilter *
        process.scrapingFilter *
        process.ecalLaserCorrFilter
        )


####################################################################
## Primary vertex filtering
process.load('TopAnalysis.Configuration.objectDefinitions13Tev.primaryVertex_cff')


#################################################################### 
## Trigger information  


if runOnAOD:    
    from PhysicsTools.PatAlgos.tools.trigTools import * 
    switchOnTrigger( process ) 
    process.patTrigger.onlyStandAlone = False
    process.triggerSequence = cms.Sequence(process.patTrigger*
                                           process.patTriggerEvent)
else:
    process.triggerSequence = cms.Sequence()
    
    
####################################################################
## Full configuration for PF2PAT 

pfpostfix = "PFlow"

if runOnMC:
    jetCorr =('AK4PFchs', ['L1FastJet','L2Relative','L3Absolute'], 'None')
else:
    jetCorr = ('AK4PFchs', ['L1FastJet','L2Relative','L3Absolute', 'L2L3Residual'])

#process.userPatSequence = cms.Sequence() ;

## Create and define reco objects 
if runOnAOD:
    electronTag = 'patElectrons' + pfpostfix  
    muonTag = 'patMuons' + pfpostfix  
    metTag = 'patMETs' + pfpostfix  
    
    ## Output module for edm files (needed for PAT sequence, even if not used in EndPath) 
    from Configuration.EventContent.EventContent_cff import FEVTEventContent
    process.out = cms.OutputModule("PoolOutputModule",
                                   FEVTEventContent,
                                   dataset = cms.untracked.PSet(dataTier = cms.untracked.string('RECO')),
                                   fileName = cms.untracked.string("eh.root"),
                                   )
    # add particle flow
    process.load("PhysicsTools.PatAlgos.patSequences_cff")
    from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
    usePF2PAT(process, runPF2PAT=True, jetAlgo='AK4', runOnMC=runOnMC, postfix=pfpostfix, 
              jetCorrections=jetCorr, pvCollection=cms.InputTag(opt.primaryVertexOptions['outputCollection']),typeIMetCorrections=True) 

    getattr(process, 'pfPileUp'+pfpostfix).checkClosestZVertex = False
    
    # switch off all top projections
    getattr(process,'pfNoMuon'+pfpostfix).enable = False
    getattr(process,'pfNoElectron'+pfpostfix).enable = False
    getattr(process,'pfNoJet'+pfpostfix).enable = False
    getattr(process,'pfNoTau'+pfpostfix).enable = False
    # agrohsje don't require isolation on cmsRun level !!! 
    # seems not to work -> check with python -i ?
    getattr(process,'pfIsolatedElectrons'+pfpostfix).isolationCut = cms.double(999999.) 
    getattr(process,'pfIsolatedMuons'+pfpostfix).isolationCut = cms.double(999999.)


else :
    #default values for MiniAODs 
    electronTag = 'slimmedElectrons' 
    muonTag = 'slimmedMuons' 
    metTag = 'slimmedMETs'

    #redo jets to adjust b-tagging/JEC info  
    process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')
    from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
    from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
    process.pfCHS = cms.EDFilter("CandPtrSelector", 
                                 src = cms.InputTag("packedPFCandidates"), 
                                 cut = cms.string("fromPV"))
    process.ak4PFJetsCHS = ak4PFJets.clone(src = 'pfCHS', doAreaFastjet = True)
    
    
####################### Rerun Electron IDs   ###############


# See: https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
# overwrite a default parameter: for miniAOD, the collection name is a slimmed one
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag(electronTag)
from PhysicsTools.SelectorUtils.centralIDRegistry import central_id_registry
process.egmGsfElectronIDSequence = cms.Sequence(process.egmGsfElectronIDs)
# Define which IDs we want to produce
# Each of these two example IDs contains all four standard
# cut-based ID working points (only two WP of the PU20bx25 are actually used here).
eIDInputTag=''
if not runOnAOD:
    eIDInputTag='miniAOD_'
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V1_'+eIDInputTag+'cff']
#Add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
process.load("TopAnalysis.TopUtils.ElectronIdProducer_cff")
process.ElectronIdProducer.electrons= cms.InputTag(electronTag)
# Name of the ID that is calculated
if not runOnAOD:
    eIDInputTag='miniAOD-'
process.ElectronIdProducer.electronIdMap = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V1-'+eIDInputTag+'standalone-tight')
# Name of the New Id in the Id Vector
keepElectronID='Phys14-tight'
process.ElectronIdProducer.idName = cms.string(keepElectronID)
electronTag = 'ElectronIdProducer'

#################################################################### 
## Use SwitchJetCollection in order to rerun the btagging


## b-tag infos
bTagInfos = ['impactParameterTagInfos','secondaryVertexTagInfos']
## b-tag discriminators
bTagDiscriminators = ['jetBProbabilityBJetTags','jetProbabilityBJetTags',
                      'trackCountingHighPurBJetTags','trackCountingHighEffBJetTags',
                      'simpleSecondaryVertexHighEffBJetTags','simpleSecondaryVertexHighPurBJetTags',
                      'combinedSecondaryVertexBJetTags', 'combinedInclusiveSecondaryVertexV2BJetTags'
                      ]
## Jets, tracks, and vertices 
if runOnAOD:
    jetSource = 'pfJets'+pfpostfix
    trackSource = 'generalTracks'
    pvSource = 'offlinePrimaryVertices'
    svSource = cms.InputTag('inclusiveSecondaryVertices')
else:
    jetSource = 'ak4PFJetsCHS' 
    trackSource = 'unpackedTracksAndVertices'
    pvSource = 'unpackedTracksAndVertices'
    svSource = cms.InputTag('unpackedTracksAndVertices','secondary')
    
from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection

## Switch the default jet collection (done in order to use the above specified b-tag infos and discriminators)
switchJetCollection(
    process,
    jetSource = cms.InputTag(jetSource),
    trackSource = cms.InputTag(trackSource),
    pvSource = cms.InputTag(pvSource),
    svSource = svSource,
    btagInfos = bTagInfos,
    btagDiscriminators = bTagDiscriminators,
    jetCorrections = jetCorr,
    genJetCollection = cms.InputTag(genJetCollection),
    postfix = pfpostfix
    )

getattr(process,'patJetPartons'+pfpostfix).particles = cms.InputTag(genParticleCollection)
getattr(process,'patJetPartonMatch'+pfpostfix).matched = cms.InputTag(genParticleCollection)
getattr(process, 'patJetCorrFactors'+pfpostfix).primaryVertices = cms.InputTag(pvSource)

## agrohsje following lines needed ?
process.inclusiveVertexFinder.tracks = cms.InputTag(trackSource)
process.trackVertexArbitrator.tracks = cms.InputTag(trackSource)

patJets = ['patJets'+pfpostfix]
for m in patJets:
    if hasattr(process,m):
        print "Switching 'addTagInfos' for " + m + " to 'True'"
        setattr( getattr(process,m), 'addTagInfos', cms.bool(True) )
        print "Switching 'addJetFlavourInfo' for " + m + " to 'True'"
        setattr( getattr(process,m), 'addJetFlavourInfo', cms.bool(True) )


####################################################################
## Lepton information and dilepton filter at reco level 


from PhysicsTools.PatAlgos.selectionLayer1.electronSelector_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.muonSelector_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import *

process.kinMuons = selectedPatMuons.clone(
    src = muonTag,
    cut = 'pt > 8  && abs(eta) < 2.7'
    )
process.kinElectrons = selectedPatElectrons.clone(
    src = electronTag,
    cut = 'pt > 8  && abs(eta) < 2.7' # because of ECalP4 to be on the safe side
    )        

## filter on at least two opposite charge elecs/muons or emu
process.filterkinLeptons = cms.EDFilter("SimpleCounter",
                                        src = cms.VInputTag(cms.InputTag("kinMuons"),  
                                                            cms.InputTag("kinElectrons")), 
                                        requireOppoQ = cms.bool(True),
                                        minNumber = cms.uint32(minleptons),
                                        debug=cms.bool(debug)
                                        )
if isSignal:
    process.kinLeptonFilterSequence = cms.Sequence()
else:
    process.kinLeptonFilterSequence = cms.Sequence(process.filterkinLeptons)


####################################################################
## Jet information 

process.treeJets = selectedPatJets.clone( 
    src='patJets'+pfpostfix, 
    cut='eta < 5 && pt>5') # unfortunately starting at 10 GeV are needed for MET rescaling 8GeV should be ok as corrected pt 
    ### cut at uncorrected pt > 10 GeV on tree writer level! for MET rescaling - might be obsolete for 13 TeV (and when not using MET...)
    

#################################################################### 
## Configure ntuple 
process.PFTree   = cms.EDAnalyzer('TreeWriterTtZ',

                                  debugmode = cms.bool(debug),
                                  runOnAOD  = cms.bool(runOnAOD),
                                  #general input collections
                                  treeName = cms.string('pfTree'),
                                  muonSrc = cms.InputTag(muonTag),
                                  keepElecIdOnly = cms.string(keepElectronID),
                                  elecGSFSrc = cms.InputTag(electronTag), #just the same here to make it run. this can be prevented by a try{}catch(...){} in treewriter for getByLabel
                                  elecPFSrc = cms.InputTag(electronTag),
                                  jetSrc = cms.InputTag('treeJets'),  #jetTag), # ('treeJets'),
                                  btagAlgo = cms.string('combinedInclusiveSecondaryVertexV2BJetTags'), ###combinedSecondaryVertexBJetTags'),
                                  metSrc = cms.InputTag(metTag),  #here also try, catch statements
                                  mvaMetSrc = cms.InputTag(metTag), 
                                  metT1Src   =cms.InputTag(metTag),
                                  metT0T1TxySrc        =cms.InputTag(metTag),
                                  metT0T1Src           =cms.InputTag(metTag),
                                  metT1TxySrc          =cms.InputTag(metTag),
                                  vertexSrc = cms.InputTag(opt.primaryVertexOptions['outputCollection']),
                                  
                                  #block for extended information needed for efficiency studies
                                  includeReco = cms.bool(False),
                                  recoMuonSrc = cms.InputTag('muons'),
                                  isPFMuonCand = cms.bool(False),
                                  recoElecSrc  = cms.InputTag('gsfElectrons'),
                                  isPFElecCand = cms.bool(False),
                                  recoTrackSrc = cms.InputTag('generalTracks'),
                                  recoSuCluSrc = cms.InputTag('superClusters'),
                                                                    
                                  #block for trigger information
                                  includeTrigger = cms.bool(True),
                                  triggerResults = cms.InputTag('TriggerResults','','HLT'), #needed for both AOD and MiniAOD
                                  triggerEvent   = cms.InputTag('patTriggerEvent'), ### "selectedPatTrigger" for MiniAODs
                                  triggerObjects = cms.vstring(""),
                                  
                                  #block for event information.  Essential (PU)
                                  PUInfo = cms.InputTag('addPileupInfo'),
                                  includePDFWeights = cms.bool(includePDFWeights),
                                  pdfWeights = cms.InputTag("pdfWeights:"+PDF),
                                  additionalWeights = cms.vstring(""), #("scaleUp"), #can be any vector of strings
                                  #agrohsje fix  
                                  rhoIso = cms.InputTag("fixedGridRhoFastjetAll"), #dummy for 13 TeV AOD
                                  
                                  includeGen = cms.bool(isSignal),
                                  genParticles = cms.InputTag(genParticleCollection),
                                  # agrohsje fix later 
                                  genJets = cms.InputTag("ak4GenJetsNoNuNoLepton"), 
                                  #cms.InputTag(""), # in miniaod:slimmedGenJets, in aod:ak4GenJets
                                  

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


####################################################################
## Path (filter and analyzer only)

process.dump=cms.EDAnalyzer('EventContentAnalyzer')

process.path = cms.Path( 
    process.preCutPUInfo*
    process.fsFilterSequence*
    process.postCutPUInfo*
    process.goodOfflinePrimaryVertices *   
    process.prefilterSequence * #for data only 
    #    process.dump *
    process.kinLeptonFilterSequence * # agrohsje 
    process.PFTree
    )

#process.outpath    = cms.EndPath()


####################################################################
## Signal catcher for more information on errors and to resume jobs    


process.load("TopAnalysis.TopUtils.SignalCatcher_cfi")


