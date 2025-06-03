#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "EventAction.hh"

class MySteppingAction : public G4UserSteppingAction
{
public:
    MySteppingAction(MyEventAction *eventAction);
    ~MySteppingAction();

    virtual void UserSteppingAction(const G4Step *);

private:
    MyEventAction *fEventAction;

    void writeToFile(const G4String& tag, G4double x, G4double y, G4double z, G4double energyDep);
};

#endif