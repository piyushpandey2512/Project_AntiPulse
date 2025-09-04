#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "HitBuffer.hh"
#include <algorithm>
#include <cmath>
#include "GratingHit.hh"
#include "G4SDManager.hh"

MyEventAction::MyEventAction(MyRunAction* runAction)
: fRunAction(runAction)
{
    fEnergyDeposition = 0;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
    fEnergyDeposition = 0;
    hasFront = false;
    hasBack = false;
}

void MyEventAction::EndOfEventAction(const G4Event* event)
{
    auto* manager = G4AnalysisManager::Instance();
    manager->FillNtupleDColumn(1, 0, fEnergyDeposition); // Column 0 in ntuple 1
    manager->AddNtupleRow(1);


    // ---- Print event summary ----
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ((printModulo > 0) && (eventID % printModulo == 0)) {
        G4cout << "--> Event " << eventID << " completed. Total energy: "
               << G4BestUnit(fEnergyDeposition, "Energy") << G4endl;
    }
// =======================================================================
    // --- TASK 5: HIERARCHICAL GRATING PARTICLE COUNTING (Your New Logic) ---
    // This is the new code to analyze the grating hits.
    
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    G4int hcID = sdManager->GetCollectionID("GratingCounterHitsCollection");
    if (hcID < 0) return; // If the collection doesn't exist, do nothing.

    auto hitsCollection = static_cast<GratingHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
    if (!hitsCollection || hitsCollection->entries() == 0) return; // If no hits, do nothing.

    // --- Analysis Logic ---
    bool hitG1_passed = false;
    bool hitG2_passed = false;
    bool hitC3 = false;

    // --- ADD THESE NEW FLAGS ---
    bool hitG1_absorbed = false;
    bool hitG2_absorbed = false;

    // Loop through all the hits from the primary particle in this event
    for (size_t i = 0; i < hitsCollection->entries(); ++i) {
        GratingHit* hit = (*hitsCollection)[i];
        
        // We only care about pass-through events (positive detector ID)
        if (hit->GetDetectorNb() == 1) hitG1_passed = true;
        if (hit->GetDetectorNb() == 2) hitG2_passed = true;
        if (hit->GetDetectorNb() == 3) hitC3 = true;

        // --- NEW ABSORPTION CHECKS ---
        if (hit->GetDetectorNb() == -1) hitG1_absorbed = true;
        if (hit->GetDetectorNb() == -2) hitG2_absorbed = true;
    }

    // Check the conditions hierarchically and update the RunAction counters
    if (hitG1_passed) {
        fRunAction->AddPassedG1();
        if (hitG2_passed) {
            fRunAction->AddPassedG2();
            if (hitC3) {
                fRunAction->AddHitCounter();
            }
        }
        // If it passed G1, it could not have been absorbed by G2 without passing G1 first.
        // So we check for G2 absorption here.
        else if (hitG2_absorbed) {
             fRunAction->AddAbsorbedG2();
        }
    }
    // If it did NOT pass G1, check if it was absorbed by G1.
    else if (hitG1_absorbed) {
        fRunAction->AddAbsorbedG1();
    }
    // --- Histogram Filling ---
    G4int gratingHistId = manager->GetH1Id("GratingInteractions");

    // Fill the histogram based on what happened in the event
    // Bin 0: G1 Absorbed
    // Bin 1: G1 Passed
    // Bin 2: G2 Absorbed
    // Bin 3: G2 Passed
    // Bin 4: C3 Hit
    if (hitG1_absorbed) manager->FillH1(gratingHistId, 0);
    if (hitG1_passed)   manager->FillH1(gratingHistId, 1);
    
    if (hitG2_absorbed) manager->FillH1(gratingHistId, 2);
    if (hitG2_passed)   manager->FillH1(gratingHistId, 3);

    if (hitC3)          manager->FillH1(gratingHistId, 4);
    // Bin 5 is unused for now.
    // ----------------------------------------------------------------------
}

void MyEventAction::SetFrontPoint(const G4ThreeVector& front) {
    fFrontPosition = front;
    hasFront = true;
}

void MyEventAction::SetBackPoint(const G4ThreeVector& back) {
    fBackPosition = back;
    hasBack = true;
}


void MyEventAction::StoreIntraModuleMomentum(const G4Track* track, const G4ThreeVector& momentum) {
    fIntraModuleMomentumMap[track->GetTrackID()] = momentum;
}

G4ThreeVector MyEventAction::GetIntraModuleMomentum(const G4Track* track) const {
    auto it = fIntraModuleMomentumMap.find(track->GetTrackID());
    if (it != fIntraModuleMomentumMap.end()) return it->second;
    return G4ThreeVector(0,0,0);
}

void MyEventAction::StoreInterModuleMomentum(const G4Track* track, const G4ThreeVector& momentum) {
    fInterModuleMomentumMap[track->GetTrackID()] = momentum;
}

G4ThreeVector MyEventAction::GetInterModuleMomentum(const G4Track* track) const {
    auto it = fInterModuleMomentumMap.find(track->GetTrackID());
    if (it != fInterModuleMomentumMap.end()) return it->second;
    return G4ThreeVector(0,0,0);
}

void MyEventAction::StoreSingleScintMomentum(G4int trackID, const G4ThreeVector& momentum)
{
    fSingleScintMomentumMap[trackID] = momentum;
}

G4ThreeVector MyEventAction::GetSingleScintMomentum(G4int trackID)
{
    auto it = fSingleScintMomentumMap.find(trackID);
    if (it != fSingleScintMomentumMap.end()) {
        return it->second;
    }
    return G4ThreeVector(0,0,0); // Return zero vector if not found
}

void MyEventAction::StoreB2BFrontMomentum(G4int trackID, const G4ThreeVector& momentum) {
    fB2BFrontMomentumMap[trackID] = momentum;
}

G4ThreeVector MyEventAction::GetB2BFrontMomentum(G4int trackID) const {
    auto it = fB2BFrontMomentumMap.find(trackID);
    if (it != fB2BFrontMomentumMap.end()) return it->second;
    return G4ThreeVector(0,0,0);
}