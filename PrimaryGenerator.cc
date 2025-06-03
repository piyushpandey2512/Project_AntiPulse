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
    G4ParticleDefinition* antiProton = particleTable->FindParticle("anti_proton");
    
    // Set the particle type to be generated
    fParticleGun->SetParticleDefinition(piPlus);
    // fParticleGun->SetParticleDefinition(antiProton);

    // Define the source positions
    std::vector<G4ThreeVector> sourcePositions = {
        G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm),          // Center of STL geometry
        G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm + 50 * cm), // +50 cm along STL axis
        G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm - 50 * cm)  // -50 cm along STL axis
    };

    // Define the center of the modules
    std::vector<G4ThreeVector> moduleCenters = {
        G4ThreeVector(15 * cm, 0, 45 * cm),  // Module 1
        G4ThreeVector(23 * cm, 0, 45 * cm),  // Module 2
        G4ThreeVector(15 * cm, 0, -45 * cm), // Module 3
        G4ThreeVector(23 * cm, 0, -45 * cm)  // Module 4
    };

    // Calculate the average center of the modules
    G4ThreeVector avgModuleCenter(0, 0, 0);
    for (const auto& module : moduleCenters) {
        avgModuleCenter += module;
    }
    avgModuleCenter /= moduleCenters.size();

    // Define the cone half-angle (in radians)
    G4double coneHalfAngle = 70.0 * deg; // Adjust this value to cover all modules

    // Loop over each source
    for (const auto& sourcePos : sourcePositions) {
        fParticleGun->SetParticlePosition(sourcePos);

        // Calculate the cone axis (direction toward the average module center)
        G4ThreeVector coneAxis = (avgModuleCenter - sourcePos).unit();

        // Generate a random direction within the cone
        G4double cosTheta = std::cos(coneHalfAngle);
        G4double randomCosTheta = cosTheta + (1 - cosTheta) * G4UniformRand();
        G4double sinTheta = std::sqrt(1 - randomCosTheta * randomCosTheta);
        G4double phi = 2 * M_PI * G4UniformRand();
        G4ThreeVector randomDirection(
            sinTheta * std::cos(phi),
            sinTheta * std::sin(phi),
            randomCosTheta);

        // Rotate the random direction to align with the cone axis
        G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

        // Set the particle momentum direction
        fParticleGun->SetParticleMomentumDirection(finalDirection);

        // Set the particle energy
        fParticleGun->SetParticleEnergy(240 * MeV);

        // Generate the primary vertex
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }


    // For isotropic emission of pions.
    // Loop over each source
    /*
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
    }*/
}


/****************************************************************/

