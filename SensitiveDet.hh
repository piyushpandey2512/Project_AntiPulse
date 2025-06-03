#ifndef SENSITIVE_HH
#define SENSITIVE_HH

#include "G4VSensitiveDetector.hh"
#include "g4root.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
	MySensitiveDetector(G4String); // remeber the argument G4String -
	~MySensitiveDetector();
private:
	// important function of the sensitive class, to process the hits
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
};

#endif


