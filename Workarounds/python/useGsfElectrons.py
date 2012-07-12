import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.tauTools import *

from PhysicsTools.PatAlgos.tools.helpers import listModules, applyPostfix

from copy import deepcopy

from PhysicsTools.PatAlgos.tools.pfTools import *


def useGsfElectronsInPF2PAT(process, postfix, dR = "04"):
    print "using Gsf Electrons in PF2PAT"
    print "WARNING: this will destory the feature of top projection which solves the ambiguity between leptons and jets because"
    print "WARNING: there will be overlap between non-PF electrons and jets even though top projection is ON!"
    print "********************* "
    module = applyPostfix(process,"patElectrons",postfix)
    module.useParticleFlow = False
    print "Building particle-based isolation for GsfElectrons in PF2PAT(PFBRECO)"
    print "********************* "
    adaptPFIsoElectrons( process, module, "PFIso"+postfix, dR )
    getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,"patElectrons"+postfix),
                                                   process.pfParticleSelectionSequence +
                                                   setupPFElectronIso(process, 'gsfElectrons', "PFIso"+postfix) +
                                                   getattr(process,"patElectrons"+postfix) )
