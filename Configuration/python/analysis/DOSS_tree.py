import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

###parsing stuff###

options = VarParsing.VarParsing()

options.register ('is2011',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is 2011 data/MC")
options.register ('crab',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"uses crab")
options.register ('globalTag','START53_V26',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"global tag")
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
options.register ('includereco',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes info for eff studies")
options.register ('includetrigger',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes full trigger string info for event")
options.register ('includePDF',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes pdf weights info for event")
options.register ('PDF','cteq65',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")
options.register ('inputScript','TtZAnalysis.Configuration.samples.mc.TTJets_MassiveBinDECAY_TuneZ2star_8TeV_madgraph_tauola_Summer12_DR53X_PU_S10_START53_V7A_v1_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register ('json','nojson',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"json files")
options.register ('isSync',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"switch on for sync")
options.register('samplename', 'standard', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which sample to run over - obsolete")
options.register ('laseroff',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"use ECal Laser filter")

options.register ('jpsi',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"make trees for J/Psi")

options.register ('isSignal',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is SignalMC")
options.register ('muCone03',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"use iso cone of 0.3")

options.register ('wantSummary',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"prints trigger summary")
options.register ('ttH',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"writes the ttH tree")
options.register ('susy',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"writes the Susy tree")
options.register ('isPrompt',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"loads extra filters etc for prompt")




options.register ('debug',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"enters debug mode")
options.register ('isFastSim',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"FastSim")


import sys


if hasattr(sys, "argv"):
    options.parseArguments()

is2011=options.is2011                    # def false
crab=options.crab                        # False   OBSOLETE!!!!!
globalTag=options.globalTag              # START53_V11
reportEvery=options.reportEvery          # 1000
maxEvents=options.maxEvents              # -1. overwrites the one specified in input script. be careful!!
outputFile=options.outputFile            # def_out
isMC=options.isMC                        # True
genFilter=options.genFilter              # 'none'
genFilterString=options.genFilterString  # 'none'
genFilterInvert=options.genFilterInvert  # False
includereco=options.includereco          # False
includetrigger=options.includetrigger    # True
includePDFWeights=options.includePDF     # False
PDF=options.PDF                          # cteq65
inputScript=options.inputScript          # TtZAnalysis.Configuration.samples.mc.DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_Summer12-PU_S7_START52_V9-v2_cff
json=options.json                        # give full path!!json files in TtZAnalysis/Data/data ONLY needed with nafjobsplitter
laseroff=options.laseroff
susy=options.susy
jpsi=options.jpsi

isSignal=options.isSignal
newMuons=options.muCone03
wantSummary=options.wantSummary
ttH=options.ttH

isFastSim=options.isFastSim

debug=options.debug
isPrompt=options.isPrompt

if susy:
    isPrompt=True

useBHadrons=False #will be changes for top filter sequence!

if not isMC:
    isSignal=False


syncfile=options.isSync                  # False

#print genFilterString

print "\n\n"

electronIsoCone="03"
useGsf = True
minleptons=2

print 'minimum 2 leptons required. Check tree filling cuts for kinLeptons!! Even for includereco=true'

if syncfile:
    globalTag='START52_V9'
    isMC=True
    genFilter=""
    outputFile='SyncFiletest'
    includereco=False
    includetrigger=False
    minleptons=2

if includePDFWeights:
    import os
    newlha = os.environ['CMSSW_BASE']+'/src/TtZAnalysis/Data/data/PDFSets'
    os.environ['LHAPATH']=newlha



pfpostfix='PFlow' #unfortunately some things don't seem to work otherwise.. pfjets get lost somehow produces LOADS of overhead...

process = cms.Process("Yield")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')

if not isMC and not is2011 and isPrompt: # https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagJetProbabilityCalibration?redirectedfrom=CMS.SWGuideBTagJetProbabilityCalibration#Calibration_in_52x_and_53x_Data
    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("BTagTrackProbability2DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_2D_Data53X_v2"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU")),
        cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_3D_Data53X_v2"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU"))
        )

##  not needed anymore in rereco

if isMC and not is2011:

    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("BTagTrackProbability2DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_2D_MC53X_v2"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU")),
        cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_3D_MC53X_v2"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU"))
        )


if isMC and is2011:

    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("BTagTrackProbability2DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_2D_MC_80_Ali44_v1"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU")),
        cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_3D_MC_80_Ali44_v1"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU"))
        )

#data 2011 is ok

#Options
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(wantSummary))



#Message Logger Stuff
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
#process.MessageLogger.suppressWarning=['particleFlowDisplacedVertexCandidate','The interpolated laser correction is <= zero! (0). Using 1. as correction factor.']
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery

process.GlobalTag.globaltag = globalTag + '::All'
#process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( inputFiles ), skipEvents=cms.untracked.uint32( skipEvents ) )
process.out    = cms.OutputModule("PoolOutputModule", outputCommands =  cms.untracked.vstring(), fileName = cms.untracked.string( outputFile + '_PatTuple') )

################# Input script (some default one for crab

process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( '/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S10_START53_V7A-v1/0000/A89D210D-1BE2-E111-9EFB-0030487F1797.root' ))
process.source.skipBadFiles = cms.untracked.bool( True )

if not inputScript=='':
    process.load(inputScript)


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32 (maxEvents) )
if options.skipEvents > 0:
    process.source.skipEvents = cms.untracked.uint32(options.skipEvents)

#########JSON###
# limit to json file (if passed as parameter)
realdata = True
if isMC:
    realdata=False
if realdata and not (json=="nojson"):
    import os
    jsonpath = json 

    import FWCore.PythonUtilities.LumiList as LumiList
    import FWCore.ParameterSet.Types as CfgTypes
    myLumis = LumiList.LumiList(filename = jsonpath).getCMSSWString().split(',')
    process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
    process.source.lumisToProcess.extend(myLumis)




#### TFile Service
outFileName = outputFile + '.root'

process.TFileService = cms.Service("TFileService",
    fileName = cms.string(outFileName)
)


#####pre filter sequences#######

#### for MC ##

### PDF weights. if pdf file cannot be accessed, check afs access depending on where your LHAPDF path variable is set

process.pdfWeights = cms.EDProducer("PdfWeightProducer",
                                    PdfInfoTag = cms.untracked.InputTag("generator"),
                                    PdfSetNames = cms.untracked.vstring(PDF+".LHgrid")
                                    )


process.load('TtZAnalysis.TreeWriter.puinfo_cff')

process.PUInfo.includePDFWeights = includePDFWeights
process.PUInfo.pdfWeights = "pdfWeights:"+PDF

process.preCutPUInfo = process.PUInfo.clone()
process.preCutPUInfo.treeName = 'preCutPUInfo'

process.postCutPUInfo = process.PUInfo.clone()
process.postCutPUInfo.treeName = 'PUTreePostCut'
process.postCutPUInfo.includePDFWeights = False

process.pfLeps = cms.EDFilter("PdgIdPFCandidateSelector",
                              pdgId = cms.vint32(-13, 13, 11, -11),
                              src = cms.InputTag("particleFlow")
                              )
process.allLeps = cms.EDProducer("CandViewMerger",
                                 src = cms.VInputTag(cms.InputTag("gsfElectrons"), cms.InputTag("pfLeps"))
                                 )
process.requireMinLeptons = cms.EDFilter("CandViewCountFilter",
                                         src = cms.InputTag('allLeps'),
                                         minNumber = cms.uint32(minleptons)
                                         )

process.requireRecoLeps =  cms.Sequence(process.pfLeps *
                                        process.allLeps *
                                        process.requireMinLeptons)

process.preFilterSequence = cms.Sequence()

if isMC:
    ##
    if genFilter=='Top':
        process.load('TopAnalysis.TopFilter.filters.GeneratorTopFilter_cfi')
        process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

        #process.load("TopAnalysis.TopUtils.GenLevelBJetProducer_cfi")
        #process.produceGenLevelBJets.deltaR = 5.0
        #process.produceGenLevelBJets.noBBbarResonances = True

        process.decaySubset.fillMode = "kME" # Status3, use kStable for Status2     
        process.topsequence = cms.Sequence( 
            process.makeGenEvt *
            process.generatorTopFilter 
            )
        process.generatorTopFilter.channels = genFilterString
        process.generatorTopFilter.invert_selection=genFilterInvert

        if genFilterInvert:
            print 'genFilter set to Top: inverted : ' 
            print genFilterString
        else:
            print 'genFilter set to Top: ' 
            print genFilterString

########b-hadron stuff 1st part

        process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi") # supplies PDG ID to real name resolution of MC particles, necessary for GenLevelBJetProducer
        process.load("TopAnalysis.TopUtils.GenLevelBJetProducer_cff")
        process.produceGenLevelBJetsPlusHadron.deltaR = 5.0
        process.produceGenLevelBJetsPlusHadron.noBBbarResonances = True
        process.produceGenLevelBJetsPlusHadron.doImprovedHadronMatching = True
       # process.produceGenLevelBJetsPlusHadron.doValidationPlotsForImprovedHadronMatching = False
        process.load("TopAnalysis.TopUtils.sequences.improvedJetHadronQuarkMatching_cff")

##top projections turned off -> re-switch this?

       # process.genParticlesForJetsNoNuPlusHadron.ignoreParticleIDs += cms.vuint32( 12,14,16)
       # process.genParticlesForJetsNoMuNoNuPlusHadron.ignoreParticleIDs += cms.vuint32( 12,13,14,16)

        useBHadrons=True

        process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                 process.topsequence *
                                                 process.postCutPUInfo *
                                                 process.improvedJetHadronQuarkMatchingSequence *
                                                 process.produceGenLevelBJetsPlusHadron #*
                                                 #process.requireRecoLeps
                                                 )




    elif genFilter=='Z':
        process.load('TopAnalysis.TopFilter.filters.GeneratorZFilter_cfi')
        if not options.genFilterMassHigh == 0:
            process.generatorZFilter.diLeptonMassIntervals=(options.genFilterMassLow,options.genFilterMassHigh)

        if genFilterString==['ElectronElectron']:
            print 'genFilter set to Z: Electron Electron'
            process.generatorZFilter.zDecayModes = cms.vint32(11)
        elif genFilterString==['MuonMuon']:
            print 'genFilter set to Z: Muon Muon'
            process.generatorZFilter.zDecayModes = cms.vint32(13)
        elif genFilterString==['TauTau']:
            print 'genFilter set to Z: Tau Tau'
            process.generatorZFilter.zDecayModes = cms.vint32(15)
        elif genFilterString==['Leptonic']:
            print 'genFilter set to Z: all leptonic'
            process.generatorZFilter.zDecayModes = cms.vint32(11,13,15)
        else:
            print '\n\n\nWARNING!!! gen Filter set to Z but no specific decay mode selected!!'
            print genFilterString
            print 'not valid!'
            exit(8888)
        print "massrange: " 
        print options.genFilterMassLow
        print options.genFilterMassHigh
        
        if genFilterInvert:
            print 'genFilter Z inverted'
            process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                     ~process.generatorZFilter *
                                                     process.postCutPUInfo )#*
                                                    # process.requireRecoLeps)
        else:
            process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                     process.generatorZFilter *
                                                     process.postCutPUInfo)#*
                                                    # process.requireRecoLeps)
            
   
    else:
        process.preFilterSequence = cms.Sequence(process.preCutPUInfo )#* 
                                                 #process.requireRecoLeps)
    if includePDFWeights:
        getattr(process, 'preFilterSequence').replace(process.preCutPUInfo,
                                                      process.pdfWeights *
                                                      process.preCutPUInfo)

    if is2011:
        ## load pythia bug filter

        process.load("GeneratorInterface.GenFilters.TotalKinematicsFilter_cfi")
        process.totalKinematicsFilter.tolerance = 5
        
        getattr(process, 'preFilterSequence').replace(process.preCutPUInfo,
                                                      process.totalKinematicsFilter *
                                                      process.preCutPUInfo)

#### for data ###
else:
    ## add extra filter for ECal laser Calib

    process.preFilterSequence = cms.Sequence(process.preCutPUInfo *
                                             process.requireRecoLeps)

    if not is2011:
        process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
        getattr(process, 'preFilterSequence').replace(process.preCutPUInfo,
                                                      process.ecalLaserCorrFilter *
                                                      process.preCutPUInfo)

    
    

#from PhysicsTools.PatAlgos.patTemplate_cfg import *


filtervertices=True
if isSignal:
    filtervertices=False

## The good primary vertex filter ____________________________________________||
process.primaryVertexFilter = cms.EDFilter(
    "VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),
    cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2"),
    filter = cms.bool(filtervertices)
    )

## The beam scraping filter __________________________________________________||
process.noscraping = cms.EDFilter(
    "FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
    )

## The iso-based HBHE noise filter ___________________________________________||
process.load('CommonTools.RecoAlgos.HBHENoiseFilter_cfi')

## The CSC beam halo tight filter ____________________________________________||
## The tracking failure filter _______________________________________________||
if not is2011:
    process.load('RecoMET.METFilters.trackingFailureFilter_cfi')
    process.load('RecoMET.METAnalyzers.CSCHaloFilter_cfi')

## The HCAL laser filter _____________________________________________________||
    process.load("RecoMET.METFilters.hcalLaserEventFilter_cfi")

## The ECAL dead cell trigger primitive filter _______________________________||
    process.load('RecoMET.METFilters.EcalDeadCellTriggerPrimitiveFilter_cfi')
    process.EcalDeadCellTriggerPrimitiveFilter.tpDigiCollection = cms.InputTag("ecalTPSkimNA")


## The EE bad SuperCrystal filter ____________________________________________||
    process.load('RecoMET.METFilters.eeBadScFilter_cfi')

## The ECAL laser correction filter
###process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
## already plugged in before

## The Good vertices collection needed by the tracking failure filter ________||
process.goodVertices = cms.EDFilter(
  "VertexSelector",
  filter = cms.bool(False),
  src = cms.InputTag("offlinePrimaryVertices"),
  cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.rho < 2")
)



## The tracking POG filters __________________________________________________||
#process.load('RecoMET.METFilters.trackingPOGFilters_cff')

if is2011 or not isPrompt:
    process.filtersSeq = cms.Sequence(
        process.primaryVertexFilter *
        process.goodVertices *
        process.noscraping *
        process.HBHENoiseFilter 
        )
else:
    process.filtersSeq = cms.Sequence(
        process.primaryVertexFilter *
        process.noscraping *
        process.HBHENoiseFilter *
        process.CSCTightHaloFilter *
        process.hcalLaserEventFilter *
        process.EcalDeadCellTriggerPrimitiveFilter *
        process.goodVertices * 
        process.trackingFailureFilter *
        process.eeBadScFilter #*
        #process.ecalLaserCorrFilter * ##is already in prefilterseq... nasty but ok
        #process.trkPOGFilters
        )



if isMC:
    process.filtersSeq = cms.Sequence(process.goodVertices 
                                      )


### if its not signal do some pre filtering:

    
if not isSignal:
    process.preFilterSequence += process.requireRecoLeps



##########Do primary vertex filtering###


#from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector


process.goodOfflinePrimaryVertices = cms.EDFilter( "PrimaryVertexObjectFilter" , 
                                                   filterParams = cms.PSet( minNdof = cms.double( 4. ) , 
                                                                            maxZ = cms.double( 24. ) , 
                                                                            maxRho = cms.double( 2. ) ) , 
                                                   filter = cms.bool( filtervertices) , 
                                                   src = cms.InputTag( 'offlinePrimaryVertices' ) )
#

###########default pat and pf2pat########
# pat sequence
process.load("PhysicsTools.PatAlgos.patSequences_cff")
### this is necessary to avoid the conflict between PAT and RECO configurations  latinos...implementation! OLD! see isojet stuff below
### see: https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1357.html
#process.kt6PFJets.doRhoFastjet = True
#process.kt6PFJets.doAreaFastjet = True
#process.kt6PFJets.voronoiRfact = 0.9
#process.kt6PFJets.Rho_EtaMax   = cms.double( 4.4)




if isMC:
    jetCorr =('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute'])
else:
    jetCorr = ('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute', 'L2L3Residual'])



from PhysicsTools.PatAlgos.tools.pfTools import *
usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=isMC, postfix=pfpostfix, jetCorrections=jetCorr, pvCollection=cms.InputTag('goodVertices'),typeIMetCorrections=True) 


if not is2011:
    getattr(process,'patJetCorrFactors'+pfpostfix).rho=cms.InputTag("kt6PFJets","rho","RECO")

process.pfPileUp.checkClosestZVertex = False

##################################
#  MVA eID
##################################
process.load('EGamma.EGammaAnalysisTools.electronIdMVAProducer_cfi')
process.eidMVASequence = cms.Sequence(  process.mvaTrigV0 )
getattr(process,'patElectrons'+pfpostfix).electronIDSources.mvaTrigV0    = cms.InputTag("mvaTrigV0")
getattr(process, 'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patElectrons'+pfpostfix),
                                              process.eidMVASequence *
                                              getattr(process,'patElectrons'+pfpostfix)
                                              )

########
#   adapt electrons to R=03 and use gsf

from TtZAnalysis.Workarounds.useGsfElectrons import *

process.patPFElectronsPFlow = getattr(process,'patElectrons'+pfpostfix).clone()
getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patElectrons'+pfpostfix),
                                                     getattr(process,'patElectrons'+pfpostfix) *
                                                     process.patPFElectronsPFlow)


####### make default patElectrons collection fed by gsf
useGsfElectronsInPF2PAT(process,pfpostfix,electronIsoCone)
# bugfixes made in pfTools and ofIsolation
    

####### adapt pfElectrons 
    
getattr(process,'patPFElectrons'+pfpostfix).isolationValues = cms.PSet(
    pfChargedHadrons = cms.InputTag("elPFIsoValueCharged"+electronIsoCone+"PFId"+pfpostfix),
    pfChargedAll = cms.InputTag("elPFIsoValueChargedAll"+electronIsoCone+"PFId"+pfpostfix),
    pfPUChargedHadrons = cms.InputTag("elPFIsoValuePU"+electronIsoCone+"PFId"+pfpostfix),
    pfNeutralHadrons = cms.InputTag("elPFIsoValueNeutral"+electronIsoCone+"PFId"+pfpostfix),
    pfPhotons = cms.InputTag("elPFIsoValueGamma"+electronIsoCone+"PFId"+pfpostfix)
    )

    


######### end of electron implementation ########
##### MUONS ########

if newMuons:

    print "Muon Cone size 0.3 is used!!!!!\n\n\n"

    getattr(process,'pfIsolatedMuons'+pfpostfix).doDeltaBetaCorrection = True
    getattr(process,'pfIsolatedMuons'+pfpostfix).deltaBetaIsolationValueMap = cms.InputTag("muPFIsoValuePU03"+pfpostfix, "", "")
    getattr(process,'pfIsolatedMuons'+pfpostfix).isolationValueMapsCharged = [cms.InputTag("muPFIsoValueCharged03"+pfpostfix)]
    getattr(process,'pfIsolatedMuons'+pfpostfix).isolationValueMapsNeutral = [cms.InputTag("muPFIsoValueNeutral03"+pfpostfix), cms.InputTag("muPFIsoValueGamma03"+pfpostfix)]

    getattr(process,'patMuons'+pfpostfix).isolationValues = cms.PSet(
        pfNeutralHadrons = cms.InputTag("muPFIsoValueNeutral03"+pfpostfix),
        pfChargedAll = cms.InputTag("muPFIsoValueChargedAll03"+pfpostfix),
        pfPUChargedHadrons = cms.InputTag("muPFIsoValuePU03"+pfpostfix),
        pfPhotons = cms.InputTag("muPFIsoValueGamma03"+pfpostfix),
        pfChargedHadrons = cms.InputTag("muPFIsoValueCharged03"+pfpostfix)
        )


if susy:
    
    from PhysicsTools.PatAlgos.tools.pfTools import *
    process.muIsoSequencetwo = setupPFMuonIso(process, 'muons', 'PFIso2')
    adaptPFIsoMuons( process, applyPostfix(process,"patMuonsPFlow",""), 'PFIso2', "04")
    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                       getattr(process,'muIsoSequencetwo') *
                                                       getattr(process,'patMuons'+pfpostfix))
    

    process.patMuonsPFlow.useParticleFlow = cms.bool(False)
    process.patMuonsPFlow.pfMuonSource = cms.InputTag("particleFlow")
    process.muonMatchPFlow.src     = "muons"

    

################Jets########## 

###break top projection and repair taus

getattr(process,'pfNoMuon'+pfpostfix).enable = False
getattr(process,'pfNoElectron'+pfpostfix).enable = False
getattr(process,'pfNoJet'+pfpostfix).enable = False
getattr(process,'pfNoTau'+pfpostfix).enable = False

if jpsi:
    getattr(process,'patJets'+pfpostfix).addTagInfos = True

#process.pfJetsForTaus = getattr(process,'pfJets'+pfpostfix).clone()
#getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'pfJets'+pfpostfix),
#                                                       getattr(process,'pfJets'+pfpostfix) *
#                                                       process.pfJetsForTaus)

#getattr(process,'pfJets'+pfpostfix).src='pfNoPileUp'+pfpostfix
#getattr(process,'patJets'+pfpostfix).jetSource = 'pfJets'+pfpostfix

#getattr(process,'patPF2PATSequence'+pfpostfix).remove(getattr(process,'pfJets'+pfpostfix))
#getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'pfNoPileUp'+pfpostfix),
#                                                       getattr(process,'pfNoPileUp'+pfpostfix) *
#                                                       getattr(process,'pfJets'+pfpostfix))
                                                       


##just for taus
#getattr(process,'pfNoJet'+pfpostfix).bottomCollection = 'pfNoPileUp'+pfpostfix

#massSearchReplaceAnyInputTag((getattr(process, 'pfTauSequence'+pfpostfix)),cms.InputTag('pfJets'+pfpostfix), cms.InputTag('pfJetsForTaus'),True)
#getattr(process,'pfNoTau'+pfpostfix).bottomCollection = cms.InputTag('pfJetsForTaus')


###### make isolation cut invalid

getattr(process,'pfIsolatedElectrons'+pfpostfix).isolationCut = 99999 
getattr(process,'pfIsolatedMuons'+pfpostfix).isolationCut = 99999 

####### Fix not automatically implemented b-tagging modules/vertices:

# process.load('RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff')

######### build jets for rho value ## only if 2011 rho is used (hopefully not for that long..)
# https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaEARhoCorrection

process.kt6PFJetsForIso = process.kt4PFJets.clone(rParam = 0.6, doRhoFastjet = True,  Rho_EtaMax = cms.double(2.5))
process.kt6PFJetsForIsoNoPU = process.kt6PFJetsForIso.clone( src = "pfNoPileUp"+ pfpostfix)

process.isoJetSequence = cms.Sequence(process.kt6PFJetsForIso * process.kt6PFJetsForIsoNoPU)

### for met/jet correction (2011 data) ## warning!! postfix hardcoded! using prescription:
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorPFnoPU

process.kt6PFJets = process.kt4PFJets.clone(
    rParam = cms.double(0.6),
    doAreaFastjet = cms.bool(True),
    doRhoFastjet = cms.bool(True)
    )

process.kt6PFJetsPFlow = process.kt6PFJets.clone()
if is2011:
    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patJets'+pfpostfix),
                                                           process.kt6PFJetsPFlow *
                                                           getattr(process,'patJets'+pfpostfix))


###### Match triggers to leptons

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )

process.patTrigger.onlyStandAlone = False

#process.reducedPatTrigger = cms.EDProducer("TriggerObjectFilterByCollection",
#    src = cms.InputTag("patTrigger"),
#    collections = cms.vstring("hltL1extraParticles", "hltL2MuonCandidates", "hltL3MuonCandidates", "hltGlbTrkMuonCands", "hltMuTrackJpsiCtfTrackCands", "hltMuTrackJpsiEffCtfTrackCands", "hltMuTkMuJpsiTrackerMuonCands"),
#) 

#process.patTrigger.addL1Algos = True

process.patGSFElectronsTriggerMatches = cms.EDProducer("PATTriggerMatcherDRDPtLessByR",
    matchedCuts = cms.string("path(\"HLT_Ele27_WP80_v*\")"),
    src = cms.InputTag("patElectrons"+ pfpostfix),
    maxDPtRel = cms.double(10),
    resolveByMatchQuality = cms.bool(True),
    maxDeltaR = cms.double(0.2),
    resolveAmbiguities = cms.bool(True),
    matched = cms.InputTag("patTrigger")
)
process.patPFElectronsTriggerMatches = process.patGSFElectronsTriggerMatches.clone()
process.patPFElectronsTriggerMatches.src = 'patPFElectrons'+pfpostfix



process.patMuonsTriggerMatches = process.patGSFElectronsTriggerMatches.clone()
process.patMuonsTriggerMatches.src = 'patMuons'+ pfpostfix
process.patMuonsTriggerMatches.matchedCuts = cms.string("path(\"HLT_IsoMu24_v*\") || path(\"HLT_IsoMu24_eta2p1_v*\")")
process.patMuonsTriggerMatches.maxDeltaR = cms.double(0.1)


muontriggermodules = cms.string("filter(\"hltDiMuonGlb17Trk8DzFiltered0p2\") || filter(\"hltDiMuonGlb17Glb8DzFiltered0p2\")")


if not includereco and includetrigger:

    process.patMuonsTriggerMatches.matchedCuts = muontriggermodules

    process.patGSFElectronsTriggerMatches.matchedCuts = cms.string("path(\"HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*\") || path(\"HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*\") || path(\"HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*\")")

    process.patPFElectronsTriggerMatches.matchedCuts = process.patGSFElectronsTriggerMatches.matchedCuts



process.patGSFElectronsWithTrigger = cms.EDProducer("PATTriggerMatchElectronEmbedder",
                                                    src = cms.InputTag("patElectrons"+ pfpostfix),
                                                    matches = cms.VInputTag("patGSFElectronsTriggerMatches")
                                                    )

process.patPFElectronsWithTrigger = process.patGSFElectronsWithTrigger.clone()
process.patPFElectronsWithTrigger.src = "patPFElectrons"+pfpostfix
process.patPFElectronsWithTrigger.matches = ['patPFElectronsTriggerMatches']

process.patMuonsWithTrigger = cms.EDProducer("PATTriggerMatchMuonEmbedder",
                                             src = cms.InputTag("patMuons"+ pfpostfix),
                                             matches = cms.VInputTag("patMuonsTriggerMatches")
                                             )


if includereco or includetrigger:
    process.triggerMatches =  cms.Sequence(#process.reducedPatTrigger *
                                           process.patGSFElectronsTriggerMatches *
                                           process.patPFElectronsTriggerMatches *
                                           process.patMuonsTriggerMatches *
                                           process.patGSFElectronsWithTrigger *
                                           process.patPFElectronsWithTrigger *
                                           process.patMuonsWithTrigger)
    

else:
    process.triggerMatches =  cms.Sequence()
    

process.patTriggerSequence = cms.Sequence(process.patTrigger *
                                          process.triggerMatches *
                                          process.patTriggerEvent)

#from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run52xOn51xTrigger

#run52xOn51xTrigger(process,'patTriggerSequence')

###### Merge SuperClusters

process.superClusters = cms.EDProducer("SuperClusterMerger",
                                       src=cms.VInputTag( cms.InputTag("correctedHybridSuperClusters") , cms.InputTag("correctedMulti5x5SuperClustersWithPreshower") ) )


####### some jet and ID cuts to speed up and keep treesize low. The leptons are only produced to filter, the tree is filled with leptons without cuts!

process.treeJets = process.selectedPatJets.clone()
process.treeJets.src="patJets"+pfpostfix
process.treeJets.cut = 'eta < 5 && pt>5' # unfortunately starting at 10 GeV are needed for MET rescaling 8GeV should be ok as corrected pt
### cut at uncorrected pt > 10 GeV on tree writer level

process.kinMuons = process.selectedPatMuons.clone()
process.kinMuons.src = 'patMuons' + pfpostfix
#process.kinMuons.cut = cms.string('pt > 30  && abs(eta) < 2.1')
process.kinMuons.cut = cms.string('pt > 8  && abs(eta) < 2.7')
if includetrigger or jpsi:
    process.kinMuons.cut = cms.string('pt > 3  && abs(eta) < 2.7')


process.kinElectrons = process.selectedPatElectrons.clone()
process.kinElectrons.src = 'patElectrons' + pfpostfix
process.kinElectrons.cut = cms.string( 'pt > 8  && abs(eta) < 2.7') # because of ECalP4 to be on the safe side

if includetrigger or jpsi:
    process.kinElectrons.cut = cms.string( 'pt > 3  && abs(eta) < 2.7')

process.kinPFElectrons = process.selectedPatElectrons.clone()
process.kinPFElectrons.src = 'patPFElectrons' + pfpostfix
process.kinPFElectrons.cut = cms.string( 'pt > 8  && abs(eta) < 2.7')

if includetrigger or jpsi:
    process.kinPFElectrons.cut = cms.string( 'pt > 3  && abs(eta) < 2.7')


process.MuonGSFMerge = cms.EDProducer("CandViewMerger",
                                     src = cms.VInputTag(cms.InputTag("kinMuons"),  cms.InputTag("kinElectrons"))
                                     )

process.MuonPFMerge = cms.EDProducer("CandViewMerger",
                                     src = cms.VInputTag(cms.InputTag("kinMuons"),  cms.InputTag("kinPFElectrons"))
                                     )


process.filterkinLeptons = cms.EDFilter("SimpleCounter",
                                       src = cms.VInputTag(cms.InputTag("kinMuons"),  
                                                           cms.InputTag("kinElectrons"),   
                                                           cms.InputTag("kinPFElectrons"),
                                                           cms.InputTag("MuonGSFMerge"),
                                                           cms.InputTag("MuonPFMerge")),
                                       minNumber = cms.uint32(minleptons)
                                       )

process.kinLeptonFilterSequence = cms.Sequence(process.kinMuons *
                                              process.kinElectrons *
                                              process.kinPFElectrons *
                                              process.MuonGSFMerge *
                                              process.MuonPFMerge *
                                              process.filterkinLeptons)

if not isSignal:
    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                           getattr(process,'patMuons'+pfpostfix) *
                                                           process.kinLeptonFilterSequence)

    

else: ## is Signal

    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                           getattr(process,'patMuons'+pfpostfix) *
                                                           process.kinMuons *
                                                           process.kinElectrons *
                                                           process.kinPFElectrons )
                                                       

########## Prepare Tree ##
if ttH:
    process.load('TtZAnalysis.TreeWriter.treewriter_tth_cff')

elif susy:
    process.load('TtZAnalysis.TreeWriter.treewriter_susy_cff')
    from TtZAnalysis.Workarounds.usePFIsoCone import *
    usePFIsoCone(process)
    #load the standard pat objects
    #process.kinMuons.src = 'patMuons'
    #process.kinElectrons.src = 'patElectrons'

else:
    process.load('TtZAnalysis.TreeWriter.treewriter_ttz_cff')





process.PFTree.vertexSrc         = 'goodVertices'
process.PFTree.metSrc            = 'patMETs'+pfpostfix
process.PFTree.mvaMetSrc            = 'patMETs'+pfpostfix
process.PFTree.includeTrigger    = includetrigger
process.PFTree.includeReco       = includereco
process.PFTree.rhoJetsIsoNoPu    = cms.InputTag("kt6PFJetsForIsoNoPU","rho",process.name_())
process.PFTree.rhoJetsIso        = cms.InputTag("kt6PFJetsForIso","rho",process.name_()) ## only used if rho 2011
process.PFTree.includeRho2011    = is2011
process.PFTree.includePDFWeights = includePDFWeights
process.PFTree.pdfWeights        = "pdfWeights:"+PDF
process.PFTree.includeGen        = isSignal
process.PFTree.useBHadrons       = useBHadrons
process.PFTree.isJPsi            = jpsi

if isSignal and genFilter=="Top":
    process.PFTree.genJets           = 'ak5GenJetsPlusHadron'


# always: if not (includereco or includetrigger):
process.PFTree.muonSrc = 'kinMuons'
process.PFTree.elecGSFSrc =  'kinElectrons'
process.PFTree.elecPFSrc =  'kinPFElectrons'




if includetrigger: #lower pfMuon threshold
    process.pfSelectedMuonsPFlow.cut = cms.string('pt>3')
    process.PFTree.triggerObjects = cms.vstring("hltDiMuonGlb17Trk8DzFiltered0p2", ##tkMu8
                                                "hltL3fL1sMu10MuOpenL1f0L2f10L3Filtered17",     #mu17 leg of TkMu8
                                                "hltL3fL1sMu10MuOpenOR3p5L1f0L2f10L3Filtered17", #mu17 leg of TkMu8 late RunC
                                                "hltDiMuonGlbFiltered17TrkFiltered8",             #TkMu8 leg of TkMu8
                                                "hltDiMuonGlb17Glb8DzFiltered0p2", ##Mu8 only if Z Filter is switched on!! (RunA, C, D)
                                                "hltL3fL1DoubleMu10MuOpenL1f0L2f10L3Filtered17",   ## Mu17 leg of Mu17Mu8
                                                "hltL3fL1DoubleMu10MuOpenOR3p5L1f0L2f10L3Filtered17", ## Mu17 leg for late RunC
                                                "hltL3pfL1DoubleMu10MuOpenL1f0L2pf0L3PreFiltered8",   # Mu8 leg of Mu17Mu8
                                                "hltL3pfL1DoubleMu10MuOpenOR3p5L1f0L2pf0L3PreFiltered8") # Mu8 leg of Mu17Mu8 late RunC



process.PFTree.debugmode= debug

## make tree sequence

process.treeSequence = cms.Sequence(process.patTriggerSequence *
                                    process.superClusters *
                                    process.treeJets *
                                    process.PFTree)



##### does this prevent segfaults?

###### Path

process.path = cms.Path( process.goodOfflinePrimaryVertices *
                         process.preFilterSequence *
                         process.filtersSeq *
                        #  process.inclusiveVertexing *   ## segfaults?!?! in the newest release or MC
                        
                      #  process.btagging *             #not yet implemented fully in pf2pat sequence../ needed for new btagging tag
                        
                        getattr(process,'patPF2PATSequence'+pfpostfix) *
                        process.isoJetSequence  *
                        process.treeSequence
                         )

#if susy:
#    process.path.replace(getattr(process,'patPF2PATSequence'+pfpostfix),
#                 process.patDefaultSequence)

if isFastSim:
    massSearchReplaceAnyInputTag(process.path,cms.InputTag("kt6PFJets","rho", "RECO"), cms.InputTag("kt6PFJets","rho", "HLT"),True)


#massSearchReplaceAnyInputTag(process.path,cms.InputTag('goodOfflinePrimaryVertices'), cms.InputTag('goodVertices'),True)


#### plug in vertex filter and change collections

process.outpath    = cms.EndPath()


print "\nglobal Tag: " + globalTag

