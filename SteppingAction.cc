#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AnalysisManager.hh"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "G4SystemOfUnits.hh"
using namespace CLHEP;


using namespace std;

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
    : fEventAction(eventAction) {}

MySteppingAction::~MySteppingAction() {}

void MySteppingAction::writeToFile(const G4String& tag, G4double x, G4double y, G4double z, G4double energyDep)
{
    const std::string filePath = "PionInteractions.dat";
    std::ofstream res(filePath, std::ios::app);

    if (res.is_open()) {
        res << std::left << std::setw(12) << tag
            << std::right << std::fixed << std::setprecision(5)
            << std::setw(10) << x / cm 
            << std::setw(10) << y / cm
            << std::setw(10) << z / cm
            << std::setw(15) << energyDep / MeV << " MeV" << std::endl;

        res.close();
    } else {
        std::cerr << "[ERROR] Could not open PionInteractions.dat for writing!" << std::endl;
    }
}

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4VPhysicalVolume* volume = preStepPoint->GetPhysicalVolume();
    if (!volume) return;

    G4String volumeName = volume->GetName();
    G4int copyNumber = volume->GetCopyNo();

    G4ThreeVector position = preStepPoint->GetPosition();
    G4double x = position.x() / cm;
    G4double y = position.y() / cm;
    G4double z = position.z() / cm;
    G4double energyDep = step->GetTotalEnergyDeposit();
    

    // std::cout << "[DEBUG] Volume: " << volumeName << ", Copy Number: " << copyNumber << ", Z: " << z << std::endl;

    // Right side front layer
    if (copyNumber >= 0 && copyNumber < 100 && z > 40 && z < 50) {

        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            fEventAction->SetFrontPoint(position);  // Pass to EventAction
            writeToFile("Front (Right)", x, y, z, energyDep);
        }
    }

    // Right side back layer
    if (copyNumber >= 100 && copyNumber < 200 && z > 40 && z < 50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            fEventAction->SetBackPoint(position);  // Pass to EventAction
            writeToFile("Back (Right)", x, y, z, energyDep);
        }
    }

    // Left side front layer
    if (copyNumber >= 300 && copyNumber < 400 && z < -40 && z > -50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            fEventAction->SetFrontPoint(position);  // Pass to EventAction
            writeToFile("Front (Left)", x, y, z, energyDep);
        }
    }

    // Left side back layer
    if (copyNumber >= 200 && copyNumber < 300 && z < -40 && z > -50) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            fEventAction->SetBackPoint(position);  // Pass to EventAction
            writeToFile("Back (Left)", x, y, z, energyDep);
        }
    }

    // Accumulate energy deposition for integral scoring
    if (energyDep > 0.) {
        fEventAction->AddEnergyDep(energyDep);
    }
}