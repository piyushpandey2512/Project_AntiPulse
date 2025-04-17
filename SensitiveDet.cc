#include "SensitiveDet.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"

//Constructor
//Don't miss G4String det name,
MySensitiveDetector::MySensitiveDetector(G4String DetName) : G4VSensitiveDetector(DetName)
{}

//Destructor
MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
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


