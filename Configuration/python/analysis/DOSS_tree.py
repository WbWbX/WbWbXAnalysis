import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

###parsing stuff###

options = VarParsing.VarParsing()

options.register ('is2011',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is 2011 data/MC")
options.register ('globalTag','START52_V9',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"global tag")
options.register ('reportEvery',1000,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"report every")
options.register ('outputFile','def_out',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register ('isMC',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is MC")
options.register ('genFilter','none',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"gen Filter")
options.register ('genFilterString','none',VarParsing.VarParsing.multiplicity.list,VarParsing.VarParsing.varType.string,"gen Filter selection string")
options.register ('genFilterInvert',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"invert gen Filter")
options.register ('includereco',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes info for eff studies")
options.register ('includetrigger',True,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes trigger info for event")
options.register ('includePDF',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes pdf weights info for event")
options.register ('PDF','cteq65',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")
options.register ('inputScript','TtZAnalysis.Configuration.samples.mc.DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register ('json','nojson',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"json file in cern afs")
options.register ('isSync',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"switch on for sync")
options.register('samplename', 'standard', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which sample to run over - obsolete")

options.parseArguments()

is2011=options.is2011                    # def false
globalTag=options.globalTag              # START52_V9
reportEvery=options.reportEvery          # 1000
outputFile=options.outputFile            # def_out
isMC=options.isMC                        # True
genFilter=options.genFilter              # 
genFilterString=options.genFilterString  # 
genFilterInvert=options.genFilterInvert  # False
includereco=options.includereco          # False
includetrigger=options.includetrigger    # True
includePDFWeights=options.includePDF     # False
PDF=options.PDF                          # cteq65
inputScript=options.inputScript          # TopAnalysis.Configuration.samples.singlemu_runA_prompt_cff
json=options.json                        # uses cern afs dqm directory ,you must add: Collisions12/8TeV/Prompt/

syncfile=options.isSync                  # False

#print genFilterString

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


pfpostfix='PFlow' #unfortunately some things don't seem to work otherwise.. pfjets get lost somehow produces LOADS of overhead...

process = cms.Process("Yield")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')


#Options
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))


#Message Logger Stuff
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery

process.GlobalTag.globaltag = globalTag + '::All'
#process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( inputFiles ), skipEvents=cms.untracked.uint32( skipEvents ) )
process.out    = cms.OutputModule("PoolOutputModule", outputCommands =  cms.untracked.vstring(), fileName = cms.untracked.string( outputFile + '_PatTuple') )

################# Input script
if syncfile:
    process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( '/store/mc/Summer12/TTJets_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S7_START52_V5-v1/0000/FEBE99BB-3881-E111-B1F3-003048D42DC8.root' ))

else:     
    if inputScript=='':
        print "need input script"
        exit(8888)
    
    process.load(inputScript)

#########JSON###
# limit to json file (if passed as parameter)
realdata = True
if isMC:
    realdata=False
if realdata and not (json=="nojson"):
    jsonpath = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/' + json 
    import FWCore.PythonUtilities.LumiList as LumiList
    import FWCore.ParameterSet.Types as CfgTypes
    myLumis = LumiList.LumiList(filename = jsonpath).getCMSSWString().split(',')
    process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
    process.source.lumisToProcess.extend(myLumis)




#### TFile Service
outFileName = 'tree_' + outputFile + '.root'

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
        process.load('TopAnalysis.TopFilter.GeneratorTopFilter_cfi')
        process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")
        process.decaySubset.fillMode = "kME" # Status3, use kStable for Status2     
        process.topsequence = cms.Sequence( 
            process.makeGenEvt *
            process.generatorTopFilter 
            )
        process.generatorTopFilter.channels = genFilterString
        process.generatorTopFilter.invert_selection=genFilterInvert

        process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                 process.topsequence *
                                                 process.postCutPUInfo *
                                                 process.pfLeps *
                                                 process.allLeps *
                                                 process.requireMinLeptons
                                                 )

    elif genFilter=='Z':
        process.load('TopAnalysis.TopFilter.filters.GeneratorZFilter_cfi')
        if genFilterString=='ElectronElectron':
            process.generatorZFilter.zDecayModes = cms.vint32(11)
        elif genFilterString=='MuonMuon':
            process.generatorZFilter.zDecayModes = cms.vint32(13)
        elif genFilterString=='TauTau':
            process.generatorZFilter.zDecayModes = cms.vint32(15)
        elif genFilterString=='Leptonic':
            process.generatorZFilter.zDecayModes = cms.vint32(11,13,15)
        
        if genFilterInvert:
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

#### for data ###
else:
    
    process.preFilterSequence = cms.Sequence(process.HBHENoiseFilter *
                                             process.noscraping *
                                             process.pfLeps *
                                             process.allLeps *
                                             process.requireMinLeptons)




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

#new for 5_2_X not yet implmented in default pf2pat

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

if useGsf:
    from TtZAnalysis.Workarounds.useGsfElectrons import *

    useGsfElectronsInPF2PAT(process,pfpostfix,electronIsoCone)
    # bugfixes made in pfTools and ofIsolation



else:
    getattr(process,'patElectrons'+pfpostfix).isolationValues = cms.PSet(
        pfChargedHadrons = cms.InputTag("elPFIsoValueCharged"+electronIsoCone+"PFId"),
        pfChargedAll = cms.InputTag("elPFIsoValueChargedAll"+electronIsoCone+"PFId"),
        pfPUChargedHadrons = cms.InputTag("elPFIsoValuePU"+electronIsoCone+"PFId"),
        pfNeutralHadrons = cms.InputTag("elPFIsoValueNeutral"+electronIsoCone+"PFId"),
        pfPhotons = cms.InputTag("elPFIsoValueGamma"+electronIsoCone+"PFId")
        )

######### end of electron implementation ########

################Jets########## 

###break top projection and repair taus
process.pfJetsForTaus = getattr(process,'pfJets'+pfpostfix).clone()
getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'pfJets'+pfpostfix),
                                                       getattr(process,'pfJets'+pfpostfix) *
                                                       process.pfJetsForTaus)

getattr(process,'pfJets'+pfpostfix).src='pfNoPileUp'+pfpostfix
getattr(process,'patJets'+pfpostfix).jetSource = 'pfJets'+pfpostfix
getattr(process,'pfNoJet'+pfpostfix).bottomCollection = 'pfNoPileUp'+pfpostfix

massSearchReplaceAnyInputTag((getattr(process, 'pfTauSequence'+pfpostfix)),cms.InputTag('pfJets'+pfpostfix), cms.InputTag('pfJetsForTaus'),True)
getattr(process,'pfNoTau'+pfpostfix).bottomCollection = cms.InputTag('pfJetsForTaus')


###### make isolation cut invalid

getattr(process,'pfIsolatedElectrons'+pfpostfix).isolationCut = 99999 ## not even necessary if gsf electrons are used..
getattr(process,'pfIsolatedMuons'+pfpostfix).isolationCut = 99999 

####### Fix not automatically implemented b-tagging modules/vertices:

process.load('RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff')

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

process.patElectronsTriggerMatches = cms.EDProducer("PATTriggerMatcherDRDPtLessByR",
    matchedCuts = cms.string('path("*")'),
    src = cms.InputTag("patElectrons"+ pfpostfix),
    maxDPtRel = cms.double(0.5),
    resolveByMatchQuality = cms.bool(True),
    maxDeltaR = cms.double(0.5),
    resolveAmbiguities = cms.bool(True),
    matched = cms.InputTag("patTrigger")
)
process.patMuonsTriggerMatches = process.patElectronsTriggerMatches.clone()
process.patMuonsTriggerMatches.src = 'patMuons'+ pfpostfix

process.patElectronsWithTrigger = cms.EDProducer("PATTriggerMatchElectronEmbedder",
    src = cms.InputTag("patElectrons"+ pfpostfix),
    matches = cms.VInputTag("patElectronsTriggerMatches")
                  )

process.patMuonsWithTrigger = cms.EDProducer("PATTriggerMatchMuonEmbedder",
    src = cms.InputTag("patMuons"+ pfpostfix),
    matches = cms.VInputTag("patMuonsTriggerMatches")
                  )
if includereco:
    process.triggerMatches =  cms.Sequence(process.patElectronsTriggerMatches *
                                           process.patMuonsTriggerMatches *
                                           process.patElectronsWithTrigger *
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
process.treeJets.cut = 'pt>10' # unfortunately starting at 10 GeV are needed for MET rescaling

process.IDMuons = process.selectedPatMuons.clone()
process.IDMuons.src = 'patMuons' + pfpostfix
process.IDMuons.cut = cms.string('pt > 18  && abs(eta) < 2.7')

process.IDElectrons = process.selectedPatElectrons.clone()
process.IDElectrons.src = 'patElectrons' + pfpostfix
process.IDElectrons.cut = cms.string( 'pt > 18  && abs(eta) < 2.7')

process.IDLeptons = cms.EDProducer("CandViewMerger",
                                     src = cms.VInputTag(cms.InputTag("IDMuons"),  cms.InputTag("IDElectrons"))
                                     )



process.filterIDLeptons = cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag('IDLeptons'),
                                       minNumber = cms.uint32(minleptons)
                                       )

process.IDLeptonFilterSequence = cms.Sequence(process.IDMuons *
                                              process.IDElectrons *
                                              process.IDLeptons *
                                              process.filterIDLeptons)

getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                       getattr(process,'patMuons'+pfpostfix) *
                                                       process.IDLeptonFilterSequence)
                                                       

########## Prepare Tree ##

process.load('TtZAnalysis.TreeWriter.treewriter_cff')

process.PFTree.metSrc            = 'patMETs'+pfpostfix
process.PFTree.includeTrigger    = includetrigger
process.PFTree.includeReco       = includereco
process.PFTree.rhoJetsIsoNoPu    = cms.InputTag("kt6PFJetsForIsoNoPU","rho",process.name_())
process.PFTree.rhoJetsIso        = cms.InputTag("kt6PFJetsForIso","rho",process.name_())
process.PFTree.useGsfElecs       = useGsf
process.PFTree.includePDFWeights = includePDFWeights
process.PFTree.pdfWeights        = "pdfWeights:"+PDF
if not includereco:
    process.PFTree.muonSrc = 'patMuons' + pfpostfix
    process.PFTree.elecSrc =  'patElectrons' + pfpostfix

## make tree sequence

process.treeSequence = cms.Sequence(process.triggerMatches *
                                    process.superClusters *
                                    process.treeJets *
                                    process.PFTree)






###### Path

process.path = cms.Path(process.goodOfflinePrimaryVertices *
                        process.preFilterSequence *
                        process.inclusiveVertexing *
                        process.btagging *             #not yet implemented fully in pf2pat sequence..
                        process.patTriggerSequence *
                        getattr(process,'patPF2PATSequence'+pfpostfix) *
                        process.isoJetSequence *
                        process.treeSequence)

#### plug in vertex filter and change collections

process.outpath    = cms.EndPath()






#### do a lot of cleanup

#process.path.remove(process.phPFIsoValueCharged03PFId * process.phPFIsoValueChargedAll03PFId * process.phPFIsoValueGamma03PFId * process.phPFIsoValueNeutral03PFId * process.phPFIsoValuePU03PFId)
#process.path.remove(process.muPFIsoValueCharged03 * process.muPFIsoValueChargedAll03 * process.muPFIsoValueGamma03 * process.muPFIsoValueNeutral03 * process.muPFIsoValueGammaHighThreshold03 * process.muPFIsoValueNeutralHighThreshold03 * process.muPFIsoValuePU03)
#process.path.remove(process.elPFIsoValueCharged04PFId * process.elPFIsoValueChargedAll04PFId * process.elPFIsoValueGamma04PFId * process.elPFIsoValueNeutral04PFId * process.elPFIsoValuePU04PFId)
#process.path.remove(process.elPFIsoValueCharged04PFIdPFIso * process.elPFIsoValueChargedAll04PFIdPFIso * process.elPFIsoValueGamma04PFIdPFIso * process.elPFIsoValueNeutral04PFIdPFIso * process.elPFIsoValuePU04PFIdPFIso)
#process.path.remove(process.elPFIsoValueCharged04NoPFIdPFIso * process.elPFIsoValueChargedAll04NoPFIdPFIso * process.elPFIsoValueGamma04NoPFIdPFIso * process.elPFIsoValueNeutral04NoPFIdPFIso * process.elPFIsoValuePU04NoPFIdPFIso)

#process.path.remove(process.selectedPatElectrons * process.selectedPatMuons * process.selectedPatTaus * process.selectedPatJets * process.selectedPatPFParticles * process.selectedPatCandidateSummary * process.countPatElectrons * process.countPatMuons * process.countPatTaus * process.countPatLeptons * process.countPatJets * process.countPatPFParticles)
