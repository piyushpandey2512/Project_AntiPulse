#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh" 
#include "G4Run.hh"

// #include "g4root.hh"

class MyRunAction : public G4UserRunAction
{
public:
	MyRunAction();
	~MyRunAction();
	
	// Create two method,
	//one for beginning for Run
	virtual void BeginOfRunAction(const G4Run*);
	//Another for the end of Run -
	virtual void EndOfRunAction(const G4Run* run);

	void SetOutputFileName(const G4String& name);

private:
	G4String fOutputFileName;
	void CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double min, G4double max);
};

#endif
