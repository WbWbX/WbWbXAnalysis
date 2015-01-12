#include "DataFormats/TrackReco/interface/Track.h"

namespace {
    template<typename T>
    auto getMHits_impl(T const& t) -> decltype(t.trackerExpectedHitsInner().numberOfHits()) { return t.trackerExpectedHitsInner().numberOfHits();}
    template<typename T>
    auto getMHits_impl(T const& t) -> decltype(t.hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS)) { return t.hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);}
}

int getMHits(reco::Track const & t) { return getMHits_impl(t);}

