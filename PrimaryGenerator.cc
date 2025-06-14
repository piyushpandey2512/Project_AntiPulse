#include "PrimaryGenerator.hh"
#include "G4Event.hh"
#include <random>
#include "GeoConstruction.hh"
#include "G4RunManager.hh"

// Add these as class members in PrimaryGenerator.hh or as static/global for quick testing
bool useThreeSourceCone = false;
bool useConeSourceTowardScintillator = false;
bool useMoireSource = true;

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
    G4cout << "[PrimaryGenerator] Generating event: " << anEvent->GetEventID() << G4endl;

    // Define the particle type
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* piPlus  = particleTable->FindParticle("pi+");
    G4ParticleDefinition* piMinus = particleTable->FindParticle("pi-");
    G4ParticleDefinition* piZero  = particleTable->FindParticle("pi0");
    G4ParticleDefinition* kPlus   = particleTable->FindParticle("kaon+");
    G4ParticleDefinition* kMinus  = particleTable->FindParticle("kaon-");

    // --- 3-source cone emission with correct particle mix and energies ---
    if (useThreeSourceCone) {
        std::vector<G4ThreeVector> sourcePositions = {
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm + 49.9 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm - 49.9 * cm)
        };

        std::vector<G4ThreeVector> moduleCenters = {
            G4ThreeVector(15.8*cm, 0, 45*cm),
            G4ThreeVector(23.8*cm, 0, 45*cm),
            G4ThreeVector(15.8*cm, 0, -45*cm),
            G4ThreeVector(23.8*cm, 0, -45*cm)
        };

        G4ThreeVector avgModuleCenter(0, 0, 0);
        for (const auto& module : moduleCenters) {
            avgModuleCenter += module;
        }
        avgModuleCenter /= moduleCenters.size();

        G4double coneHalfAngle = 70.0 * deg;

        // 3D box dimensions
        G4double boxX = 7.0 * cm;
        G4double boxY = 7.0 * cm;
        G4double boxZ = 250.0 * micrometer;

        for (const auto& sourceCenter : sourcePositions) {
            // Random position within the box centered at sourceCenter
            G4double rx = (G4UniformRand() - 0.5) * boxX;
            G4double ry = (G4UniformRand() - 0.5) * boxY;
            G4double rz = (G4UniformRand() - 0.5) * boxZ;
            G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

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
    }

    if (useConeSourceTowardScintillator) {
        // Scintillator center
        G4ThreeVector scintillatorCenter(10.0 * cm, 0, 0);

        // Source position 10 cm before the scintillator along -x
        G4ThreeVector sourcePos = scintillatorCenter - G4ThreeVector(10.0 * cm, 0, 0);
        fParticleGun->SetParticlePosition(sourcePos);



        // Cone emission around +x direction (toward scintillator)
        // Max cone half-angle in radians (~68.2 degrees)
        G4double thetaMax = std::atan(std::sqrt(std::pow(0.3 * cm, 2) + std::pow(25 * cm, 2)) / (10.0 * cm));

        // Random direction within cone
        G4double cosTheta = std::cos(thetaMax) + (1 - std::cos(thetaMax)) * G4UniformRand();  // cone sampling
        G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
        G4double phi = 2.0 * CLHEP::pi * G4UniformRand();
        G4ThreeVector direction(sinTheta * std::cos(phi),
                                sinTheta * std::sin(phi),
                                cosTheta);  // direction in local (+x) frame

        // Rotate direction to align with +x axis
        G4RotationMatrix rotToX;
        rotToX.rotateY(-90.0 * deg); // align default +z cone to +x
        direction = rotToX * direction;

        fParticleGun->SetParticleDefinition(piPlus); // Set particle type
        fParticleGun->SetParticleMomentumDirection(direction.unit());
        fParticleGun->SetParticleEnergy(240 * MeV);
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }


    if (useMoireSource) {
        std::vector<G4ThreeVector> sourcePositions = {
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm + 50 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm - 50 * cm)
        };

        G4double boxX = 7.0 * cm;
        G4double boxY = 7.0 * cm;
        G4double boxZ = 250.0 * micrometer;

        for (const auto& sourceCenter : sourcePositions) {
            // Random position within the box
            G4double rx = (G4UniformRand() - 0.5) * boxX;
            G4double ry = (G4UniformRand() - 0.5) * boxY;
            G4double rz = (G4UniformRand() - 0.5) * boxZ;
            G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

            // Select particle type based on annihilation ratios
            G4ParticleDefinition* particle;
            G4double energy;
            G4double rand = G4UniformRand();
            
            if (rand < 0.60) { // Charged pions (60% of total)
                particle = (G4UniformRand() < 0.5) ? piPlus : piMinus;
                energy = 230 * MeV; // Average kinetic energy
            }
            else if (rand < 0.80) { // Neutral pions (40% of total)
                particle = particleTable->FindParticle("pi0");
                energy = 230 * MeV; // Same average energy as charged
            }
            else if (rand < 0.96) { // Charged kaons (16% of total)
                particle = (G4UniformRand() < 0.5) ? kPlus : kMinus;
                energy = 150*MeV + 250*MeV*G4UniformRand(); // Uniform distribution
            }
            else { // Eta mesons (4% - neutral, not detected)
                continue; // Skip neutral particles for charged detection
            }

            // Isotropic direction and energy setup
            G4double cosTheta = 2 * G4UniformRand() - 1;
            G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
            G4double phi = 2 * M_PI * G4UniformRand();
            G4ThreeVector direction(
                sinTheta * std::cos(phi),
                sinTheta * std::sin(phi),
                cosTheta);

            // Configure particle gun
            fParticleGun->SetParticleDefinition(particle);
            fParticleGun->SetParticlePosition(sourcePos);
            fParticleGun->SetParticleMomentumDirection(direction);
            fParticleGun->SetParticleEnergy(energy);
            
            // Generate primary vertex
            fParticleGun->GeneratePrimaryVertex(anEvent);
        }
    }
}