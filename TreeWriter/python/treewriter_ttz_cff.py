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
                          metT1Src   =cms.InputTag('patMETs'),
                          metT0T1TxySrc        =cms.InputTag('patMETs'),
                          metT0T1Src           =cms.InputTag('patMETs'),
                          metT1TxySrc          =cms.InputTag('patMETs'),
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
                          
                          includePDFWeights = cms.bool(False),
                          pdfWeights = cms.InputTag(''),

                          includeGen = cms.bool(False),
                          genParticles = cms.InputTag('genParticles'),
                          genJets = cms.InputTag('ak5GenJetsNoNu'),


                          debugmode = cms.bool(False),

#produceGenLevelBJetsPlusHadron matchGenHFHadronJets
                          BHadronVsJet = cms.InputTag("matchGenBCHadronB", "BHadronVsJet"),
                          AntiBHadronVsJet = cms.InputTag("matchGenBCHadronB", "AntiBHadronVsJet"),

                          BJetIndex     = cms.InputTag("matchGenBCHadronB", "BJetIndex"),
                          AntiBJetIndex = cms.InputTag("matchGenBCHadronB", "AntiBJetIndex"),
                          BHadJetIndex = cms.InputTag("matchGenBCHadronB", "BHadJetIndex"),
                          AntiBHadJetIndex = cms.InputTag("matchGenBCHadronB", "AntiBHadJetIndex"),
                          BHadrons = cms.InputTag("matchGenBCHadronB", "BHadrons"),
                          AntiBHadrons = cms.InputTag("matchGenBCHadronB", "AntiBHadrons"),
                          BHadronFromTopB = cms.InputTag("matchGenBCHadronB", "BHadronFromTopB"),
                          AntiBHadronFromTopB = cms.InputTag("matchGenBCHadronB", "AntiBHadronFromTopB"),
                          genBHadPlusMothers = cms.InputTag("matchGenBCHadronB","genBHadPlusMothers"),
                          genBHadPlusMothersIndices = cms.InputTag("matchGenBCHadronB","genBHadPlusMothersIndices"),
                          genBHadIndex = cms.InputTag("matchGenBCHadronB","genBHadIndex"),
                          genBHadFlavour = cms.InputTag("matchGenBCHadronB","genBHadFlavour"),
                          genBHadJetIndex = cms.InputTag("matchGenBCHadronB","genBHadJetIndex"),

                          isSusy =  cms.bool(False),
                          isJPsi = cms.bool(False),

                          useBHadrons = cms.bool(False),
                          
                          triggerObjects = cms.vstring(""),


                          additionalWeights = cms.vstring("")

                          )
