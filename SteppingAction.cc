#include "SteppingAction.hh"
<<<<<<< HEAD
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


=======
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AnalysisManager.hh"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
    : fEventAction(eventAction) {}

MySteppingAction::~MySteppingAction() {}

// Define these maps globally or as class members
std::map<G4int, G4ThreeVector> entryPointMap1; // Right-side front
std::map<G4int, G4ThreeVector> entryPointMap2; // Left-side front


void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    // Get the pre-step point and volume
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4VPhysicalVolume* volume = preStepPoint->GetPhysicalVolume();
    if (!volume) return;

    G4String volumeName = volume->GetName();
    G4cout << "[DEBUG] Step detected in volume: " << volumeName << G4endl;

    // Check if the step is in the scintillator volume
    if (volumeName != "ScintillatorLV") return;

    // Extract copy number and event information
    G4int copyNumber = volume->GetCopyNo();
    const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
    G4int eventID = event->GetEventID();
    G4cout << "[DEBUG] Event ID: " << eventID << ", Copy Number: " << copyNumber << G4endl;

    // Get the position and energy deposition
    G4ThreeVector position = preStepPoint->GetPosition();
    G4double x = position.x() / cm;
    G4double y = position.y() / cm;
    G4double z = position.z() / cm;
    G4double energyDep = step->GetTotalEnergyDeposit();
    G4cout << "[DEBUG] Position: (" << x << ", " << y << ", " << z << "), Energy Deposition: " << energyDep / MeV << " MeV" << G4endl;

    // Get the analysis manager instance
    auto analysisManager = G4AnalysisManager::Instance();

    // Front surface - Right side
    if (copyNumber >= 0 && copyNumber < 100 && z > 40 && z < 50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap1.find(eventID) == entryPointMap1.end()) {
            entryPointMap1[eventID] = position;
            G4cout << "[DEBUG] Front (Right) entry point recorded for Event ID: " << eventID << G4endl;

            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            // Write to file
            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Front (Right)\t"
                    << x << "\t" << y << "\t" << z << "\t"
                    << energyDep / MeV << " MeV" << std::endl;
                G4cout << "[DEBUG] Data written to PionInteractions.dat for Front (Right)." << G4endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }
        }
    }

    // Back module - Right side
    if (copyNumber >= 100 && copyNumber < 200 && z > 40 && z < 50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap1.find(eventID) != entryPointMap1.end()) {
            G4cout << "[DEBUG] Back (Right) detected for Event ID: " << eventID << G4endl;

            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            // Write to file
            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Back (Right)\t"
                    << x << "\t" << y << "\t" << z << "\t"
                    << energyDep / MeV << " MeV" << std::endl;
                G4cout << "[DEBUG] Data written to PionInteractions.dat for Back (Right)." << G4endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }

            entryPointMap1.erase(eventID);
            step->GetTrack()->SetTrackStatus(fStopAndKill);
            G4cout << "[DEBUG] Track stopped for Event ID: " << eventID << G4endl;
        }
    }

    // Front surface - Left side
    if (copyNumber >= 300 && copyNumber < 400 && z < -40 && z > -50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap2.find(eventID) == entryPointMap2.end()) {
            entryPointMap2[eventID] = position;
            G4cout << "[DEBUG] Front (Left) entry point recorded for Event ID: " << eventID << G4endl;

            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            // Write to file
            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Front (Left)\t"
                    << x << "\t" << y << "\t" << z << "\t"
                    << energyDep / MeV << " MeV" << std::endl;
                G4cout << "[DEBUG] Data written to PionInteractions.dat for Front (Left)." << G4endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }
        }
    }

    // Back module - Left side
    if (copyNumber >= 200 && copyNumber < 300 && z < -40 && z > -50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap2.find(eventID) != entryPointMap2.end()) {
            G4cout << "[DEBUG] Back (Left) detected for Event ID: " << eventID << G4endl;

            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            // Write to file
            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Back (Left)\t"
                    << x << "\t" << y << "\t" << z << "\t"
                    << energyDep / MeV << " MeV" << std::endl;
                G4cout << "[DEBUG] Data written to PionInteractions.dat for Back (Left)." << G4endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }

            entryPointMap2.erase(eventID);
            step->GetTrack()->SetTrackStatus(fStopAndKill);
            G4cout << "[DEBUG] Track stopped for Event ID: " << eventID << G4endl;
        }
    }
}





/*
void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4VPhysicalVolume* volume = preStepPoint->GetPhysicalVolume();
    if (!volume) return;

    G4String volumeName = volume->GetName();
    if (volumeName != "ScintillatorLV") return;

    G4int copyNumber = volume->GetCopyNo();
    const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
    G4int eventID = event->GetEventID();

    G4ThreeVector position = preStepPoint->GetPosition();
    G4double x = position.x() / cm;
    G4double y = position.y() / cm;
    G4double z = position.z() / cm;

    G4double energyDep = step->GetTotalEnergyDeposit();

    // Get the analysis manager instance
    auto analysisManager = G4AnalysisManager::Instance();

    // Front surface - Right side
    if (copyNumber >= 0 && copyNumber < 100 && z > 40 && z < 50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap1.find(eventID) == entryPointMap1.end()) {
            entryPointMap1[eventID] = position;

            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Front (Right)\t"
                    << x << "\t" << y << "\t" << z << "\t"
                    << energyDep / MeV << " MeV" << std::endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }
    }

    // Back module - Right side
    if (copyNumber >= 100 && copyNumber < 200 && z > 40 && z < 50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap1.find(eventID) != entryPointMap1.end()) {
            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Back (Right)\t"
                << x << "\t" << y << "\t" << z << "\t"
                << energyDep / MeV << " MeV" << std::endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }

            entryPointMap1.erase(eventID);
            step->GetTrack()->SetTrackStatus(fStopAndKill);
        }
    }

    // Front surface - Left side
    if (copyNumber >= 300 && copyNumber < 400 && z < -40 && z > -50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap2.find(eventID) == entryPointMap2.end()) {
            entryPointMap2[eventID] = position;

            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Front (Left)\t"
                << x << "\t" << y << "\t" << z << "\t"
                << energyDep / MeV << " MeV" << std::endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }
        }
    }

    // Back module - Left side
    if (copyNumber >= 200 && copyNumber < 300 && z < -40 && z > -50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary && entryPointMap2.find(eventID) != entryPointMap2.end()) {
            // Fill histograms
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition

            std::ofstream res("PionInteractions.dat", std::ios::app);
            if (res.is_open()) {
                res << "Back (Left)\t"
                << x << "\t" << y << "\t" << z << "\t"
                << energyDep / MeV << " MeV" << std::endl;
            } else {
                G4cerr << "Error: Could not open PionInteractions.dat for writing!" << G4endl;
            }

            entryPointMap2.erase(eventID);
            step->GetTrack()->SetTrackStatus(fStopAndKill);
        }
    }
}
}
*/



 /*
    // Extract event-level information
    const G4Event *event = G4RunManager::GetRunManager()->GetCurrentEvent();
    G4int eventID = event->GetEventID();
    G4int parentID = step->GetTrack()->GetParentID();
    G4int trackID = step->GetTrack()->GetTrackID();
    G4int stepID = step->GetTrack()->GetCurrentStepNumber();
    G4String processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();

    // Check if the step is in the scintillator volume
    if (volume->GetLogicalVolume()->GetName() == "ScintillatorLV")
    {
        // Fill histograms for energy deposition and copy number
        if (energyDep > 0)
        {
            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(0, copyNumber);          // Histogram for copy numbers
            analysisManager->FillH1(1, energyDep / MeV);     // Histogram for energy deposition
        }
    }

    // Write step information to a file
    std::ofstream file("Information.txt", std::ios::app);
    file << setw(9) << eventID << "\t"
         << setw(3) << parentID << "\t"
         << setw(3) << trackID << "\t"
         << setw(5) << stepID << "\t"
         << setw(15) << processName << "\t"
         << setw(8) << energyDep / MeV << "\t"
         << setw(15) << volume->GetName() << "\t"
         << setw(3) << copyNumber << G4endl;
    file.close();
    */
>>>>>>> 3ac4172 (Initial fresh commit)
