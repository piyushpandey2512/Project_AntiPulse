#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"

class MyRunAction;

class MyEventAction : public G4UserEventAction
{
public:
    MyEventAction(MyRunAction*);
    ~MyEventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
    void AddEnergyDep(G4double edep) { fEnergyDeposition += edep; }

    void SetFrontPoint(const G4ThreeVector& front);
    void SetBackPoint(const G4ThreeVector& back);
    
private:
    G4double fEnergyDeposition;

    // For angular deviation
    G4ThreeVector fFrontPosition;
    G4ThreeVector fBackPosition;
    G4bool hasFront = false;
    G4bool hasBack = false;
};

#endif
