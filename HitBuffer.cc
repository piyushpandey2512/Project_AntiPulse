#include "HitBuffer.hh"

std::map<G4int, HitPoint> frontHits45;
std::map<G4int, HitPoint> backHits45;
std::map<G4int, HitPoint> frontHitsMinus45;
std::map<G4int, HitPoint> backHitsMinus45;

void ClearHitBuffers() {
    frontHits45.clear();
    backHits45.clear();
    frontHitsMinus45.clear();
    backHitsMinus45.clear();
}