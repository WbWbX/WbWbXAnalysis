import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


options = VarParsing.VarParsing()

options.register ('inputScript','TtZAnalysis.Configuration.samples.mc.TTJets_MassiveBinDECAY_TuneZ2star_8TeV_madgraph_tauola_Summer12_DR53X_PU_S10_START53_V7A_v1_cff',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register ('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register ('outputFile','def_out',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")


import sys


if hasattr(sys, "argv"):
    options.parseArguments()


process = cms.Process("GENINFO")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
#process.MessageLogger.suppressWarning=['particleFlowDisplacedVertexCandidate','The interpolated laser correction is <= zero! (0). Using 1. as correction factor.']
process.MessageLogger.cerr.FwkReport.reportEvery = 10000



process.GlobalTag.globaltag = 'START53_V22' + '::All'
process.out    = cms.OutputModule("PoolOutputModule", outputCommands =  cms.untracked.vstring(), fileName = cms.untracked.string( options.outputFile + '_PatTuple') )

process.source = cms.Source('PoolSource',fileNames=cms.untracked.vstring( '/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S10_START53_V7A-v1/0000/A89D210D-1BE2-E111-9EFB-0030487F1797.root' ))
    
if not options.inputScript=='':
    process.load(options.inputScript)


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32 (-1) )
if options.skipEvents > 0:
    process.source.skipEvents = cms.untracked.uint32(options.skipEvents)

outFileName = options.outputFile + '.root'

process.TFileService = cms.Service("TFileService",
    fileName = cms.string(outFileName)
)

process.GENInfoTree = cms.EDAnalyzer('genInfoTreeWriter')

process.path = cms.Path( process.GENInfoTree )

process.outpath    = cms.EndPath()
