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
        // Set particle properties for each source
        fParticleGun->SetParticlePosition(sourcePos);

        // Randomize direction within a small cone (similar to /gps/ang/maxtheta)
        G4double phi = 2 * M_PI * G4UniformRand();
        G4double theta = 1 * deg * G4UniformRand(); // Max theta = 1 degree
        G4double sinTheta = std::sin(theta);
        G4double cosTheta = std::cos(theta);
        G4ThreeVector direction(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
        fParticleGun->SetParticleMomentumDirection(direction);

        // Set particle energy to 150 MeV
        fParticleGun->SetParticleEnergy(150 * MeV);

        // Generate the primary vertex
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

