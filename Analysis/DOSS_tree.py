import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


globalTag='GR_R_52_V7C' #data until 195536
reportEvery=100
outputFile='SyncFile'
isMC=True
genFilter="top"
genFilterString='ElectronElectron'
genFilterInvert=False
includereco=True
includetrigger=True
inputScript='TopAnalysis.Configuration.samples.singlemu_runA_prompt_cff'
json='Cert_190456-196531_8TeV_PromptReco_Collisions12_JSON.txt'
electronIsoCone="03"
useGsf = True
minleptons=2 #leptons in prefilter and ID filter!! must be changed for includereco

syncfile=True
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
if realdata:
    jsonpath = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Prompt/' + json 
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
process.load('TtZAnalysis.TreeWriter.puinfo_cff')

process.PUInfo.includePDFWeights = False
process.PUInfo.pdfWeights = ''

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
    if genFilter=='top':
        process.load('TopAnalysis.TopFilter.GeneratorTopFilter_cfi')
        process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")
        process.decaySubset.fillMode = "kME" # Status3, use kStable for Status2     
        process.topsequence = cms.Sequence( 
            process.makeGenEvt *
            process.generatorTopFilter 
            )
        process.generatorTopFilter.channels = [genFilterString]
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
usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=isMC, postfix=pfpostfix, jetCorrections=jetCorr, pvCollection=cms.InputTag('goodOfflinePrimaryVertices')) 


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

process.triggerMatches =  cms.Sequence(process.patElectronsTriggerMatches *
                                           process.patMuonsTriggerMatches *
                                           process.patElectronsWithTrigger *
                                           process.patMuonsWithTrigger)



###### Merge SuperClusters

process.superClusters = cms.EDProducer("SuperClusterMerger",
                                       src=cms.VInputTag( cms.InputTag("correctedHybridSuperClusters") , cms.InputTag("correctedMulti5x5SuperClustersWithPreshower") ) )

####### some jet and ID cuts to keep treesize low

process.treeJets = process.selectedPatJets.clone()
process.treeJets.src="patJets"+pfpostfix
process.treeJets.cut = 'pt>25 && abs(eta) < 2.9'

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

########## Prepare Tree ##

process.load('TtZAnalysis.TreeWriter.treewriter_cff')

process.PFTree.metSrc            = 'patMETs'+pfpostfix
process.PFTree.includeTrigger    = includetrigger
process.PFTree.includeReco       = includereco
process.PFTree.rhoJetsIsoNoPu    = cms.InputTag("kt6PFJetsForIsoNoPU","rho",process.name_())
process.PFTree.rhoJetsIso        = cms.InputTag("kt6PFJetsForIso","rho",process.name_())
process.PFTree.useGsfElecs       = useGsf
process.PFTree.includePDFWeights = False
process.PFTree.pdfWeights        = ''

## make tree sequence

process.treeSequence = cms.Sequence(process.IDMuons *
                                    process.IDElectrons *
                                    process.IDLeptons *
                                    process.filterIDLeptons *
                                    process.triggerMatches *
                                    process.superClusters *
                                    process.treeJets *
                                    process.PFTree)






###### Path

process.path = cms.Path(process.goodOfflinePrimaryVertices *
                        process.preFilterSequence *
                        process.inclusiveVertexing *
                        process.btagging *             #not yet implemented fully in pf2pat sequence..
                        process.patTrigger *
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
