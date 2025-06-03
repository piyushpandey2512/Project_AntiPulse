#include "SensitiveDet.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "HitBuffer.hh"


// Constructor
MySensitiveDetector::MySensitiveDetector(G4String DetName) : G4VSensitiveDetector(DetName)
{}

// Destructor
MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
    // Get the track of the particle
    G4Track *track = aStep->GetTrack();

    // Get pre-step and post-step points
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

    // Get the position of the interaction
    G4ThreeVector positionPion = preStepPoint->GetPosition();

    // Extract x, y, z coordinates
    G4double xPos = positionPion.x() / cm; // Convert to cm
    G4double yPos = positionPion.y() / cm;
    G4double zPos = positionPion.z() / cm;    



    // Get the volume information
    const G4VTouchable *touchable = aStep->GetPostStepPoint()->GetTouchable();
    G4VPhysicalVolume *physVol = touchable->GetVolume();

    // Get the copy number of the scintillator
    G4int copyNumber = touchable->GetVolume()->GetCopyNo();

    // Get additional information about the particle and step
    G4int parentID = track->GetParentID();
    G4int trackID = track->GetTrackID();
    G4int stepID = track->GetCurrentStepNumber();
    G4String processName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
    G4double localTime = postStepPoint->GetLocalTime();
    G4double globalTime = postStepPoint->GetGlobalTime();
    G4double energyDeposit = aStep->GetTotalEnergyDeposit() / MeV;
    G4String particleName = track->GetDynamicParticle()->GetDefinition()->GetParticleName();

    // Log the information for debugging
    // G4cout << "Event ID: " << G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID()
    //        << ", Scintillator Copy Number: " << copyNumber
    //        << ", Particle: " << particleName
    //        << ", Energy Deposit: " << energyDeposit << " MeV"
    //        << ", Process: " << processName
    //        << G4endl;

    // Use G4AnalysisManager to store the data in an ntuple
    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    manager->FillNtupleIColumn(0, eventID);              // fEvent
    manager->FillNtupleDColumn(1, xPos);                 // fXcor
    manager->FillNtupleDColumn(2, yPos);                 // fYcor
    manager->FillNtupleDColumn(3, zPos);                 // fZcor
    manager->FillNtupleIColumn(4, parentID);             // fParentID
    manager->FillNtupleIColumn(5, trackID);              // fTrackID
    manager->FillNtupleIColumn(6, stepID);               // fStepID
    manager->FillNtupleSColumn(7, processName);          // fProcess
    manager->FillNtupleDColumn(8, localTime);            // fTimeL
    manager->FillNtupleDColumn(9, globalTime);           // fTimeG
    manager->FillNtupleDColumn(10, energyDeposit);       // fEnergyDeposition
    manager->FillNtupleIColumn(11, copyNumber);          // fCopyID
    manager->FillNtupleSColumn(12, particleName);        // fParticles
    manager->AddNtupleRow(0);

	// Fill histograms for scintillator hits and energy deposition
    if (particleName == "pi+" || particleName == "pi-" || particleName == "pi0" || particleName == "anti_proton") {
        // Histogram for scintillator copy numbers where pions are detected
        manager->FillH1(0, copyNumber); // Histogram ID 0 for scintillator copy numbers

        // Histogram for energy deposition of pions
        if (energyDeposit > 0) {
            manager->FillH1(1, energyDeposit); // Histogram ID 1 for energy deposition
        }
    }

    
    // Only accept valid tracks
    if (trackID <= 0) return false;

    // Record hits based on known geometry locations
    if (xPos < 16 && zPos > 44 && frontHits45.count(trackID) == 0) {
        frontHits45[trackID] = {{xPos, yPos, zPos}, eventID, trackID};
    }
    else if (xPos > 22 && zPos > 44 && backHits45.count(trackID) == 0) {
        backHits45[trackID] = {{xPos, yPos, zPos}, eventID, trackID};
    }
    else if (xPos < 16 && zPos < -44 && frontHitsMinus45.count(trackID) == 0) {
        frontHitsMinus45[trackID] = {{xPos, yPos, zPos}, eventID, trackID};
    }
    else if (xPos > 22 && zPos < -44 && backHitsMinus45.count(trackID) == 0) {
        backHitsMinus45[trackID] = {{xPos, yPos, zPos}, eventID, trackID};
    }

    return true; // Indicate that the hit was processed successfully
}