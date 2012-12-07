import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.tauTools import *

from PhysicsTools.PatAlgos.tools.helpers import listModules, applyPostfix

from copy import deepcopy


from PhysicsTools.PatAlgos.tools.pfTools import *

def usePFIsoCone(process, postfix = "PFIso", dRMuons = "03", dRElectrons="03")):
    print "Building particle-based isolation with cone"
    print "***************** "
    process.eleIsoSequence = setupPFElectronIso(process, 'gsfElectrons', postfix)
    process.muIsoSequence = setupPFMuonIso(process, 'muons', postfix)
    adaptPFIsoMuons( process, applyPostfix(process,"patMuons",""), postfix, dRMuons)
    adaptPFIsoElectrons( process, applyPostfix(process,"patElectrons",""), postfix, dRElectrons )
    getattr(process,'patDefaultSequence').replace( getattr(process,"patCandidates"), 
                                                   process.pfParticleSelectionSequence +
                                                   process.eleIsoSequence +
                                                   process.muIsoSequence +
                                                   getattr(process,"patCandidates") )
