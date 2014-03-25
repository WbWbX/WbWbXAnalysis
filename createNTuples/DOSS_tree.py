import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

###parsing stuff###

options = VarParsing.VarParsing()

options.register ('is2011',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"is 2011 data/MC")
options.register ('crab',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"uses crab")
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
options.register ('includereco',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes info for eff studies")
options.register ('includetrigger',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes full trigger string info for event")
options.register ('includePDF',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"includes pdf weights info for event")
options.register ('PDF','cteq66',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"pdf set for weights")
options.register ('inputScript','TtZAnalysis.Configuration.samples.mc.TTJets_MassiveBinDECAY_TuneZ2star_8TeV_madgraph_tauola_Summer12_DR53X_PU_S10_START53_V7A_v1_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register ('json','nojson',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"json files")
options.register ('isSync',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"switch on for sync")
options.register('oppoQ', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "require opposite charge leptons (for data and BG only)")
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

options.register ('muPOGTest',False,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"FastSim")


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
    #os.environ['LHAPATH']=newlha



pfpostfix='PFlow' #unfortunately some things don't seem to work otherwise.. pfjets get lost somehow produces LOADS of overhead...

process = cms.Process("Yield")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')


if isMC:
    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("BTagTrackProbability2DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_2D_MC53X_v2"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU")),
        cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
                 tag = cms.string("TrackProbabilityCalibration_3D_MC53X_v2"),
                 connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_BTAU"))
        )

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

################# Input script (some default one for crab

process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( '/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S10_START53_V7A-v1/0000/A89D210D-1BE2-E111-9EFB-0030487F1797.root' ))
#process.source.skipBadFiles = cms.untracked.bool( True )

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
if realdata and not (options.json=="nojson"):
    import os
    jsonpath = options.json 
    print "using json:"
    print jsonpath
    #import FWCore.PythonUtilities.LumiList as LumiList
    #import FWCore.ParameterSet.Types as CfgTypes
    #myLumis = LumiList.LumiList(filename = jsonpath).getCMSSWString().split(',')
    #process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
    #process.source.lumisToProcess.extend(myLumis)
    import FWCore.PythonUtilities.LumiList as LumiList
    process.source.lumisToProcess = LumiList.LumiList(filename = jsonpath).getVLuminosityBlockRange()



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
process.PUInfo.vertexSrc='offlinePrimaryVertices'

process.preCutPUInfo = process.PUInfo.clone()
process.preCutPUInfo.treeName = 'preCutPUInfo'

process.postCutPUInfo = process.PUInfo.clone()
process.postCutPUInfo.treeName = 'PUTreePostCut'
process.postCutPUInfo.includePDFWeights = False


############pre filters to speed up cmsRun
## run before any "large" algos are run

## just rewrite:

# produce collections
process.makeVLLeps = cms.Sequence()

# loose pfMuons

process.pfMus = cms.EDFilter("PdgIdPFCandidateSelector",
    pdgId = cms.vint32(-13, 13),
    src = cms.InputTag("particleFlow")
)
process.makeVLLeps+=process.pfMus
process.pfEs = cms.EDFilter("PdgIdPFCandidateSelector",
    pdgId = cms.vint32(-11, 11),
    src = cms.InputTag("particleFlow")
)
process.makeVLLeps+=process.pfEs

process.requireMinLeptons = cms.EDFilter("LeptonPreFilter", #filters on any opposite charge combination between muons and/or electrons
                                         Asrc = cms.VInputTag(cms.InputTag('pfMus'),cms.InputTag('muons'),cms.InputTag('pfEs'),cms.InputTag('gsfElectrons')),
                                         Bsrc = cms.VInputTag(),
                                         debug = cms.bool(debug),
                                         minPt =  cms.double(8)
                                         )

if(includetrigger):
    process.requireMinLeptons.minPt=3

#############


if (not isMC) or ("ttbarbg" in outputFile) or ((not isSignal) and includetrigger):
     if "_emu" in outputFile :
         print "emu preselection mode"
         process.requireMinLeptons.Asrc = cms.VInputTag(cms.InputTag('pfMus'),cms.InputTag('muons'))
         process.requireMinLeptons.Bsrc = cms.VInputTag(cms.InputTag('pfEs'),cms.InputTag('gsfElectrons'))
     if "_mumu" in outputFile:
         print "mumu preselection mode"
         process.requireMinLeptons.Asrc = cms.VInputTag(cms.InputTag('pfMus'),cms.InputTag('muons'))
         process.requireMinLeptons.Bsrc = cms.VInputTag()
     if "_ee" in outputFile:
         print "ee preselection mode"
         process.requireMinLeptons.Asrc = cms.VInputTag(cms.InputTag('pfEs'),cms.InputTag('gsfElectrons'))
         process.requireMinLeptons.Bsrc = cms.VInputTag()

#if not (includetrigger or includereco):


process.requireRecoLeps =  cms.Sequence(process.makeVLLeps *
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
        #process.produceGenLevelBJetsPlusHadron.doImprovedHadronMatching = True
        # process.produceGenLevelBJetsPlusHadron.doValidationPlotsForImprovedHadronMatching = False
        process.load("TopAnalysis.TopUtils.sequences.improvedJetHadronQuarkMatching_cff")
        process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
    

        
        process.load("TopAnalysis.TopUtils.GenLevelBJetProducer_cfi")
        process.produceGenLevelBJets.deltaR = 5.0
        process.produceGenLevelBJets.noBBbarResonances = True
        
        process.load("TopAnalysis.TopUtils.GenHFHadronMatcher_cff")
        process.matchGenHFHadronJets.flavour = 5
        process.matchGenHFHadronJets.noBBbarResonances = True
        
        process.load("TopAnalysis.TopUtils.sequences.improvedJetHadronQuarkMatching_cff")

        useBHadrons=True
# process.produceGenLevelBJetsPlusHadron = process.matchGenHFHadronJets.clone()
        if isSignal:
            process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                 process.topsequence *
                                                 process.postCutPUInfo *
                                                 process.improvedJetHadronQuarkMatchingSequence *
                                                 process.produceGenLevelBJets *
                                                 process.matchGenHFHadronJets
                                                 )
        else:
            process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                     process.topsequence *
                                                     process.postCutPUInfo 
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
                                                     process.postCutPUInfo )
        else:
            process.preFilterSequence = cms.Sequence(process.preCutPUInfo * 
                                                     process.generatorZFilter *
                                                     process.postCutPUInfo)
            
            
    else:
        process.preFilterSequence = cms.Sequence(process.preCutPUInfo )
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
    

    process.preFilterSequence = cms.Sequence(process.preCutPUInfo)

#    if not is2011:
#        process.load('RecoMET.METFilters.ecalLaserCorrFilter_cfi')
#        getattr(process, 'preFilterSequence').replace(process.preCutPUInfo,
#                                                      process.ecalLaserCorrFilter *
#                                                      process.preCutPUInfo)

    
    

#from PhysicsTools.PatAlgos.patTemplate_cfg import *

############All filters################
filtervertices=True
if isSignal:
    filtervertices=False


#process.filtersSeq = cms.Sequence()

process.load("TopAnalysis.ZTopUtils.metFilters_cff")
process.noscraping = cms.EDFilter(
    "FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
    )
if isMC:
    process.filtersSeq = cms.Sequence(process.ignoreMetFilters)
else:
    process.filtersSeq = cms.Sequence(process.noscraping * process.metFilters)

if is2011:
    process.filtersSeq = cms.Sequence(
        process.primaryVertexFilter *
        #process.goodVertices *
        process.noscraping *
        process.HBHENoiseFilter 
        )


    
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
usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=isMC, postfix=pfpostfix, jetCorrections=jetCorr, pvCollection=cms.InputTag('goodOfflinePrimaryVertices'),typeIMetCorrections=True) 


if not is2011:
    getattr(process,'patJetCorrFactors'+pfpostfix).rho=cms.InputTag("kt6PFJets","rho","RECO")

process.pfPileUp.checkClosestZVertex = False

##################################
#  MVA eID
##################################
process.load('EgammaAnalysis.ElectronTools.electronIdMVAProducer_cfi')
process.eidMVASequence = cms.Sequence(  process.mvaTrigV0 )
getattr(process,'patElectrons'+pfpostfix).electronIDSources.mvaTrigV0    = cms.InputTag("mvaTrigV0")
getattr(process, 'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patElectrons'+pfpostfix),
                                                        process.eidMVASequence *
                                                        getattr(process,'patElectrons'+pfpostfix)
                                                        )

##########Energy corrections
process.load("EgammaAnalysis.ElectronTools.electronRegressionEnergyProducer_cfi")
process.eleRegressionEnergy.inputElectronsTag= 'gsfElectrons'
process.eleRegressionEnergy.inputCollectionType=0
process.eleRegressionEnergy.useRecHitCollections=True
process.eleRegressionEnergy.produceValueMaps=True

process.load("EgammaAnalysis.ElectronTools.calibratedElectrons_cfi")
process.calibratedElectrons.isMC=isMC
if isMC:
    process.calibratedElectrons.inputDataset = "Summer12_LegacyPaper"

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
                                                   calibratedElectrons = cms.PSet(
        initialSeed = cms.untracked.uint32(123456789),
        engineName = cms.untracked.string('TRandom3')
        )
                                                   )

process.calibratedElectrons.updateEnergyError = cms.bool(True)
process.calibratedElectrons.correctionsType = cms.int32(2)
process.calibratedElectrons.combinationType = cms.int32(3)


process.elecEnergyCalibration = cms.Sequence(
    process.eleRegressionEnergy *
    process.calibratedElectrons
    )



getattr(process,'patElectrons'+pfpostfix).electronIDSources = cms.PSet(mvaTrigV0 = cms.InputTag("mvaTrigV0"))




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

##energy corrections
process.load("TopAnalysis.ZTopUtils.correctmuonenergy_cff")
process.correctMuonEnergy.muonSrc = 'pfMuonsFromVertex'+pfpostfix
process.correctMuonEnergy.muonType = "pfMuons"
process.correctMuonEnergy.isMC = isMC
process.correctMuonEnergy.debug=False

process.correctRecoMuonEnergy=process.correctMuonEnergy.clone()
process.correctRecoMuonEnergy.muonSrc = 'muons'
process.correctRecoMuonEnergy.muonType = "recoMuons"


getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'pfMuonsFromVertex'+pfpostfix),
                                                       getattr(process,'pfMuonsFromVertex'+pfpostfix) *
                                                       process.correctMuonEnergy)


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
    adaptPFIsoMuons( process, applyPostfix(process,"patMuonsPFlow",""), 'PFIso2', "03")
    getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMuons'+pfpostfix),
                                                           getattr(process,'muIsoSequencetwo') *
                                                           getattr(process,'patMuons'+pfpostfix))
    

    process.patMuonsPFlow.useParticleFlow = cms.bool(False)
    process.patMuonsPFlow.pfMuonSource = cms.InputTag("particleFlow")
    process.muonMatchPFlow.src     = "muons"

    
if options.muPOGTest:
    process.pfMuonsFromVertexPFlow.d0Cut = cms.double(1000.2)
    process.pfMuonsFromVertexPFlow.dzCut = cms.double(1000.5)

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

################################################
########################
################################################
################################################
########################

#MVA met

process.load('RecoMET.METPUSubtraction.mvaPFMET_leptons_cff') 
#process.mvametSequence = = cms.Sequence( process.pfMEtMVAsequence) 
process.calibratedAK5PFJetsForPFMEtMVA.src = 'pfJets' +pfpostfix
if isMC:
    process.calibratedAK5PFJetsForPFMEtMVA.correctors= cms.vstring("ak5PFL1FastL2L3")
else:
    process.calibratedAK5PFJetsForPFMEtMVA.correctors= cms.vstring("ak5PFL1FastL2L3Residual")

process.pfMEtMVA.srcUncorrJets='pfJets' +pfpostfix
process.pfMEtMVA.srcVertices = 'goodOfflinePrimaryVertices'
process.pfMEtMVA.inputFileNames = cms.PSet(
    U     = cms.FileInPath('RecoMET/METPUSubtraction/data/gbrmet_53_June2013_type1.root'),
    DPhi  = cms.FileInPath('RecoMET/METPUSubtraction/data/gbrmetphi_53_June2013_type1.root'),
    CovU1 = cms.FileInPath('RecoMET/METPUSubtraction/data/gbru1cov_53_Dec2012.root'),
    CovU2 = cms.FileInPath('RecoMET/METPUSubtraction/data/gbru2cov_53_Dec2012.root')
    )

process.pfMEtMVA.srcLeptons=cms.VInputTag("isomuons","isoelectrons","isotaus") #should be adapted to analysis selection.. 

process.pfMEtMVA.srcRho = cms.InputTag("kt6PFJets","rho","RECO")
process.patMEtMVA= getattr(process,'patMETs'+pfpostfix).clone()
process.patMEtMVA.metSource = 'pfMEtMVA'




#### other mets
process.mets = cms.Sequence()
process.load("JetMETCorrections.Type1MET.pfMETCorrections_cff")
process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")


#Not in 53X
process.pfMEtSysShiftCorrParameters_2012runABCDvsNvtx_data = cms.PSet( # CV: ReReco data + Summer'13 JEC
    px = cms.string("+4.83642e-02 + 2.48870e-01*Nvtx"),
    py = cms.string("-1.50135e-01 - 8.27917e-02*Nvtx")
    )

process.pfMEtSysShiftCorrParameters_2012runABCDvsNvtx_mc = cms.PSet( # CV: Summer'12 MC + Summer'13 JEC
    px = cms.string("+1.62861e-01 - 2.38517e-02*Nvtx"),
    py = cms.string("+3.60860e-01 - 1.30335e-01*Nvtx")
    )

if isMC:
    process.pfJetMETcorr.jetCorrLabel = "ak5PFL1FastL2L3"
    process.pfMEtSysShiftCorr.parameter = process.pfMEtSysShiftCorrParameters_2012runABCDvsNvtx_mc
else:
    process.pfJetMETcorr.jetCorrLabel = "ak5PFL1FastL2L3Residual"
    process.pfMEtSysShiftCorr.parameter = process.pfMEtSysShiftCorrParameters_2012runABCDvsNvtx_data

process.pfMetT0T1Txy=process.pfType1CorrectedMet.clone()
process.pfMetT0T1Txy.applyType0Corrections = cms.bool(True)

process.pfMetT0T1Txy.srcType1Corrections = cms.VInputTag(
    cms.InputTag('pfJetMETcorr', 'type1') ,
    cms.InputTag('pfMEtSysShiftCorr')
    )


process.pfMetT0T1=process.pfType1CorrectedMet.clone()
process.pfMetT0T1.applyType0Corrections = cms.bool(True)

process.pfMetT1Txy=process.pfType1CorrectedMet.clone()
#process.pfMetT1Txy.src=cms.InputTag('pfMEtSysShiftCorr')
process.pfMetT1Txy.applyType0Corrections = cms.bool(False)
process.pfMetT1Txy.srcType1Corrections = cms.VInputTag(
    cms.InputTag('pfJetMETcorr', 'type1'),
    cms.InputTag('pfMEtSysShiftCorr')

    )

process.pfType1CorrectedMet=process.pfType1CorrectedMet.clone()
process.pfType1CorrectedMet.applyType0Corrections = cms.bool(False)
process.pfType1CorrectedMet.srcType1Corrections = cms.VInputTag(
    cms.InputTag('pfJetMETcorr', 'type1') ,

    )

#process.patPFMetPFlow

process.patpfMetT1      = getattr(process,'patPFMet'+pfpostfix).clone()
process.patpfMetT0T1Txy = getattr(process,'patPFMet'+pfpostfix).clone()
process.patpfMetT0T1    = getattr(process,'patPFMet'+pfpostfix).clone()
process.patpfMetT1Txy   = getattr(process,'patPFMet'+pfpostfix).clone()
process.patpfMetT1.metSource = 'pfType1CorrectedMet'
process.patpfMetT0T1Txy.metSource = 'pfMetT0T1Txy'
process.patpfMetT0T1.metSource = 'pfMetT0T1'
process.patpfMetT1Txy.metSource = 'pfMetT1Txy'


process.mets+=process.pfMEtMVAsequence 
process.mets+=process.patMEtMVA
process.mets+=process.pfMEtSysShiftCorrSequence
process.mets+=process.producePFMETCorrections
process.mets+=process.pfMetT0T1Txy
process.mets+=process.pfMetT0T1
process.mets+=process.pfMetT1Txy
process.mets+=process.patpfMetT1
process.mets+=process.patpfMetT0T1Txy
process.mets+=process.patpfMetT0T1
process.mets+=process.patpfMetT1Txy

getattr(process,'patPF2PATSequence'+pfpostfix).replace(getattr(process,'patMETs'+pfpostfix),
                                                       process.mets*
                                                       getattr(process,'patMETs'+pfpostfix))


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
                                        requireOppoQ = cms.bool(options.oppoQ),
                                        minNumber = cms.uint32(minleptons),
                                        debug=cms.bool(debug)
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




#process.PFTree.vertexSrc         = 'goodVertices' #use standard
process.PFTree.metSrc            = 'patMETs'+pfpostfix
process.PFTree.mvaMetSrc          = 'patMEtMVA'
process.PFTree.metT1Src            = 'patpfMetT1'
process.PFTree.metT0T1TxySrc        ='patpfMetT0T1Txy'
process.PFTree.metT0T1Src           ='patpfMetT0T1'
process.PFTree.metT1TxySrc          ='patpfMetT1Txy'
process.PFTree.includeTrigger    = includetrigger
process.PFTree.includeReco       = includereco
process.PFTree.includePDFWeights = includePDFWeights
process.PFTree.pdfWeights        = "pdfWeights:"+PDF
process.PFTree.includeGen        = isSignal
process.PFTree.useBHadrons       = useBHadrons
process.PFTree.isJPsi            = jpsi

if isSignal and genFilter=="Top":
    process.PFTree.genJets           = 'ak5GenJetsPlusHadron'


if not includereco:
    process.PFTree.muonSrc = 'patMuons' + pfpostfix
    process.PFTree.elecGSFSrc =  'patElectrons' + pfpostfix
    process.PFTree.elecPFSrc =  'patPFElectrons' + pfpostfix

if includereco:
    process.PFTree.muonSrc = 'patMuonsWithTrigger'
    process.PFTree.elecGSFSrc =  'patGSFElectronsWithTrigger'
    process.PFTree.elecPFSrc =  'patPFElectronsWithTrigger'



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
process.dump=cms.EDAnalyzer('EventContentAnalyzer')

process.path = cms.Path( 
    #process.dump *
    process.elecEnergyCalibration *
    process.correctRecoMuonEnergy * #pfMuon eneergy is hidden in pf2pat sequence
    process.preFilterSequence *
    process.goodOfflinePrimaryVertices *
    process.filtersSeq *
    getattr(process,'patPF2PATSequence'+pfpostfix) *
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

massSearchReplaceAnyInputTag(process.path,cms.InputTag("gsfElectrons",""), cms.InputTag("calibratedElectrons" , "calibratedGsfElectrons"),True)

process.calibratedElectrons.inputElectronsTag = cms.InputTag('gsfElectrons','','RECO')
process.eleRegressionEnergy.inputElectronsTag = cms.InputTag('gsfElectrons','','RECO')

massSearchReplaceAnyInputTag(process.path,cms.InputTag("pfMuonsFromVertex"+pfpostfix), cms.InputTag("correctMuonEnergy"),True)
process.correctMuonEnergy.muonSrc = 'pfMuonsFromVertex'+pfpostfix

massSearchReplaceAnyInputTag(process.path,cms.InputTag('muons'), cms.InputTag("correctRecoMuonEnergy"),True)
process.correctRecoMuonEnergy.muonSrc = 'muons'

###USE calibrated gsf at the bottom (masssearchandreplace.)

if realdata and not (options.json=="nojson"):
    print "\n\n\n\nrunning on lumis: "
    print process.source.lumisToProcess

print "\nglobal Tag: " + globalTag

if isPrompt:
    print "prompt reco not supported anymore! exit"
    exit(8888)


copyjson=False
if(hasattr(process.source,'lumisToProcess')):
	lumisToProcess=process.source.lumisToProcess
	copyjson=True
process.load(inputScript)
if copyjson:
	process.source.lumisToProcess=lumisToProcess
