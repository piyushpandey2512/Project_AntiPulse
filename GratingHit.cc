#include "GratingHit.hh"

// This line defines the thread-local allocator pointer that was declared in the header.
// It is initialized to zero (nullptr).
G4ThreadLocal G4Allocator<GratingHit>* GratingHitAllocator = 0;

// Constructor
GratingHit::GratingHit()
 : G4VHit(),      // Call the base class constructor
   fTrackID(-1),  // Initialize track ID to an invalid value
   fDetectorNb(-1) // Initialize detector number to an invalid value
{}

// Destructor
GratingHit::~GratingHit()
{}