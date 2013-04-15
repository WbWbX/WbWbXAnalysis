import FWCore.ParameterSet.Config as cms

PFTree   = cms.EDAnalyzer('TreeWriterTtZ',
                          treeName = cms.string('pfTree'),
                          muonSrc = cms.InputTag('patMuonsWithTrigger'),
                          elecGSFSrc = cms.InputTag('patGSFElectronsWithTrigger'),
                          elecPFSrc = cms.InputTag('patPFElectronsWithTrigger'),
                          jetSrc = cms.InputTag('treeJets'),
                          btagAlgo = cms.string('combinedSecondaryVertexBJetTags'),
                          metSrc = cms.InputTag('patMETs'), #
                          mvaMetSrc = cms.InputTag('patMETs'), #
                          vertexSrc = cms.InputTag('goodOfflinePrimaryVertices'),
                          
                          includeReco = cms.bool(False),
                          recoMuonSrc = cms.InputTag('muons'),
                          isPFMuonCand = cms.bool(False),
                          recoElecSrc  = cms.InputTag('gsfElectrons'),
                          isPFElecCand = cms.bool(False),
                          recoTrackSrc = cms.InputTag('generalTracks'),
                          recoSuCluSrc = cms.InputTag('superClusters'),
                          
                          writeMatchedHLTObjects = cms.vstring("HLT_IsoMu24_v*",
                                                               "HLT_IsoMu24_eta2p1_v*",
                                                               "HLT_Ele27_WP80_v*",
                                                               "hltL3MuonCandidates",
                                                               "hltL3ElectronCandidates"),
                          
                                                                    

                          includeTrigger = cms.bool(True),
                          triggerResults = cms.InputTag('TriggerResults','','HLT'),
                          
                          PUInfo = cms.InputTag('addPileupInfo'),
                          rhoIso = cms.InputTag("kt6PFJets","rho", "RECO"),
                          includeRho2011 = cms.bool(True),
                          rhoJetsIsoNoPu = cms.InputTag("kt6PFJets"), #
                          rhoJetsIso = cms.InputTag("kt6PFJets"), #
                          
                          includePDFWeights = cms.bool(False),
                          pdfWeights = cms.InputTag(''),

                          includeGen = cms.bool(False),
                          genParticles = cms.InputTag('genParticles'),
                          genJets = cms.InputTag('ak5GenJetsNoNu'),


                          debugmode = cms.bool(False)
                          )
