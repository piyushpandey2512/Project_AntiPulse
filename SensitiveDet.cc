#include "SensitiveDet.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "HitBuffer.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"     // <-- Include for Initialize method
#include "GratingHit.hh"      // <-- Include for the new hit class

// Constructor
MySensitiveDetector::MySensitiveDetector(G4String DetName)
 : G4VSensitiveDetector(DetName)
{
    // =======================================================================
    // --- Define the name of the new hits collection this SD will create ---
    collectionName.insert("GratingCounterHitsCollection");
    // =======================================================================
}

// Destructor
MySensitiveDetector::~MySensitiveDetector() {}

// =======================================================================
// --- ADD THIS NEW METHOD ---
// This is called automatically at the beginning of each event.
void MySensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
    // Create the hits collection object for the current event.
    fGratingHitsCollection = new GratingHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Get a unique ID for this collection from the SD manager.
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

    // Add the collection to the Hits Collection of this Event (HCE).
    hce->AddHitsCollection(hcID, fGratingHitsCollection);
}
// =======================================================================

// --- ADD THIS EMPTY METHOD ---
// Required by the base class, but we will do our analysis in EventAction.
void MySensitiveDetector::EndOfEvent(G4HCofThisEvent* /*hce*/) {}


// The main processing method, called for every step in a sensitive volume
G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{

    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    // --- Get common information ---
    G4Track *track = aStep->GetTrack();
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    const G4VTouchable* touchable = preStepPoint->GetTouchable();
    G4String volumeName = touchable->GetVolume()->GetLogicalVolume()->GetName();

    // =======================================================================
    // --- LOGIC 1: Collect hits for Primary Antiprotons on Gratings/Counter ---
    // =======================================================================
    // We only care about the first moment a primary particle (TrackID==1) ENTERS a volume.
    if (preStepPoint->GetStepStatus() == fGeomBoundary && track->GetTrackID() == 1) {
        G4int detectorID = 0;

        if (volumeName == "GratingOpeningLog") {
            // Passed through a grating opening. Get the mother's copy number (1 or 2).
            detectorID = touchable->GetCopyNumber(2);
        }
        else if (volumeName == "SolidCounterLog") {
            // Hit the solid counter. Get its copy number (3).
            detectorID = touchable->GetVolume()->GetCopyNo();
        }
        else if (volumeName == "WallLog") {
            // Hit a silicon wall. Record as a negative number.
            detectorID = -touchable->GetCopyNumber(2);
        }
        
        // If this was a valid grating or counter hit...
        if (detectorID != 0) {
            // ...create a new hit object.
            GratingHit* newHit = new GratingHit();
            newHit->SetTrackID(track->GetTrackID());
            newHit->SetDetectorNb(detectorID);
            
            // Add the hit to our collection for this event.
            fGratingHitsCollection->insert(newHit);
        }
    }


    // =======================================================================
    // --- NEW LOGIC: Record detailed hits on the SOLID COUNTER ---
    // =======================================================================
    if (volumeName == "SolidCounterLog") {
        // 1. Get the total energy deposited in this step.
        G4double energyDeposit = aStep->GetTotalEnergyDeposit();

        // 2. We only want to record hits that actually deposit energy.
        if (energyDeposit > 0.0) {
            G4AnalysisManager* manager = G4AnalysisManager::Instance();
            
            // 3. Get the position of the hit (the pre-step point).
            G4ThreeVector position = preStepPoint->GetPosition();
            G4double x = position.x() / cm;
            G4double y = position.y() / cm;
            G4double z = position.z() / cm;

            // 4. (Optional) Print the X and Y coordinates to the console for immediate feedback.
            // G4cout << "Counter Hit Recorded at (X, Y) = (" << x << " cm, " << y << " cm)" << G4endl;

            // 5. Fill the new n-tuple (ID 2) with the hit information.
            manager->FillNtupleDColumn(3, 0, x);
            manager->FillNtupleDColumn(3, 1, y);
            manager->FillNtupleDColumn(3, 2, z);
            manager->FillNtupleDColumn(3, 3, energyDeposit / MeV);
            manager->AddNtupleRow(3);
        }
    }


    // =======================================================================
    // --- ORIGINAL LOGIC: Detailed data logging for Scintillators ---
    // This code is UNCHANGED and will only be reached if the step was not
    // a primary particle entering a grating opening or the solid counter.
    // =======================================================================
    if (volumeName == "ScintillatorLV" || volumeName == "ScintillatorSingleLV") {

        G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
        G4ThreeVector positionPion = preStepPoint->GetPosition();
        G4int copyNumber = touchable->GetVolume()->GetCopyNo();
        G4int parentID = track->GetParentID();
        G4int trackID = track->GetTrackID();
        G4int stepID = track->GetCurrentStepNumber();
        G4String processName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
        G4double localTime = postStepPoint->GetLocalTime();
        G4double globalTime = postStepPoint->GetGlobalTime();
        G4double energyDeposit = aStep->GetTotalEnergyDeposit() / MeV;
        G4String particleName = track->GetDynamicParticle()->GetDefinition()->GetParticleName();
        
        G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        manager->FillNtupleIColumn(0, 0, eventID);
        manager->FillNtupleDColumn(0, 1, positionPion.x() / cm);
        manager->FillNtupleDColumn(0, 2, positionPion.y() / cm);
        manager->FillNtupleDColumn(0, 3, positionPion.z() / cm);
        manager->FillNtupleIColumn(0, 4, parentID);
        manager->FillNtupleIColumn(0, 5, trackID);
        manager->FillNtupleIColumn(0, 6, stepID);
        manager->FillNtupleSColumn(0, 7, processName);
        manager->FillNtupleDColumn(0, 8, localTime);
        manager->FillNtupleDColumn(0, 9, globalTime);
        manager->FillNtupleDColumn(0, 10, energyDeposit);
        manager->FillNtupleIColumn(0, 11, copyNumber);
        manager->FillNtupleSColumn(0, 12, particleName);
        manager->AddNtupleRow(0);

        if (particleName == "pi+" || particleName == "pi-" || particleName == "pi0" || particleName == "anti_proton") {
            manager->FillH1(0, copyNumber);
            if (energyDeposit > 0) {
                manager->FillH1(1, energyDeposit);
            }
        }
        
        if (trackID > 0) {
            G4double xPos = positionPion.x();
            G4double zPos = positionPion.z();
            if (xPos < 16*cm && zPos > 44*cm && frontHits45.count(trackID) == 0) {
                frontHits45[trackID] = {{xPos, positionPion.y(), zPos}, eventID, trackID};
            }
            else if (xPos > 22*cm && zPos > 44*cm && backHits45.count(trackID) == 0) {
                backHits45[trackID] = {{xPos, positionPion.y(), zPos}, eventID, trackID};
            }
            else if (xPos < 16*cm && zPos < -44*cm && frontHitsMinus45.count(trackID) == 0) {
                frontHitsMinus45[trackID] = {{xPos, positionPion.y(), zPos}, eventID, trackID};
            }
            else if (xPos > 22*cm && zPos < -44*cm && backHitsMinus45.count(trackID) == 0) {
                backHitsMinus45[trackID] = {{xPos, positionPion.y(), zPos}, eventID, trackID};
            }
        }
    }

    return true; // Indicate that the step was processed successfully
}