#include "SensitiveDet.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

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
    manager->FillNtupleIColumn(0, eventID); // Event ID
    manager->FillNtupleIColumn(1, copyNumber); // Scintillator copy number
    manager->FillNtupleSColumn(2, particleName); // Particle name
    manager->FillNtupleDColumn(3, energyDeposit); // Energy deposit
    manager->FillNtupleSColumn(4, processName); // Process name
    manager->FillNtupleDColumn(5, localTime); // Local time
    manager->FillNtupleDColumn(6, globalTime); // Global time
    manager->AddNtupleRow(0); // Add the row to ntuple 0

	// Fill histograms for scintillator hits and energy deposition
    if (particleName == "pi+" || particleName == "pi-" || particleName == "pi0") {
        // Histogram for scintillator copy numbers where pions are detected
        manager->FillH1(0, copyNumber); // Histogram ID 0 for scintillator copy numbers

        // Histogram for energy deposition of pions
        if (energyDeposit > 0) {
            manager->FillH1(1, energyDeposit); // Histogram ID 1 for energy deposition
        }
    }

    return true; // Indicate that the hit was processed successfully
}