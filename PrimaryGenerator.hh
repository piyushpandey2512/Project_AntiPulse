#ifndef Primary_HH

#define Primary_HH

<<<<<<< HEAD


#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"

#include "G4SystemOfUnits.hh"

#include "G4ParticleTable.hh"

#include "G4ThreeVector.hh"

#include "Randomize.hh"



=======
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"

>>>>>>> 3ac4172 (Initial fresh commit)
class MyPrimaryParticles : public G4VUserPrimaryGeneratorAction

{

public:

    MyPrimaryParticles();

    ~MyPrimaryParticles();



    virtual void GeneratePrimaries(G4Event*);


<<<<<<< HEAD

private:

    G4ParticleGun* fParticleGun; // Reuse for each particle

=======
private:

    G4ParticleGun* fParticleGun; // Reuse for each particle
    void GeneratePrimariesFixed(G4Event* anEvent);
    void GeneratePrimariesRandom(G4Event* anEvent);
>>>>>>> 3ac4172 (Initial fresh commit)
};



#endif
