#include "PrimaryGenerator.hh"
#include "G4Event.hh"
#include <random>
#include "GeoConstruction.hh"
#include "G4RunManager.hh"

// Add these as class members in PrimaryGenerator.hh or as static/global for quick testing
bool useThreeSourceCone = false;
bool useSingleSourceX = false;
bool useSingleSourceIsotropic = false;
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
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm + 49 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm - 49 * cm)
        };

        std::vector<G4ThreeVector> moduleCenters = {
            G4ThreeVector(15 * cm, 0, 45 * cm),
            G4ThreeVector(23 * cm, 0, 45 * cm),
            G4ThreeVector(15 * cm, 0, -45 * cm),
            G4ThreeVector(23 * cm, 0, -45 * cm)
        };

        G4ThreeVector avgModuleCenter(0, 0, 0);
        for (const auto& module : moduleCenters) {
            avgModuleCenter += module;
        }
        avgModuleCenter /= moduleCenters.size();

        G4double coneHalfAngle = 70.0 * deg;

        for (const auto& sourcePos : sourcePositions) {
            // Generate 3 charged pions (π+ or π-)
            for (int i = 0; i < 3; ++i) {
                G4ParticleDefinition* pion = (G4UniformRand() < 0.5) ? piPlus : piMinus;

                // Cone direction logic
                G4ThreeVector coneAxis = (avgModuleCenter - sourcePos).unit();
                G4double cosTheta = std::cos(coneHalfAngle);
                G4double randomCosTheta = cosTheta + (1 - cosTheta) * G4UniformRand();
                G4double sinTheta = std::sqrt(1 - randomCosTheta * randomCosTheta);
                G4double phi = 2 * M_PI * G4UniformRand();
                G4ThreeVector randomDirection(
                    sinTheta * std::cos(phi),
                    sinTheta * std::sin(phi),
                    randomCosTheta);
                G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

                fParticleGun->SetParticleDefinition(pion);
                fParticleGun->SetParticlePosition(sourcePos);
                fParticleGun->SetParticleMomentumDirection(finalDirection);
                fParticleGun->SetParticleEnergy(230 * MeV);
                fParticleGun->GeneratePrimaryVertex(anEvent);
            }

            // Generate 2 neutral pions (π0)
            for (int i = 0; i < 2; ++i) {
                G4ThreeVector coneAxis = (avgModuleCenter - sourcePos).unit();
                G4double cosTheta = std::cos(coneHalfAngle);
                G4double randomCosTheta = cosTheta + (1 - cosTheta) * G4UniformRand();
                G4double sinTheta = std::sqrt(1 - randomCosTheta * randomCosTheta);
                G4double phi = 2 * M_PI * G4UniformRand();
                G4ThreeVector randomDirection(
                    sinTheta * std::cos(phi),
                    sinTheta * std::sin(phi),
                    randomCosTheta);
                G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

                fParticleGun->SetParticleDefinition(piZero);
                fParticleGun->SetParticlePosition(sourcePos);
                fParticleGun->SetParticleMomentumDirection(finalDirection);
                fParticleGun->SetParticleEnergy(230 * MeV);
                fParticleGun->GeneratePrimaryVertex(anEvent);
            }

            // Generate kaons (K+ or K-) with 6% probability
            if (G4UniformRand() < 0.06) {
                G4ParticleDefinition* kaon = (G4UniformRand() < 0.5) ? kPlus : kMinus;

                G4ThreeVector coneAxis = (avgModuleCenter - sourcePos).unit();
                G4double cosTheta = std::cos(coneHalfAngle);
                G4double randomCosTheta = cosTheta + (1 - cosTheta) * G4UniformRand();
                G4double sinTheta = std::sqrt(1 - randomCosTheta * randomCosTheta);
                G4double phi = 2 * M_PI * G4UniformRand();
                G4ThreeVector randomDirection(
                    sinTheta * std::cos(phi),
                    sinTheta * std::sin(phi),
                    randomCosTheta);
                G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

                G4double kaonEnergy = 150 * MeV + G4UniformRand() * (400 * MeV - 150 * MeV);

                fParticleGun->SetParticleDefinition(kaon);
                fParticleGun->SetParticlePosition(sourcePos);
                fParticleGun->SetParticleMomentumDirection(finalDirection);
                fParticleGun->SetParticleEnergy(kaonEnergy);
                fParticleGun->GeneratePrimaryVertex(anEvent);
            }
        }
    }


    // --- Single source, isotropic emission ---
    if (useSingleSourceIsotropic) {
        G4ThreeVector sourcePos(0, 0, 0); // or any desired position
        fParticleGun->SetParticlePosition(sourcePos);

        G4double cosTheta = 2 * G4UniformRand() - 1;
        G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
        G4double phi = 2 * M_PI * G4UniformRand();
        G4ThreeVector direction(sinTheta * std::cos(phi),
                                sinTheta * std::sin(phi),
                                cosTheta);

        fParticleGun->SetParticleMomentumDirection(direction);
        fParticleGun->SetParticleEnergy(240 * MeV);
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }


    if (useMoireSource) {
        // 3 source boxes as in your original code
        std::vector<G4ThreeVector> sourcePositions = {
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm + 49 * cm),
            G4ThreeVector(-8.0 * cm, 3.5 * cm, 8.0 * cm - 49 * cm)
        };

        G4double boxX = 7.0 * cm;
        G4double boxY = 7.0 * cm;
        G4double boxZ = 250.0 * micrometer;

        // Number of secondaries per annihilation event (3 π±, 2 π0, ~0.3 K±)
        // For each source box, generate 5 pions (3 charged, 2 neutral) and kaons with 6% probability per event

        for (const auto& sourceCenter : sourcePositions) {
            // Generate 3 charged pions
            for (int i = 0; i < 3; ++i) {
                // Random position in box
                G4double rx = (G4UniformRand() - 0.5) * boxX;
                G4double ry = (G4UniformRand() - 0.5) * boxY;
                G4double rz = (G4UniformRand() - 0.5) * boxZ;
                G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

                // Randomly choose pi+ or pi-
                G4ParticleDefinition* pion = (G4UniformRand() < 0.5) ? piPlus : piMinus;

                // Isotropic direction
                G4double cosTheta = 2 * G4UniformRand() - 1;
                G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
                G4double phi = 2 * M_PI * G4UniformRand();
                G4ThreeVector direction(
                    sinTheta * std::cos(phi),
                    sinTheta * std::sin(phi),
                    cosTheta);

                fParticleGun->SetParticleDefinition(pion);
                fParticleGun->SetParticlePosition(sourcePos);
                fParticleGun->SetParticleMomentumDirection(direction);
                fParticleGun->SetParticleEnergy(230 * MeV); // average kinetic energy
                fParticleGun->GeneratePrimaryVertex(anEvent);
            }

            // Generate 2 neutral pions
            for (int i = 0; i < 2; ++i) {
                G4double rx = (G4UniformRand() - 0.5) * boxX;
                G4double ry = (G4UniformRand() - 0.5) * boxY;
                G4double rz = (G4UniformRand() - 0.5) * boxZ;
                G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

                G4double cosTheta = 2 * G4UniformRand() - 1;
                G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
                G4double phi = 2 * M_PI * G4UniformRand();
                G4ThreeVector direction(
                    sinTheta * std::cos(phi),
                    sinTheta * std::sin(phi),
                    cosTheta);

                fParticleGun->SetParticleDefinition(piZero);
                fParticleGun->SetParticlePosition(sourcePos);
                fParticleGun->SetParticleMomentumDirection(direction);
                fParticleGun->SetParticleEnergy(230 * MeV); // average kinetic energy
                fParticleGun->GeneratePrimaryVertex(anEvent);
            }

            // Generate kaons with 6% probability per event per source box
            if (G4UniformRand() < 0.06) {
                G4double rx = (G4UniformRand() - 0.5) * boxX;
                G4double ry = (G4UniformRand() - 0.5) * boxY;
                G4double rz = (G4UniformRand() - 0.5) * boxZ;
                G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

                G4ParticleDefinition* kaon = (G4UniformRand() < 0.5) ? kPlus : kMinus;

                G4double cosTheta = 2 * G4UniformRand() - 1;
                G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
                G4double phi = 2 * M_PI * G4UniformRand();
                G4ThreeVector direction(
                    sinTheta * std::cos(phi),
                    sinTheta * std::sin(phi),
                    cosTheta);

                // Uniform kinetic energy between 150 and 400 MeV
                G4double kaonEnergy = 150 * MeV + G4UniformRand() * (400 * MeV - 150 * MeV);

                fParticleGun->SetParticleDefinition(kaon);
                fParticleGun->SetParticlePosition(sourcePos);
                fParticleGun->SetParticleMomentumDirection(direction);
                fParticleGun->SetParticleEnergy(kaonEnergy);
                fParticleGun->GeneratePrimaryVertex(anEvent);
            }
        }
    }

}