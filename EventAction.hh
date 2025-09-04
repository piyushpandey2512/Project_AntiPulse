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

    void StoreIntraModuleMomentum(const G4Track* track, const G4ThreeVector& momentum);
    G4ThreeVector GetIntraModuleMomentum(const G4Track* track) const;
    
    void StoreInterModuleMomentum(const G4Track* track, const G4ThreeVector& momentum);
    G4ThreeVector GetInterModuleMomentum(const G4Track* track) const;

        // --- NEW METHODS FOR THE SINGLE SCINTILLATOR TEST ---
    void StoreSingleScintMomentum(G4int trackID, const G4ThreeVector& momentum);
    G4ThreeVector GetSingleScintMomentum(G4int trackID);

    void StoreB2BFrontMomentum(G4int trackID, const G4ThreeVector& momentum);
    G4ThreeVector GetB2BFrontMomentum(G4int trackID) const;
    
private:
    G4double fEnergyDeposition;

    // For angular deviation
    G4ThreeVector fFrontPosition;
    G4ThreeVector fBackPosition;
    G4bool hasFront = false;
    G4bool hasBack = false;

    std::map<G4int, G4ThreeVector> fIntraModuleMomentumMap;
    std::map<G4int, G4ThreeVector> fInterModuleMomentumMap;

    // --- NEW MEMBER for the single scintillator ---
    std::map<G4int, G4ThreeVector> fSingleScintMomentumMap;
    std::map<G4int, G4ThreeVector> fB2BFrontMomentumMap;

    MyRunAction* fRunAction;
};

#endif
