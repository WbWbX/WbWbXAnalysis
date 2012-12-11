import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

###parsing stuff###

options = VarParsing.VarParsing()

options.register ('is2011',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is 2011 data/MC")
options.register ('crab',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"uses crab")
options.register ('globalTag','START53_V11',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"global tag")
options.register ('reportEvery',1000,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"report every")
options.register ('maxEvents',-1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register ('outputFile','def_out',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register ('isMC',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is MC")
options.register ('genFilter','none',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"gen Filter")
options.register ('genFilterString','none',VarParsing.VarParsing.multiplicity.list,VarParsing.VarParsing.varType.string,"gen Filter selection string")
options.register ('genFilterMassLow',0,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"gen Filter mass range low")
options.register ('genFilterMassHigh',0,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"gen Filter mass range high")
options.register ('genFilterInvert',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"invert gen Filter")
options.register ('includereco',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes info for eff studies")
options.register ('includetrigger',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes trigger info for event")
options.register ('includePDF',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes pdf weights info for event")
options.register ('PDF','cteq65',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")
options.register ('inputScript','',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register ('json','nojson',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"json files")
options.register ('isSync',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"switch on for sync")
options.register('samplename', 'standard', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which sample to run over - obsolete")

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

crab=True # for GC/Crab

if not crab:
    print "\n\nDO NOT RUN ON CRAB OR GC WITH THIS SETTING!!!!!!\n\n"

syncfile=options.isSync                  # False

#print genFilterString

print "\n\n"

electronIsoCone="03"
useGsf = True
minleptons=1

if not includereco:
    minleptons=2

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

if not isMC and not is2011: # https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagJetProbabilityCalibration?redirectedfrom=CMS.SWGuideBTagJetProbabilityCalibration#Calibration_in_52x_and_53x_Data
    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("BTagTrackProbability2DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_2D_2012DataTOT_v1_offline"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU")),
        cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_3D_2012DataTOT_v1_offline"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU"))
        )



#Options
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))


#Message Logger Stuff
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
#process.MessageLogger.suppressWarning=['particleFlowDisplacedVertexCandidate','The interpolated laser correction is <= zero! (0). Using 1. as correction factor.']
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery

process.GlobalTag.globaltag = globalTag + '::All'
#process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( inputFiles ), skipEvents=cms.untracked.uint32( skipEvents ) )
process.out    = cms.OutputModule("PoolOutputModule", outputCommands =  cms.untracked.vstring(), fileName = cms.untracked.string( outputFile + '_PatTuple') )

################# Input script (some default one for crab
#if syncfile or crab:
process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( '/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S10_START53_V7A-v1/0000/A89D210D-1BE2-E111-9EFB-0030487F1797.root' ))

#if not (syncfile or crab):     
if not inputScript=='':
    process.load(inputScript)


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32 (maxEvents) )

#########JSON###
# limit to json file (if passed as parameter)
realdata = True
if isMC:
    realdata=False
if realdata and not (json=="nojson"):
    import os
    jsonpath = os.environ['CMSSW_BASE']+'/src/TtZAnalysis/Data/data/'+json 

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


process.noscraping = cms.EDFilter("FilterOutScraping",
                                applyfilter = cms.untracked.bool(True),
                                debugOn = cms.untracked.bool(False),
                                numtrack = cms.untracked.uint32(10),
                                thresh = cms.untracked.double(0.25)
                                )
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')


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
if isMC:
    ##
    if genFilter=='Top':
        process.load('TopAnalysis.TopFilter.filters.GeneratorTopFilter_cfi')
        process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")
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

        process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                 process.topsequence *
                                                 process.postCutPUInfo *
                                                 process.pfLeps *
                                                 process.allLeps *
                                                 process.requireMinLeptons
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
                                                     process.postCutPUInfo *
                                                     process.pfLeps *
                                                     process.allLeps *
                                                     process.requireMinLeptons)
        else:
            process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                     process.generatorZFilter *
                                                     process.postCutPUInfo*
                                                     process.pfLeps *
                                                     process.allLeps *
                                                     process.requireMinLeptons)
            
   
    else:
        process.preFilterSequence = cms.Sequence(process.pfLeps *
                                                 process.allLeps *
                                                 process.preCutPUInfo * 
                                                 process.requireMinLeptons)# *
                                                 #process.postCutPUInfo)
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

    process.preFilterSequence = cms.Sequence(process.HBHENoiseFilter *
                                             process.noscraping *
                                             process.pfLeps *
                                             process.allLeps *
                                             process.preCutPUInfo *
                                             process.requireMinLeptons)
    if not is2011:
        process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
        getattr(process, 'preFilterSequence').replace(process.preCutPUInfo,
                                                      process.ecalLaserCorrFilter *
                                                      process.preCutPUInfo)

    
    




##########Do primary vertex filtering###


from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector

process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlinePrimaryVertices')
    )




###########default pat and pf2pat########
# pat sequence
process.load("PhysicsTools.PatAlgos.patSequences_cff")
### this is necessary to avoid the conflict between PAT and RECO configurations
### see: https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1357.html
process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.doAreaFastjet = True
process.kt6PFJets.voronoiRfact = 0.9
process.kt6PFJets.Rho_EtaMax   = cms.double( 4.4)

if isMC:
    jetCorr =('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute'])
else:
    jetCorr = ('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute', 'L2L3Residual'])



from PhysicsTools.PatAlgos.tools.pfTools import *
usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=isMC, postfix=pfpostfix, jetCorrections=jetCorr, pvCollection=cms.InputTag('goodOfflinePrimaryVertices'),typeIMetCorrections=True) 


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
######SUSY STUFF####### adapt Muons to get pfIso in reco muons BRUTAL but fast..elecs the same


from TtZAnalysis.Workarounds.usePFIsoCone import *

usePFIsoCone(process)

process.EIdSequence = cms.Sequence()

from WWAnalysis.SkimStep.electronIDs_cff import addElectronIDs
eidModules = addElectronIDs(process,process.EIdSequence)

getattr(process,'patDefaultSequence').replace(process.patElectrons,
                                        process.EIdSequence *
                                        process.patElectrons)

for module in eidModules:
    setattr(process.patElectrons.electronIDSources,module.label(),cms.InputTag(module.label()))

if not isMC:
    removeMCMatching( process)

##now use standard default patsequence



################Jets########## 

###break top projection and repair taus
process.pfJetsForTaus = getattr(process,'pfJets'+pfpostfix).clone()
getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'pfJets'+pfpostfix),
                                                       getattr(process,'pfJets'+pfpostfix) *
                                                       process.pfJetsForTaus)

getattr(process,'pfJets'+pfpostfix).src='pfNoPileUp'+pfpostfix
getattr(process,'patJets'+pfpostfix).jetSource = 'pfJets'+pfpostfix

#getattr(process,'patPF2PATSequence'+pfpostfix).remove(getattr(process,'pfJets'+pfpostfix))
#getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'pfNoPileUp'+pfpostfix),
#                                                       getattr(process,'pfNoPileUp'+pfpostfix) *
#                                                       getattr(process,'pfJets'+pfpostfix))
                                                       


##just for taus
getattr(process,'pfNoJet'+pfpostfix).bottomCollection = 'pfNoPileUp'+pfpostfix

massSearchReplaceAnyInputTag((getattr(process, 'pfTauSequence'+pfpostfix)),cms.InputTag('pfJets'+pfpostfix), cms.InputTag('pfJetsForTaus'),True)
getattr(process,'pfNoTau'+pfpostfix).bottomCollection = cms.InputTag('pfJetsForTaus')


###### make isolation cut invalid

getattr(process,'pfIsolatedElectrons'+pfpostfix).isolationCut = 99999 
getattr(process,'pfIsolatedMuons'+pfpostfix).isolationCut = 99999 

####### Fix not automatically implemented b-tagging modules/vertices:

# process.load('RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff')

######### build jets for rho value ## only if 2011 rho is used (hopefully not for that long..)

process.kt6PFJetsForIso = process.kt6PFJets.clone( Rho_EtaMax = cms.double(2.5))
process.kt6PFJetsForIsoNoPU = process.kt6PFJetsForIso.clone( src = "pfNoPileUp"+ pfpostfix)

process.isoJetSequence = cms.Sequence(process.kt6PFJetsForIso * process.kt6PFJetsForIsoNoPU)

### for met correction (2011 data) ## warning!! postfix hardcoded!

process.kt6PFJetsPFlow = process.kt6PFJets.clone()
if is2011:
    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patJets'+pfpostfix),
                                                           process.kt6PFJetsPFlow *
                                                           getattr(process,'patJets'+pfpostfix))


###### Match triggers to leptons

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )

process.patGSFElectronsTriggerMatches = cms.EDProducer("PATTriggerMatcherDRDPtLessByR",
    matchedCuts = cms.string('path("*")'),
    src = cms.InputTag("patElectrons"),
    maxDPtRel = cms.double(0.5),
    resolveByMatchQuality = cms.bool(True),
    maxDeltaR = cms.double(0.5),
    resolveAmbiguities = cms.bool(True),
    matched = cms.InputTag("patTrigger")
)
process.patPFElectronsTriggerMatches = process.patGSFElectronsTriggerMatches.clone()
process.patPFElectronsTriggerMatches.src = 'patPFElectrons'+pfpostfix



process.patMuonsTriggerMatches = process.patGSFElectronsTriggerMatches.clone()
process.patMuonsTriggerMatches.src = 'patMuons'

process.patGSFElectronsWithTrigger = cms.EDProducer("PATTriggerMatchElectronEmbedder",
    src = cms.InputTag("patElectrons"),
    matches = cms.VInputTag("patGSFElectronsTriggerMatches")
                  )
process.patPFElectronsWithTrigger = process.patGSFElectronsWithTrigger.clone()
process.patPFElectronsWithTrigger.src = "patPFElectrons"+pfpostfix
process.patPFElectronsWithTrigger.matches = ['patPFElectronsTriggerMatches']

process.patMuonsWithTrigger = cms.EDProducer("PATTriggerMatchMuonEmbedder",
    src = cms.InputTag("patMuons"),
    matches = cms.VInputTag("patMuonsTriggerMatches")
                  )
if includereco:
    process.triggerMatches =  cms.Sequence(process.patGSFElectronsTriggerMatches *
                                           process.patPFElectronsTriggerMatches *
                                           process.patMuonsTriggerMatches *
                                           process.patGSFElectronsWithTrigger *
                                           process.patPFElectronsWithTrigger *
                                           process.patMuonsWithTrigger)
    process.patTriggerSequence = cms.Sequence(process.patTrigger)

else:
    process.triggerMatches =  cms.Sequence()
    process.patTriggerSequence = cms.Sequence()

###### Merge SuperClusters

process.superClusters = cms.EDProducer("SuperClusterMerger",
                                       src=cms.VInputTag( cms.InputTag("correctedHybridSuperClusters") , cms.InputTag("correctedMulti5x5SuperClustersWithPreshower") ) )


####### some jet and ID cuts to speed up and keep treesize low. The leptons are only produced to filter, the tree is filled with leptons without cuts!

process.treeJets = process.selectedPatJets.clone()
process.treeJets.src="patJets"+pfpostfix
process.treeJets.cut = 'pt>8' # unfortunately starting at 10 GeV are needed for MET rescaling

process.IDMuons = process.selectedPatMuons.clone()
process.IDMuons.src = 'patMuons'
process.IDMuons.cut = cms.string('pt > 18  && abs(eta) < 2.7')

process.IDElectrons = process.selectedPatElectrons.clone()
process.IDElectrons.src = 'patElectrons'
process.IDElectrons.cut = cms.string( 'pt > 18  && abs(eta) < 2.7')

process.IDPFElectrons = process.selectedPatElectrons.clone()
process.IDPFElectrons.src = 'patPFElectrons'+pfpostfix
process.IDPFElectrons.cut = cms.string( 'pt > 18  && abs(eta) < 2.7')


process.MuonGSFMerge = cms.EDProducer("CandViewMerger",
                                     src = cms.VInputTag(cms.InputTag("IDMuons"),  cms.InputTag("IDElectrons"))
                                     )

process.MuonPFMerge = cms.EDProducer("CandViewMerger",
                                     src = cms.VInputTag(cms.InputTag("IDMuons"),  cms.InputTag("IDPFElectrons"))
                                     )


process.filterIDLeptons = cms.EDFilter("SimpleCounter",
                                       src = cms.VInputTag(cms.InputTag("IDMuons"),  
                                                           cms.InputTag("IDElectrons"),   
                                                           cms.InputTag("IDPFElectrons"),
                                                           cms.InputTag("MuonGSFMerge"),
                                                           cms.InputTag("MuonPFMerge")),
                                       minNumber = cms.uint32(minleptons)
                                       )

process.IDLeptonFilterSequence = cms.Sequence(process.IDMuons *
                                              process.IDElectrons *
                                              process.IDPFElectrons *
                                              process.MuonGSFMerge *
                                              process.MuonPFMerge *
                                              process.filterIDLeptons)

getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                       getattr(process,'patMuons'+pfpostfix) *
                                                       process.IDLeptonFilterSequence)
                                                       

########## Prepare Tree ##

process.load('TtZAnalysis.TreeWriter.susytreewriter_cff')

process.PFTree.metSrc            = 'patMETs'+pfpostfix
process.PFTree.includeTrigger    = includetrigger
process.PFTree.includeReco       = includereco
process.PFTree.rhoJetsIsoNoPu    = cms.InputTag("kt6PFJetsForIsoNoPU","rho",process.name_())
process.PFTree.rhoJetsIso        = cms.InputTag("kt6PFJetsForIso","rho",process.name_())
process.PFTree.includePDFWeights = includePDFWeights
process.PFTree.pdfWeights        = "pdfWeights:"+PDF
if not includereco:
    process.PFTree.muonSrc = 'patMuons'
    process.PFTree.elecGSFSrc =  'patElectrons'
    process.PFTree.elecPFSrc =  'patPFElectrons' + pfpostfix

## make tree sequence

process.treeSequence = cms.Sequence(process.triggerMatches *
                                    process.superClusters *
                                    process.treeJets *
                                    process.PFTree)



##### does this prevent segfaults?




###### Path

process.path = cms.Path(process.goodOfflinePrimaryVertices *
                      #  process.inclusiveVertexing *   ## segfaults?!?! in the newest release or MC
                        process.preFilterSequence *
                      #  process.btagging *             #not yet implemented fully in pf2pat sequence../ needed for new btagging tag
                        process.patTriggerSequence *
                        process.patDefaultSequence *
                        getattr(process,'patPF2PATSequence'+pfpostfix) *
                        process.isoJetSequence *
                        process.treeSequence)

#### plug in vertex filter and change collections

process.outpath    = cms.EndPath()




