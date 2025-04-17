#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "g4root.hh"
#include "RunAction.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"

class MyEventAction : public G4UserEventAction
{
public:
	MyEventAction(MyRunAction*);
	~MyEventAction();
	
	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);
	
	//Function to add step wise energy deposition
	void AddEnergyDep(G4double edep){fEnergyDeposition += edep;}
private:
	G4double fEnergyDeposition;
};

#endif
