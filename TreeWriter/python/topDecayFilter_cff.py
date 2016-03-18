import FWCore.ParameterSet.Config as cms

topDecayFilter   = cms.EDFilter('TopDecayFilter',
                          src = cms.InputTag('genParticles'),
                          invert = cms.bool(False),
                          partonShower = cms.string("pythia8")
                          )
