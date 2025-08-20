#ifndef Primary_HH

#define Primary_HH
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"

#include "G4AnalysisManager.hh"

class MyPrimaryParticles : public G4VUserPrimaryGeneratorAction

{

public:

    MyPrimaryParticles();

    ~MyPrimaryParticles();



    virtual void GeneratePrimaries(G4Event*);


private:

    G4ParticleGun* fParticleGun; // Reuse for each particle
    void GeneratePrimariesFixed(G4Event* anEvent);
    void GeneratePrimariesRandom(G4Event* anEvent);
};



#endif
