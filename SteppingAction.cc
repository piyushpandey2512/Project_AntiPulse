#include "SteppingAction.hh"
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