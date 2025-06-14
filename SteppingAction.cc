#include "SteppingAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
: fEventAction(eventAction) {}

MySteppingAction::~MySteppingAction() {}

void MySteppingAction::writeToFile(
    std::ofstream& out,
    G4double x, G4double y, G4double z,
    G4double time,
    G4double energyDep,
    const G4ThreeVector& momentum,
    G4int eventID,
    G4int parentID,
    G4int stepID,
    G4int trackID,
    const G4String& processName,
    const G4String& particleName,
    const G4String& volumeName,
    G4int copyNumber,
    const G4String& inout)
{
    if (out.is_open()) {
        out << std::setw(8) << eventID
            << std::setw(8) << parentID
            << std::setw(8) << stepID
            << std::setw(8) << trackID
            << std::setw(20) << processName
            << std::setw(15) << particleName
            << std::setw(18) << volumeName
            << std::setw(8) << copyNumber
            << std::setw(6) << inout
            << std::fixed << std::setprecision(3)
            << std::right
            << std::setw(10) << x
            << std::setw(10) << y
            << std::setw(10) << z
            << std::setw(12) << time/ns
            << std::setw(15) << energyDep / MeV
            << std::setw(12) << momentum.x() / MeV
            << std::setw(12) << momentum.y() / MeV
            << std::setw(12) << momentum.z() / MeV
            << std::endl;
    } else {
        G4cerr << "[ERROR] PionInteractions output file is not open!" << G4endl;
    }
}

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    G4StepPoint* preStepPoint  = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();
    G4VPhysicalVolume* volume  = preStepPoint->GetPhysicalVolume();
    if (!volume) return;

    G4String volumeName = volume->GetName();
    G4int copyNumber = volume->GetCopyNo();

    G4Track* track = step->GetTrack();
    G4String particleName = track->GetDefinition()->GetParticleName();

    G4ThreeVector posPre  = preStepPoint->GetPosition();
    G4ThreeVector posPost = postStepPoint->GetPosition();
    G4double x = posPre.x() / cm;
    G4double y = posPre.y() / cm;
    G4double z = posPre.z() / cm;
    G4double energyDep = step->GetTotalEnergyDeposit();

    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    G4int parentID = track->GetParentID();
    G4int stepID = track->GetCurrentStepNumber();
    G4int trackID = track->GetTrackID();
    G4String processName = "None";
    if (step->GetPostStepPoint()->GetProcessDefinedStep())
        processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    // Get output stream from RunAction (const-correct)
    auto runAction = static_cast<const MyRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
    std::ofstream& out = runAction->GetPionFile();

    // Right Front: z in [20, 70] cm
    if (copyNumber >= 0 && copyNumber < 100 && z > 20 && z < 70) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
        }
    }

    // Right Back: z in [20, 70] cm
    if (copyNumber >= 100 && copyNumber < 200 && z > 20 && z < 70) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
        }
    }

    // Left Front: z in [-70, -20] cm
    if (copyNumber >= 200 && copyNumber < 300 && z > -70 && z < -20) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
        }
    }

    // Left Back: z in [-70, -20] cm
    if (copyNumber >= 300 && copyNumber < 400 && z > -70 && z < -20) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
        }
    }

    // Accumulate total energy deposition
    if (energyDep > 0.) {
        fEventAction->AddEnergyDep(energyDep);
    }
}