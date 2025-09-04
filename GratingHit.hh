#ifndef GRATING_HIT_HH
#define GRATING_HIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Types.hh"


#include "G4VHit.hh"
#include "G4THitsCollection.hh"

// This class defines the data that will be stored for each individual hit
// in our sensitive grating/counter detectors.
class GratingHit : public G4VHit
{
public:
    // Constructor and Destructor
    GratingHit();
    virtual ~GratingHit();

    inline void* operator new(size_t);
    inline void operator delete(void*);

    // --- Member functions to set and get the data ---

    // Track ID: Identifies the particle that created the hit.
    void SetTrackID(G4int track) { fTrackID = track; }
    G4int GetTrackID() const { return fTrackID; }

    // Detector Number: Identifies which detector was hit.
    // We will use 1 for Grating 1, 2 for Grating 2, and 3 for the Solid Counter.
    void SetDetectorNb(G4int nb) { fDetectorNb = nb; }
    G4int GetDetectorNb() const { return fDetectorNb; }

private:
    // --- Data members ---
    G4int fTrackID;
    G4int fDetectorNb;
};

// This defines a "collection" or a list of GratingHit objects.
// Geant4 uses this to manage all the hits created in one event.
using GratingHitsCollection = G4THitsCollection<GratingHit>;

// This is a memory management tool that Geant4 uses to efficiently create
// and delete many instances of the GratingHit class.
extern G4ThreadLocal G4Allocator<GratingHit>* GratingHitAllocator;

// This is an inline function that allows for fast creation of new hit objects.
inline void* GratingHit::operator new(size_t)
{
    if (!GratingHitAllocator) {
        GratingHitAllocator = new G4Allocator<GratingHit>;
    }
    return (void*) GratingHitAllocator->MallocSingle();
}

// This is the corresponding inline function for fast deletion.
inline void GratingHit::operator delete(void* aHit)
{
    GratingHitAllocator->FreeSingle((GratingHit*) aHit);
}

#endif