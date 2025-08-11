#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "EventAction.hh"
#include <fstream>

class MySteppingAction : public G4UserSteppingAction
{
public:
    MySteppingAction(MyEventAction *eventAction);
    ~MySteppingAction();

    virtual void UserSteppingAction(const G4Step *);



private:
    MyEventAction *fEventAction;

    void writeToFile(
        std::ofstream& out,
        G4double x, G4double y, G4double z,
        G4double time,
        G4double energyDep,
        const G4ThreeVector& momentum,
        G4int eventID,
        G4int parentID,
        G4int stepID,
        G4int trackID,
        const G4String& processName,
        const G4String& particleName,
        const G4String& volumeName,
        G4int copyNumber,
        const G4String& inout);

        std::map<G4int, G4ThreeVector> fIntraModuleMomentumMap;
        std::map<G4int, G4ThreeVector> fInterModuleMomentumMap;
};

#endif