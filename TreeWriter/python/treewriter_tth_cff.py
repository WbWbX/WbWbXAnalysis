import FWCore.ParameterSet.Config as cms

PFTree   = cms.EDAnalyzer('TreeWriterTtH',
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

                          BJetIndex     = cms.InputTag("matchGenHFHadronJets", "BJetIndex"),
                          AntiBJetIndex = cms.InputTag("matchGenHFHadronJets", "AntiBJetIndex"),
                          BHadJetIndex = cms.InputTag("matchGenHFHadronJets", "BHadJetIndex"),
                          AntiBHadJetIndex = cms.InputTag("matchGenHFHadronJets", "AntiBHadJetIndex"),
                          BHadrons = cms.InputTag("matchGenHFHadronJets", "BHadrons"),
                          AntiBHadrons = cms.InputTag("matchGenHFHadronJets", "AntiBHadrons"),
                          BHadronFromTopB = cms.InputTag("matchGenHFHadronJets", "BHadronFromTopB"),
                          AntiBHadronFromTopB = cms.InputTag("matchGenHFHadronJets", "AntiBHadronFromTopB"),
                          BHadronVsJet = cms.InputTag("matchGenHFHadronJets", "BHadronVsJet"),
                          AntiBHadronVsJet = cms.InputTag("matchGenHFHadronJets", "AntiBHadronVsJet"),
                          genBHadPlusMothers = cms.InputTag("matchGenHFHadronJets","genBHadPlusMothers"),
                          genBHadPlusMothersIndices = cms.InputTag("matchGenHFHadronJets","genBHadPlusMothersIndices"),
                          genBHadIndex = cms.InputTag("matchGenHFHadronJets","genBHadIndex"),
                          genBHadFlavour = cms.InputTag("matchGenHFHadronJets","genBHadFlavour"),
                          genBHadJetIndex = cms.InputTag("matchGenHFHadronJets","genBHadJetIndex"),


                          useBHadrons = cms.bool(False),

                          isSusy =  cms.bool(False),
                          isJPsi = cms.bool(False),

                          debugmode = cms.bool(False),
                          triggerObjects = cms.vstring("")
                          )
