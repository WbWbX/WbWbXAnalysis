import FWCore.ParameterSet.Config as cms

PFTree   = cms.EDAnalyzer('TreeWriterTtZ',
                          treeName = cms.string('pfTree'),
                          runOnAOD = cms.bool(True),
                          muonSrc = cms.InputTag('patMuonsWithTrigger'),
                          elecGSFSrc = cms.InputTag('patGSFElectronsWithTrigger'),
                          elecPFSrc = cms.InputTag('patPFElectronsWithTrigger'),
                          keepElecIdOnly = cms.string("mvaTrigV0"),
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
                          genMode = cms.InputTag('top'),

                          debugmode = cms.bool(False),

#produceGenLevelBJetsPlusHadron matchGenHFHadronJets
                          BHadronVsJet = cms.InputTag("matchGenBCHadronBLocal", "BHadronVsJet"),
                          AntiBHadronVsJet = cms.InputTag("matchGenBCHadronBLocal", "AntiBHadronVsJet"),

                          BJetIndex     = cms.InputTag("matchGenBCHadronBLocal", "BJetIndex"),
                          AntiBJetIndex = cms.InputTag("matchGenBCHadronBLocal", "AntiBJetIndex"),
                          BHadJetIndex = cms.InputTag("matchGenBCHadronBLocal", "BHadJetIndex"),
                          AntiBHadJetIndex = cms.InputTag("matchGenBCHadronBLocal", "AntiBHadJetIndex"),
                          BHadrons = cms.InputTag("matchGenBCHadronBLocal", "BHadrons"),
                          AntiBHadrons = cms.InputTag("matchGenBCHadronBLocal", "AntiBHadrons"),
                          BHadronFromTopB = cms.InputTag("matchGenBCHadronBLocal", "BHadronFromTopB"),
                          AntiBHadronFromTopB = cms.InputTag("matchGenBCHadronBLocal", "AntiBHadronFromTopB"),
                          genBHadPlusMothers = cms.InputTag("matchGenBCHadronBLocal","genBHadPlusMothers"),
                          genBHadPlusMothersIndices = cms.InputTag("matchGenBCHadronBLocal","genBHadPlusMothersIndices"),
                          genBHadIndex = cms.InputTag("matchGenBCHadronBLocal","genBHadIndex"),
                          genBHadFlavour = cms.InputTag("matchGenBCHadronBLocal","genBHadFlavour"),
                          genBHadJetIndex = cms.InputTag("matchGenBCHadronBLocal","genBHadJetIndex"),

                          isSusy =  cms.bool(False),
                          isJPsi = cms.bool(False),

                          useBHadrons = cms.bool(False),
                          
                          triggerObjects = cms.vstring(""),


                          additionalWeights = cms.vstring(""),

                          partonShower = cms.string("pythia6")

                          )
