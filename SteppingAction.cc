#include "SteppingAction.hh"
//For external file writing -

#include "EventAction.hh"
#include "G4RunManager.hh"
#include <G4Track.hh>
#include "G4UnitsTable.hh"
#include <iostream>
#include <iomanip>
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AnalysisManager.hh"
using namespace std;


MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
	fEventAction = eventAction; // We get access to the object, we created
}

MySteppingAction::~MySteppingAction()
{}

 
//Import function
void MySteppingAction::UserSteppingAction(const G4Step *step)
{
	//Open a file in append mode-
	std::ofstream file("Information.txt",std::ios::app);
	
	// G4String particle=step->GetTrack()->GetParticleDefinition()->GetParticleName();
	// if (particle=="pi+" && step->IsFirstStepInVolume()) { G4cout<<"Hello pion+"<<G4endl;}
	// if (particle=="pi-" && step->IsFirstStepInVolume()) { G4cout<<"Hello pion-"<<G4endl;}
	// if (particle=="pi0" && step->IsFirstStepInVolume()) { G4cout<<"Hello pion0"<<G4endl;}

	// Retrieve the copy number of the current volume
	G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4VPhysicalVolume* volume = preStepPoint->GetPhysicalVolume();
    if (!volume) return;
    G4int copyNumber = volume->GetCopyNo();
	
	// Retrieve energy deposited in the step
	G4double deltaEDep = step->GetTotalEnergyDeposit();
//	G4cout<<"Energy depposition step wise ="<<deltaEDep<<G4endl;
	fEventAction->AddEnergyDep(deltaEDep);
	
	// Extract few information here to be written -
	const G4Event* evnt = G4RunManager::GetRunManager()->GetCurrentEvent();
	G4int Event = evnt->GetEventID();
	
	G4int PID       = step->GetTrack()->GetParentID();
	G4int TID       = step->GetTrack()->GetTrackID();
	G4int SID     	= step->GetTrack()->GetCurrentStepNumber();
	G4String ProcessName  = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    // Check if the step is in the scintillator volume
if (volume->GetLogicalVolume()->GetName() == "ScintillatorLV") {
    // Retrieve energy deposited in the step
    G4double energyDep = step->GetTotalEnergyDeposit();

     // Retrieve the copy number of the scintillator
     G4int copyNumber = volume->GetCopyNo(); // This should correspond to m*100 + j

    // Fill the histogram for energy deposition in the scintillator
    if (energyDep > 0) { // Only record non-zero energy depositions
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->FillH1(11, energyDep / keV); // Histogram ID 11 corresponds to "EnergyDep_J-PET"
    }

    // Fill the histogram for the specific scintillator
    if (energyDep > 0) { // Only record non-zero energy depositions
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->FillH1(0, copyNumber, energyDep / keV); // Histogram ID 0 corresponds to "EnergyDep_Scintillators"
    }

    G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
    if (particleName == "pi+" || particleName == "pi-" || particleName == "pi0") {
        // Increment the histogram for the corresponding scintillator
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->FillH1(12, copyNumber % 100); // Histogram ID 12 corresponds to "PionHits_Scintillators"
    }
}
	
    file<<setw(9)<<Event<<"\t"<<
	setw(3)<<PID<<"\t"<<
	setw(3)<<TID<<"\t"<<
	setw(5)<<SID<<"\t"<<
	setw(15)<<ProcessName<<"\t"<<
	setw(8)<<deltaEDep/keV<<"\t"<<
	setw(15) << volume << "\t"<< 
    setw(3) << copyNumber <<G4endl;
	
	file.close();

	// Histogramming and specific volume actions
    auto analysisManager = G4AnalysisManager::Instance();
    switch (copyNumber) {
        case 1: // 5T Volume
            Handle5TVolume(step, analysisManager);
            break;
        case 2: // 1T Volume
            Handle1TVolume(step, analysisManager);
            break;
        case 3: // Degrader
            HandleDegrader(step, analysisManager);
            break;
        case 4: // Bore Hole
            HandleBoreHole(step, analysisManager);
            break;
        case 5: // Plastic Support
            HandlePlasticSupport(step, analysisManager);
            break;
        case 6: // Primary Coil
            HandlePrimaryCoil(step, analysisManager);
            break;
        case 7: // Correction Coil
            HandleCorrectionCoil(step, analysisManager);
            break;
        case 8: // LHe Vessel
            HandleLHeVessel(step, analysisManager);
            break;
        case 9: // Inner LN Vessel
            HandleInnerLnVessel(step, analysisManager);
            break;
        case 10: // Outer LN Vessel
            HandleOuterLnVessel(step, analysisManager);
            break;
        case 11: // Outer Cryostat Wall
            HandleOuterCryostatWall(step, analysisManager);
            break;
        default:
            // No action for unrecognized copy numbers
            break;
    }

}


// Define the specific actions for each volume
void MySteppingAction::Handle5TVolume(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(0, energy); // Histogram for 5T Volume
}

void MySteppingAction::Handle1TVolume(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(1, energy);
}

void MySteppingAction::HandleDegrader(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(2, energy); 
}

void MySteppingAction::HandleBoreHole(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(3, energy); 
}

void MySteppingAction::HandlePlasticSupport(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(4, energy); 
}

void MySteppingAction::HandlePrimaryCoil(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(5, energy); 
}

void MySteppingAction::HandleCorrectionCoil(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(6, energy); 
}

void MySteppingAction::HandleLHeVessel(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(7, energy); 
}

void MySteppingAction::HandleInnerLnVessel(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(8, energy); 
}

void MySteppingAction::HandleOuterLnVessel(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(9, energy); 
}

void MySteppingAction::HandleOuterCryostatWall(const G4Step* step, G4AnalysisManager* analysisManager) {
    G4double energy = step->GetPreStepPoint()->GetKineticEnergy();
    analysisManager->FillH1(10, energy); 
}


