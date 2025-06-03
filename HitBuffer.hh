#ifndef HITBUFFER_HH
#define HITBUFFER_HH

#include "G4ThreeVector.hh"
#include <map>

struct HitPoint {
    G4ThreeVector pos_cm;  // Position in cm
    G4int eventID;
    G4int trackID;
};

// Specific maps for top and bottom modules
extern std::map<G4int, HitPoint> frontHits45;
extern std::map<G4int, HitPoint> backHits45;
extern std::map<G4int, HitPoint> frontHitsMinus45;
extern std::map<G4int, HitPoint> backHitsMinus45;

void ClearHitBuffers();

#endif
