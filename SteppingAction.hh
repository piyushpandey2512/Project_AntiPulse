#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

#include "GeoConstruction.hh"
#include "EventAction.hh"

class MySteppingAction : public G4UserSteppingAction
{
public:
	MySteppingAction(MyEventAction *eventAction);
	~MySteppingAction();
	
	virtual void UserSteppingAction(const G4Step*);
	
private:
	MyEventAction *fEventAction;

	void Handle5TVolume(const G4Step* step, G4AnalysisManager* analysisManager);
    void Handle1TVolume(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleDegrader(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleBoreHole(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandlePlasticSupport(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandlePrimaryCoil(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleCorrectionCoil(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleLHeVessel(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleInnerLnVessel(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleOuterLnVessel(const G4Step* step, G4AnalysisManager* analysisManager);
    void HandleOuterCryostatWall(const G4Step* step, G4AnalysisManager* analysisManager);
};

#endif
