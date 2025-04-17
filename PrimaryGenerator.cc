#include "PrimaryGenerator.hh"
#include "G4Event.hh"

MyPrimaryParticles::MyPrimaryParticles()
{
    fParticleGun = new G4ParticleGun(1);
}

MyPrimaryParticles::~MyPrimaryParticles()
{
    delete fParticleGun;
}

void MyPrimaryParticles::GeneratePrimaries(G4Event* anEvent)
{
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  /*  // Define particles with error handling
    G4ParticleDefinition* piPlus = particleTable->FindParticle("pi+");
    G4ParticleDefinition* piMinus = particleTable->FindParticle("pi-");
    G4ParticleDefinition* piZero = particleTable->FindParticle("pi0");
    
    if (!piPlus || !piMinus || !piZero) {
        G4Exception("MyPrimaryParticles::GeneratePrimaries", "Event0101", FatalException,
                    "One or more particle definitions could not be found.");
        return;
    }

    // Generate isotropic directions and momenta
    for (auto particleDef : {piPlus, piMinus, piZero})
    {
        G4double phi = 2 * M_PI * G4UniformRand();
        G4double cosTheta = -1.0 + 2.0 * G4UniformRand();
        G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        G4ThreeVector direction(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

        G4double momentumMag = (140 * MeV) * G4UniformRand(); // Example momentum range
        G4ThreeVector momentum = momentumMag * direction;

        fParticleGun->SetParticleDefinition(particleDef);
        fParticleGun->SetParticleMomentumDirection(direction);
        fParticleGun->SetParticleMomentum(momentumMag);
        fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.)); // Origin of annihilation
        fParticleGun->GeneratePrimaryVertex(anEvent);
    } */

    G4ParticleDefinition* antiProton = particleTable->FindParticle("anti_proton");
    G4ParticleDefinition* piPlus = particleTable->FindParticle("pi+");
    G4ParticleDefinition* piMinus = particleTable->FindParticle("pi-");
    G4ParticleDefinition* piZero = particleTable->FindParticle("pi0");

    // if (!antiProton) {
    // G4Exception("MyPrimaryParticles::GeneratePrimaries", "Event0102", FatalException,
    //             "Antiproton particle definition not found.");
    // return;
    // }

    // fParticleGun->SetParticleDefinition(antiProton);
    fParticleGun->SetParticleDefinition(piPlus);
    // fParticleGun->SetParticleDefinition(piMinus);    
    // fParticleGun->SetParticleDefinition(piZero);

    G4ThreeVector direction(1.0, 0.0, 0.0);

    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->SetParticleMomentum(1.0 * keV);
    // fParticleGun->SetParticleMomentum(2.0 * MeV);
    // fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0));
    fParticleGun->SetParticlePosition(G4ThreeVector(-8.0*cm, 3.5*cm, 8.0*cm));

    fParticleGun->GeneratePrimaryVertex(anEvent);
}

