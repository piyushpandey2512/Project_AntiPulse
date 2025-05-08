#include "PrimaryGenerator.hh"
#include "G4Event.hh"
#include <random>
#include "GeoConstruction.hh"
#include "G4RunManager.hh"

MyPrimaryParticles::MyPrimaryParticles()
{
    fParticleGun = new G4ParticleGun(1);
}

MyPrimaryParticles::~MyPrimaryParticles()
{
    delete fParticleGun;
}


/*************Working Code for pions in all directions*******************/


void MyPrimaryParticles::GeneratePrimaries(G4Event* anEvent)
{
    G4cout << "[PrimaryGenerator] Generating event: " << anEvent->GetEventID() << G4endl;

    // Define the particle type
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* piPlus = particleTable->FindParticle("pi+");
    if (!piPlus) {
        G4Exception("MyPrimaryParticles::GeneratePrimaries", "Event0101", FatalException,
                    "Particle definition for pi+ could not be found.");
        return;
    }
    fParticleGun->SetParticleDefinition(piPlus);

    // Define the source positions
    std::vector<G4ThreeVector> sourcePositions = {
        G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm),          // Center of STL geometry
        G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm + 50 * cm), // +50 cm along STL axis
        G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm - 50 * cm)  // -50 cm along STL axis
    };

    // Loop over each source
    for (const auto& sourcePos : sourcePositions) {
        fParticleGun->SetParticlePosition(sourcePos);
    
        // Generate isotropic direction
        G4double cosTheta = 2 * G4UniformRand() - 1;
        G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
        G4double phi = 2 * M_PI * G4UniformRand();
        G4ThreeVector direction(sinTheta * std::cos(phi),
                                sinTheta * std::sin(phi),
                                cosTheta);
        fParticleGun->SetParticleMomentumDirection(direction);
    
        fParticleGun->SetParticleEnergy(150 * MeV);
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
}


/****************************************************************/

