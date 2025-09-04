#ifndef SENSITIVE_HH
#define SENSITIVE_HH

#include "G4VSensitiveDetector.hh"
#include "g4root.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

#include "GratingHit.hh"

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
	MySensitiveDetector(G4String); // remeber the argument G4String -
	~MySensitiveDetector();

	// Geant4 calls these automatically at the start/end of each event.
	virtual void Initialize(G4HCofThisEvent* hce);
	virtual void EndOfEvent(G4HCofThisEvent* hce);
private:
	// important function of the sensitive class, to process the hits
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);

	// This will point to the collection of hits for the current event.
    GratingHitsCollection* fHitsCollection;
	GratingHitsCollection* fGratingHitsCollection;
};

#endif


