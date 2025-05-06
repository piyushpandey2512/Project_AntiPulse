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
    std::map<G4int, G4ThreeVector> entryPointMap1; // Right-side front
    std::map<G4int, G4ThreeVector> entryPointMap2; // Left-side front
};

#endif