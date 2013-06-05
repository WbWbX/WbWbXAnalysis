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
                          
                                                    

                          includeTrigger = cms.bool(True),
                          triggerResults = cms.InputTag('TriggerResults','','HLT'),
                          triggerEvent = cms.InputTag('patTriggerEvent'),
                          
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


                          debugmode = cms.bool(False),

#produceGenLevelBJetsPlusHadron
                          BJetIndex     = cms.InputTag("produceGenLevelBJetsPlusHadron", "BJetIndex"),
                          AntiBJetIndex = cms.InputTag("produceGenLevelBJetsPlusHadron", "AntiBJetIndex"),
                          BHadJetIndex = cms.InputTag("produceGenLevelBJetsPlusHadron", "BHadJetIndex"),
                          AntiBHadJetIndex = cms.InputTag("produceGenLevelBJetsPlusHadron", "AntiBHadJetIndex"),
                          BHadrons = cms.InputTag("produceGenLevelBJetsPlusHadron", "BHadrons"),
                          AntiBHadrons = cms.InputTag("produceGenLevelBJetsPlusHadron", "AntiBHadrons"),
                          BHadronFromTopB = cms.InputTag("produceGenLevelBJetsPlusHadron", "BHadronFromTopB"),
                          AntiBHadronFromTopB = cms.InputTag("produceGenLevelBJetsPlusHadron", "AntiBHadronFromTopB"),
                          BHadronVsJet = cms.InputTag("produceGenLevelBJetsPlusHadron", "BHadronVsJet"),
                          AntiBHadronVsJet = cms.InputTag("produceGenLevelBJetsPlusHadron", "AntiBHadronVsJet"),
                          genBHadPlusMothers = cms.InputTag("produceGenLevelBJetsPlusHadron","genBHadPlusMothers"),
                          genBHadPlusMothersIndices = cms.InputTag("produceGenLevelBJetsPlusHadron","genBHadPlusMothersIndices"),
                          genBHadIndex = cms.InputTag("produceGenLevelBJetsPlusHadron","genBHadIndex"),
                          genBHadFlavour = cms.InputTag("produceGenLevelBJetsPlusHadron","genBHadFlavour"),
                          genBHadJetIndex = cms.InputTag("produceGenLevelBJetsPlusHadron","genBHadJetIndex"),

                          useBHadrons = cms.bool(False)

                          )
