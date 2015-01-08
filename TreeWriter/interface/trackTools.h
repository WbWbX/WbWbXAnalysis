#include "DataFormats/TrackReco/interface/Track.h"

namespace {
    template<typename T>
    auto getNHits_impl(T const& t) -> decltype(t.trackerExpectedHitsInner().numberOfHits()) { return t.trackerExpectedHitsInner().numberOfHits();}
    template<typename T>
    auto getNHits_impl(T const& t) -> decltype(t.hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS)) { return t.hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS);}
}

int getNHits(reco::Track const & t) { return getNHits_impl(t);}

