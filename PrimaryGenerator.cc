#include "PrimaryGenerator.hh"
#include "G4Event.hh"
<<<<<<< HEAD
=======
#include <random>
#include "GeoConstruction.hh"
#include "G4RunManager.hh"
>>>>>>> 3ac4172 (Initial fresh commit)

MyPrimaryParticles::MyPrimaryParticles()
{
    fParticleGun = new G4ParticleGun(1);
}

MyPrimaryParticles::~MyPrimaryParticles()
{
    delete fParticleGun;
}

<<<<<<< HEAD
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
=======

/*************Working Code for pions in all directions*******************/


void MyPrimaryParticles::GeneratePrimaries(G4Event* anEvent)
{
    G4cout << "[PrimaryGenerator] Generating event: " << anEvent->GetEventID() << G4endl;

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* piPlus = particleTable->FindParticle("pi+");

    if (!piPlus) {
        G4Exception("MyPrimaryParticles::GeneratePrimaries", "Event0101", FatalException,
                    "Particle definition for pi+ could not be found.");
        return;
    }

    // Set the particle type
    fParticleGun->SetParticleDefinition(piPlus);

    for (auto particleDef : {piPlus})
>>>>>>> 3ac4172 (Initial fresh commit)
    {
        G4double phi = 2 * M_PI * G4UniformRand();
        G4double cosTheta = -1.0 + 2.0 * G4UniformRand();
        G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        G4ThreeVector direction(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

<<<<<<< HEAD
        G4double momentumMag = (140 * MeV) * G4UniformRand(); // Example momentum range
=======
        G4double momentumMag = (150 * MeV) * G4UniformRand(); // Example momentum range
>>>>>>> 3ac4172 (Initial fresh commit)
        G4ThreeVector momentum = momentumMag * direction;

        fParticleGun->SetParticleDefinition(particleDef);
        fParticleGun->SetParticleMomentumDirection(direction);
        fParticleGun->SetParticleMomentum(momentumMag);
<<<<<<< HEAD
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

=======
        fParticleGun->SetParticlePosition(G4ThreeVector(-8.0*cm, 3.5*cm, 8.0*cm)); // Origin of annihilation
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    /*
    // Retrieve the detector construction instance
    const MyDetectorConstruction* detectorConstruction =
        static_cast<const MyDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    // Retrieve the source positions
    const auto& sources = detectorConstruction->GetSources();

    // Random number generator for spherical coordinates
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disTheta(0, M_PI);       // Theta: 0 to π
    std::uniform_real_distribution<> disPhi(0, 2 * M_PI);     // Phi: 0 to 2π

    // Generate primary particles
    for (const auto& source : sources) {
        // Generate random direction in spherical coordinates
        G4double theta = disTheta(gen); // Polar angle
        G4double phi = disPhi(gen);    // Azimuthal angle

        // Convert spherical coordinates to Cartesian coordinates
        G4double x = std::sin(theta) * std::cos(phi);
        G4double y = std::sin(theta) * std::sin(phi);
        G4double z = std::cos(theta);
        G4ThreeVector direction(x, y, z);

        // Set the particle gun properties
        fParticleGun->SetParticleMomentumDirection(direction);
        fParticleGun->SetParticleMomentum(150.0 * MeV); // 150 MeV momentum
        fParticleGun->SetParticlePosition(source);

        // Generate the primary vertex for this direction
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    */
}


/****************************************************************/

>>>>>>> 3ac4172 (Initial fresh commit)
