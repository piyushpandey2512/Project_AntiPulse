#include "SensitiveDet.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
<<<<<<< HEAD

//Constructor
//Don't miss G4String det name,
MySensitiveDetector::MySensitiveDetector(G4String DetName) : G4VSensitiveDetector(DetName)
{}

//Destructor
=======
#include "G4RunManager.hh"

// Constructor
MySensitiveDetector::MySensitiveDetector(G4String DetName) : G4VSensitiveDetector(DetName)
{}

// Destructor
>>>>>>> 3ac4172 (Initial fresh commit)
MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
<<<<<<< HEAD
	G4Track *track = aStep->GetTrack();
	
	
	// Lets find out the Track info before step and after step
	G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
	G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
	
	// Interaction positions of photon/particle
	G4ThreeVector positionPhoton = preStepPoint->GetPosition();
	
//	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
	const G4VTouchable *touchable = aStep->GetPostStepPoint()->GetTouchable();
	
	//GetPosition gives the exact coordiantes of interaction. If there are multiple
	//detector, one can have the each detector translation(or coordinates) through
	// its physical volume, such as :
	G4VPhysicalVolume *physVol = touchable->GetVolume();
	G4ThreeVector DetectorCor = physVol->GetTranslation();
	//New
	G4int ParID       = aStep->GetTrack()->GetParentID();
	G4int TraID       = aStep->GetTrack()->GetTrackID();
	G4int StepID      = track->GetCurrentStepNumber();
	G4String Process  = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	G4double timeL    = aStep->GetPostStepPoint()->GetLocalTime();
	G4double timeG    = aStep->GetPostStepPoint()->GetGlobalTime();
	G4double eDepStep = aStep->GetTotalEnergyDeposit()/MeV;
	G4int CopyNo      = touchable->GetVolume()->GetCopyNo();
	
	G4String name     = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName();
	
//	G4cout<<" Photon position = "<<positionPhoton<<"  and Copy number "<<G4endl;
	
	// Make instance of AnalysisManager --
	G4int EventId = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	//include G4RunManager in SensitiveDet.hh
	
	G4AnalysisManager *manager = G4AnalysisManager::Instance();
	manager->FillNtupleIColumn(0,EventId);
	manager->FillNtupleDColumn(1,DetectorCor[0]);
	manager->FillNtupleDColumn(2,DetectorCor[1]);
	manager->FillNtupleDColumn(3,DetectorCor[2]);
	//new
	manager->FillNtupleIColumn(4,ParID);
	manager->FillNtupleIColumn(5,TraID);
	manager->FillNtupleIColumn(6,StepID);
	manager->FillNtupleSColumn(7,Process);
	manager->FillNtupleDColumn(8,timeL);
	manager->FillNtupleDColumn(9,timeG);
	manager->FillNtupleDColumn(10,eDepStep);
	manager->FillNtupleIColumn(11,CopyNo);
	manager->FillNtupleSColumn(12,name);
	//-------
	
	manager->AddNtupleRow(0); // 0 refers that the information will be stored to tuple 0
        return 0;		
}


=======
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
>>>>>>> 3ac4172 (Initial fresh commit)
