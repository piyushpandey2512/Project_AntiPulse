#include "PrimaryGenerator.hh"
#include "G4Event.hh"
#include <random>
#include "G4SystemOfUnits.hh"
#include "GeoConstruction.hh"
#include "G4RunManager.hh"

using namespace std;

// Add these as class members in PrimaryGenerator.hh or as static/global for quick testing
bool useThreeSourceCone = false;
bool useConeSourceTowardScintillator = false;
bool useMoireSource = true;
bool useTestSingleSource = false;
bool useConeSourceTowardSingleModule = false;
bool useConeSourceTowardFourModules = false;

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
    if (anEvent->GetEventID() % 1000 == 0) {
        G4cout << "PrimaryGenerator: Generating event " << anEvent->GetEventID() << G4endl;
    }

    // Define the particle type
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* piPlus  = particleTable->FindParticle("pi+");
    G4ParticleDefinition* piMinus = particleTable->FindParticle("pi-");
    G4ParticleDefinition* piZero  = particleTable->FindParticle("pi0");
    G4ParticleDefinition* kPlus   = particleTable->FindParticle("kaon+");
    G4ParticleDefinition* kMinus  = particleTable->FindParticle("kaon-");




/***********************************************************************/

    // --- 3-source cone emission with correct particle mix and energies ---
    if (useThreeSourceCone) {
        // G4ThreeVector stlPosition(-8.0 * cm, 3.5 * cm, 8.0 * cm);
        G4ThreeVector stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);   // Adjusted for better alignment with modules
        // G4ThreeVector stlPosition(0, 0, 0); // For testing, using origin


        // Define three fixed source positions
        std::vector<G4ThreeVector> sourcePositions = {
            stlPosition,
            stlPosition + G4ThreeVector(0, 0, 50.0 * cm),
            stlPosition + G4ThreeVector(0, 0, -50.0 * cm)
        };

        // Calculate average module center for cone axis
        // std::vector<G4ThreeVector> moduleCenters = {
        //     G4ThreeVector(15.8*cm, 0, 45*cm),
        //     G4ThreeVector(23.8*cm, 0, 45*cm),
        //     G4ThreeVector(15.8*cm, 0, -45*cm),
        //     G4ThreeVector(23.8*cm, 0, -45*cm)
        // };

        std::vector<G4ThreeVector> moduleCenters = {
            G4ThreeVector(10 * cm, 0, 30 * cm),
            G4ThreeVector(20 * cm, 0, 30 * cm),
            G4ThreeVector(10 * cm, 0, -30 * cm),
            G4ThreeVector(20 * cm, 0, -30 * cm)
        };

        G4ThreeVector avgModuleCenter(0, 0, 0);
        for (const auto& module : moduleCenters) {
            avgModuleCenter += module;
        }
        avgModuleCenter /= moduleCenters.size();

        G4double coneHalfAngle = 110.0 * deg;

        for (const auto& sourceCenter : sourcePositions) {
            // Set position
            fParticleGun->SetParticlePosition(sourceCenter);

            // Select particle type based on annihilation ratios (same as useMoireSource)
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

            // Emit in cone toward module center
            G4ThreeVector coneAxis = (avgModuleCenter - sourceCenter).unit();

            G4double cosTheta = std::cos(coneHalfAngle);
            G4double randomCosTheta = cosTheta + (1 - cosTheta) * G4UniformRand();
            G4double sinTheta = std::sqrt(1 - randomCosTheta * randomCosTheta);
            G4double phi = 2 * M_PI * G4UniformRand();
            G4ThreeVector randomDirection(
                sinTheta * std::cos(phi),
                sinTheta * std::sin(phi),
                randomCosTheta);

            G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

            fParticleGun->SetParticleDefinition(particle);
            fParticleGun->SetParticleMomentumDirection(finalDirection.unit());
            fParticleGun->SetParticleEnergy(energy);
            fParticleGun->GeneratePrimaryVertex(anEvent);
        }
    }



/***********************************************************************/

    if (useConeSourceTowardSingleModule) {

        // Scintillator parameters
        G4double scinCenterX = 10.0 * cm;
        G4double scinCenterY = 0.0 * cm;
        G4double scinCenterZ = 0.0 * cm;
        G4double scinHalfX = 2.5 * cm / 2.0;
        G4double scinHalfY = 0.6 * cm / 2.0;
        G4double scinHalfZ = 50.0 * cm / 2.0;

        // Source at the origin
        G4ThreeVector sourcePos(0, 0, 0);

        // Randomly pick a point on the +y face of the single module
        G4double randX = scinCenterX + (2.0 * scinHalfX) * (G4UniformRand() - 0.5);
        G4double randY = scinCenterY + scinHalfY; // +y face
        G4double randZ = scinCenterZ + (2.0 * scinHalfZ) * (G4UniformRand() - 0.5);
        G4ThreeVector targetPoint(randX, randY, randZ);

        // Direction from source to target point
        G4ThreeVector direction = (targetPoint - sourcePos).unit();

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
            return; // Skip neutral particles for charged detection
        }

        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(energy);
        fParticleGun->SetParticlePosition(sourcePos);
        fParticleGun->SetParticleMomentumDirection(direction);
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }




/***********************************************************************/

    if (useConeSourceTowardScintillator) {
        // Scintillator parameters
        G4double scinCenterX = 10.0 * cm;
        G4double scinCenterY = 0.0 * cm;
        G4double scinCenterZ = 0.0 * cm;
        G4double scinHalfY = 0.3 * cm;      // half-width in y (0.6 cm total)
        G4double scinHalfZ = 25.0 * cm;     // half-length in z (50 cm total)
        G4double offset = 0.01 * cm;        // small offset to be just outside

        // Source at the origin
        G4ThreeVector sourcePos(0, 0, 0);

        // Randomly pick a point on the +y face of the scintillator
        G4double randZ = scinCenterZ + (2.0 * scinHalfZ) * (G4UniformRand() - 0.5);
        G4double randX = scinCenterX; // always center in x for this face
        G4double randY = scinCenterY + scinHalfY; // face at +y

        G4ThreeVector targetPoint(randX, randY, randZ);

        // Direction from source to target point
        G4ThreeVector direction = (targetPoint - sourcePos).unit();

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
            return; // Skip neutral particles for charged detection
        }
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(energy);
        fParticleGun->SetParticlePosition(sourcePos);
        fParticleGun->SetParticleMomentumDirection(direction);
    
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }



/***********************************************************************/

    // Diagnostic Moire source


    //     if (useMoireSource) {

    //     // =================================================================
    //     // --- SYMMETRY DIAGNOSTIC TEST ---
    //     // Instructions:
    //     // 1. Uncomment ONE of the two test cases below. Leave the other commented.
    //     // 2. In your terminal, go to your build directory.
    //     // 3. Force a clean re-compile by running:  make clean && make
    //     // 4. Run your simulation for a fixed number of events (e.g., 100,000).
    //     // 5. Run your reconstruction code and record the "Total extrapolated vertices found".
    //     // 6. Come back here, comment out the first test case, and uncomment the second one.
    //     // 7. Repeat steps 3, 4, and 5.
    //     // 8. Compare the number of vertices from both runs. If the code and geometry
    //     //    are symmetric, these two numbers should be nearly identical.

    //     // --- Test Case 1: Positive Z Source ONLY ---
    //     // const G4ThreeVector testSourceCenter(-8.0 * cm, 3.5 * cm, 50.0 * cm);
    //     // const G4ThreeVector testAimPoint(25.8 * cm, 0.0, 30.0 * cm); // Aims at right arm

    //     // --- Test Case 2: Negative Z Source ONLY ---
    //     const G4ThreeVector testSourceCenter(-8.0 * cm, 3.5 * cm, -50.0 * cm);
    //     const G4ThreeVector testAimPoint(25.8 * cm, 0.0, -30.0 * cm); // Aims at left arm
    //     // =================================================================


    //     // --- Common settings for the test ---
    //     std::vector<G4ThreeVector> sourcePositions = { testSourceCenter }; // Use only the selected test source
    //     G4double boxHalfX = 7.0 * cm / 2.0;
    //     G4double boxHalfY = 7.0 * cm / 2.0;
    //     G4double boxHalfZ = 250.0 * micrometer / 2.0;
    //     G4double coneHalfAngle = 40.0 * deg;


    //     // This loop will only run once per event, for the single source defined above.
    //     for (const auto& sourceCenter : sourcePositions) {
    //         // Generate a random position *within* the source box
    //         G4double rx = (G4UniformRand() - 0.5) * boxHalfX;
    //         G4double ry = (G4UniformRand() - 0.5) * boxHalfY;
    //         G4double rz = (G4UniformRand() - 0.5) * boxHalfZ;
    //         G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

    //         // Select particle type and energy
    //         G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    //         G4ParticleDefinition* piPlus  = particleTable->FindParticle("pi+");
    //         G4ParticleDefinition* piMinus = particleTable->FindParticle("pi-");
    //         G4ParticleDefinition* kPlus   = particleTable->FindParticle("kaon+");
    //         G4ParticleDefinition* kMinus  = particleTable->FindParticle("kaon-");
            
    //         G4ParticleDefinition* particle;
    //         G4double energy;
    //         G4double rand = G4UniformRand();
            
    //         if (rand < 0.60) {
    //             particle = (G4UniformRand() < 0.5) ? piPlus : piMinus;
    //             energy = 230 * MeV;
    //         }
    //         else if (rand < 0.80) {
    //             particle = particleTable->FindParticle("pi0");
    //             energy = 230 * MeV;
    //         }
    //         else if (rand < 0.96) {
    //             particle = (G4UniformRand() < 0.5) ? kPlus : kMinus;
    //             energy = 150*MeV + 250*MeV*G4UniformRand();
    //         }
    //         else {
    //             continue; // Skip eta mesons
    //         }

    //         // Define the cone axis from the particle's origin to the pre-defined aim point
    //         G4ThreeVector coneAxis = (testAimPoint - sourcePos).unit();

    //         // Sample a random direction within the defined cone
    //         G4double cosThetaMax = std::cos(coneHalfAngle);
    //         G4double cosTheta = cosThetaMax + (1.0 - cosThetaMax) * G4UniformRand();
    //         G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    //         G4double phi = 2 * M_PI * G4UniformRand();
            
    //         G4ThreeVector randomDirection(sinTheta * std::cos(phi),
    //                                       sinTheta * std::sin(phi),
    //                                       cosTheta);

    //         // Rotate this randomly generated vector so that it is oriented along the coneAxis
    //         G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

    //         // Configure particle gun
    //         fParticleGun->SetParticleDefinition(particle);
    //         fParticleGun->SetParticlePosition(sourcePos);
    //         fParticleGun->SetParticleMomentumDirection(finalDirection);
    //         fParticleGun->SetParticleEnergy(energy);
            
    //         fParticleGun->GeneratePrimaryVertex(anEvent);
    //     }
    // }



/***********************************************************************/



    if (useMoireSource) {
        G4ThreeVector stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);

        
        // 2. Define source positions in the STL's *local* frame
        std::vector<G4ThreeVector> localSourcePositions = {
            G4ThreeVector(0, 0, 0),                  // Center of STL
            G4ThreeVector(0, 0, 50 * cm),          // Offset along STL local Z
            G4ThreeVector(0, 0, -50 * cm)          // Offset along STL local -Z
        };

        // 3. Transform local positions to world coordinates
        std::vector<G4ThreeVector> sourcePositions;
        for (const auto& localPos : localSourcePositions) {
            G4ThreeVector worldPos = localPos + stlPosition;
            sourcePositions.push_back(worldPos);
        }


        // Define the dimensions of the source box (using your physically correct thick source)
        G4double boxHalfX = 7.0 * cm / 2.0;
        G4double boxHalfY = 7.0 * cm / 2.0;
        G4double boxHalfZ = 250.0 * micrometer / 2.0;

        // 1. Define the average centers of the LEFT and RIGHT detector arms.
        G4ThreeVector rightArmCenter(25.8 * cm, 0.0,  30.0 * cm);
        G4ThreeVector leftArmCenter (25.8 * cm, 0.0, -30.0 * cm);

        // 2. Define the opening angle of the emission cone. This is a key parameter to tune.
        //    A larger angle ensures all detectors are illuminated, especially from the off-axis sources.
        G4double coneHalfAngle = 70 * deg;


        // --- MODIFICATION: Get the Analysis Manager ---
        // Get the singleton instance of the G4AnalysisManager
        auto analysisManager = G4AnalysisManager::Instance();


    // Loop over each of the three source locations
    for (const auto& sourceCenter : sourcePositions) {
        // Generate a random position *within* one of the source boxes
        G4double rx = (G4UniformRand() - 0.5) * (2.0 * boxHalfX);  
        G4double ry = (G4UniformRand() - 0.5) * (2.0 * boxHalfY);
        G4double rz = (G4UniformRand() - 0.5) * (2.0 * boxHalfZ);
        G4ThreeVector sourcePos = sourceCenter + G4ThreeVector(rx, ry, rz);

        // Select particle type and energy (this logic is unchanged)
        G4ParticleDefinition* particle;
        G4double energy;
        G4double rand = G4UniformRand();
        
        if (rand < 0.60) {
            particle = (G4UniformRand() < 0.5) ? piPlus : piMinus;
            energy = 230 * MeV;
        }
        else if (rand < 0.80) {
            particle = particleTable->FindParticle("pi0");
            energy = 230 * MeV;
        }
        else if (rand < 0.96) {
            particle = (G4UniformRand() < 0.5) ? kPlus : kMinus;
            energy = 150*MeV + 250*MeV*G4UniformRand();
        }
        else {
            continue; // Skip eta mesons
        }



        analysisManager->FillH3(0,
                                sourcePos.x() / cm,
                                sourcePos.y() / cm,
                                sourcePos.z() / cm);

        // --- THIS IS THE CORRECTED AIMING LOGIC ---
        G4ThreeVector aimPoint;
        if (sourceCenter.z() > 4 * cm) { // This is the Z=+50 cm source
            aimPoint = rightArmCenter;
        } else if (sourceCenter.z() < -4 * cm) { // This is the Z=-50 cm source
            aimPoint = leftArmCenter; // It now correctly aims at the left arm
        } else { // This is the Z=0 cm source
            // Randomly aim at either arm for symmetric illumination
            if (G4UniformRand() < 0.5) {
                aimPoint = rightArmCenter;
            } else {
                aimPoint = leftArmCenter;
            }
        }

        // --- REPLACED ISOTROPIC EMISSION WITH CONE EMISSION ---
        
        // 3. Define the cone axis: a vector from the particle's actual origin to the detector center.
        //    This must be calculated for each particle because `sourcePos` is randomized.
        G4ThreeVector coneAxis = (aimPoint - sourcePos).unit();

        // 4. Sample a random direction within the defined cone using the provided logic.
        G4double cosThetaMax = std::cos(coneHalfAngle);
        G4double cosTheta = cosThetaMax + (1.0 - cosThetaMax) * G4UniformRand();
        G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
        G4double phi = 2 * M_PI * G4UniformRand();
        
        G4ThreeVector randomDirection(sinTheta * std::cos(phi),
                                      sinTheta * std::sin(phi),
                                      cosTheta);

        // 5. Rotate this randomly generated vector so that it is oriented along the coneAxis.
        //    `rotateUz` finds the rotation that maps the Z-axis to the coneAxis and applies it.
        G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

        // Configure particle gun with the new directed momentum
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticlePosition(sourcePos);
        fParticleGun->SetParticleMomentumDirection(finalDirection); // Use the new direction
        fParticleGun->SetParticleEnergy(energy);
        
        fParticleGun->GeneratePrimaryVertex(anEvent);
        }
    }


/***********************************************************************/

    if (useTestSingleSource) {
        cout << "PrimaryGenerator: Using test single source emission" << endl;
        // Test source at a fixed position
        G4ThreeVector sourcePosition(0, 0, 0); // Center of the world volume
        fParticleGun->SetParticlePosition(sourcePosition);

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
            return; // Skip neutral particles for charged detection
        }

        // Set particle type and energy
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(energy);

        fParticleGun->SetParticleMomentumDirection((G4ThreeVector(1, 0, 0)).unit()); // Forward direction
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }




/***********************************************************************/



    if (useConeSourceTowardFourModules) {
        cout << "PrimaryGenerator: Using cone emission toward four modules" << endl;
        // Module positions (same as geometry)
        std::vector<G4ThreeVector> moduleCenters = {
            G4ThreeVector(15.8*cm, 0,  30*cm),
            G4ThreeVector(25.8*cm, 0,  30*cm),
            G4ThreeVector(15.8*cm, 0, -30*cm),
            G4ThreeVector(25.8*cm, 0, -30*cm)
        };

        // Compute average module center for cone axis
        G4ThreeVector avgModuleCenter(0, 0, 0);
        for (const auto& module : moduleCenters) {
            avgModuleCenter += module;
        }
        avgModuleCenter /= moduleCenters.size();

        // Source at origin
        G4ThreeVector sourcePos(0, 0, 0);

        // Cone half angle (adjust as needed, e.g. 30 deg)
        G4double coneHalfAngle = 70.0 * deg;

        // Select particle type based on annihilation ratios
        G4ParticleDefinition* particle;
        G4double energy;
        G4double rand = G4UniformRand();

        if (rand < 0.60) {
            particle = (G4UniformRand() < 0.5) ? piPlus : piMinus;
            energy = 230 * MeV;
        } else if (rand < 0.80) {
            particle = particleTable->FindParticle("pi0");
            energy = 230 * MeV;
        } else if (rand < 0.96) {
            particle = (G4UniformRand() < 0.5) ? kPlus : kMinus;
            energy = 150*MeV + 250*MeV*G4UniformRand();
        } else {
            return; // skip neutral
        }

        // Cone axis: from source to avg module center
        G4ThreeVector coneAxis = (avgModuleCenter - sourcePos).unit();

        // Sample direction within cone
        G4double cosTheta = std::cos(coneHalfAngle);
        G4double randomCosTheta = cosTheta + (1 - cosTheta) * G4UniformRand();
        G4double sinTheta = std::sqrt(1 - randomCosTheta * randomCosTheta);
        G4double phi = 2 * M_PI * G4UniformRand();
        G4ThreeVector randomDirection(
            sinTheta * std::cos(phi),
            sinTheta * std::sin(phi),
            randomCosTheta);

        G4ThreeVector finalDirection = randomDirection.rotateUz(coneAxis);

        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(energy);
        fParticleGun->SetParticlePosition(sourcePos);
        fParticleGun->SetParticleMomentumDirection(finalDirection.unit());
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }


}
