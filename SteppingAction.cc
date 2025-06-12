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
<<<<<<< HEAD
<<<<<<< HEAD
#include <iomanip>
=======
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
>>>>>>> 6019b63 (new update)

=======
#include <iomanip>

>>>>>>> 012bc44 (new changes)
MySteppingAction::MySteppingAction(MyEventAction *eventAction)
: fEventAction(eventAction) {}

MySteppingAction::~MySteppingAction() {}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 012bc44 (new changes)
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
<<<<<<< HEAD
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
=======
void MySteppingAction::writeToFile(const G4String& tag,
                                   G4double x, G4double y, G4double z,
                                   G4double energyDep,
                                   const G4ThreeVector& momentum)
{
    const std::string filePath = "PionInteractions.dat";
    std::ofstream res(filePath, std::ios::app);

    if (res.is_open()) {
        res << std::left << std::setw(18) << tag
>>>>>>> 6019b63 (new update)
=======
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
>>>>>>> 012bc44 (new changes)
            << std::fixed << std::setprecision(3)
            << std::right
            << std::setw(10) << x
            << std::setw(10) << y
            << std::setw(10) << z
<<<<<<< HEAD
<<<<<<< HEAD
            << std::setw(12) << time/ns
=======
>>>>>>> 6019b63 (new update)
=======
            << std::setw(12) << time/ns
>>>>>>> 012bc44 (new changes)
            << std::setw(15) << energyDep / MeV
            << std::setw(12) << momentum.x() / MeV
            << std::setw(12) << momentum.y() / MeV
            << std::setw(12) << momentum.z() / MeV
            << std::endl;
<<<<<<< HEAD
<<<<<<< HEAD
=======

        res.close();
>>>>>>> 6019b63 (new update)
=======
>>>>>>> 012bc44 (new changes)
    } else {
        G4cerr << "[ERROR] PionInteractions output file is not open!" << G4endl;
    }
}

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    G4StepPoint* preStepPoint  = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();
    G4VPhysicalVolume* volume  = preStepPoint->GetPhysicalVolume();
<<<<<<< HEAD
<<<<<<< HEAD
=======
    if (!volume) return;
>>>>>>> 6019b63 (new update)
=======
>>>>>>> 012bc44 (new changes)

    G4String volumeName = volume->GetName();
    G4int copyNumber = volume->GetCopyNo();

    G4Track* track = step->GetTrack();
    G4String particleName = track->GetDefinition()->GetParticleName();

<<<<<<< HEAD
<<<<<<< HEAD
=======

>>>>>>> 6019b63 (new update)
=======
>>>>>>> 012bc44 (new changes)
    G4ThreeVector posPre  = preStepPoint->GetPosition();
    G4ThreeVector posPost = postStepPoint->GetPosition();
    G4double x = posPre.x() / cm;
    G4double y = posPre.y() / cm;
    G4double z = posPre.z() / cm;
    G4double energyDep = step->GetTotalEnergyDeposit();
<<<<<<< HEAD

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
=======

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
<<<<<<< HEAD
            // Exit
            G4ThreeVector momOut = postStepPoint->GetMomentum();
            G4ThreeVector posOut = postStepPoint->GetPosition();
            writeToFile("Front (Right) Out", posOut.x()/cm, posOut.y()/cm, posOut.z()/cm, 0., momOut);
>>>>>>> 6019b63 (new update)
=======
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
>>>>>>> 012bc44 (new changes)
        }
    }

    // Right Back: z in [20, 70] cm
    if (copyNumber >= 100 && copyNumber < 200 && z > 20 && z < 70) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
<<<<<<< HEAD
<<<<<<< HEAD
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
=======
            G4ThreeVector momIn = preStepPoint->GetMomentum();
            fEventAction->SetBackPoint(posPre);
            writeToFile("Back (Right) In", x, y, z, energyDep, momIn);
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            G4ThreeVector momOut = postStepPoint->GetMomentum();
            G4ThreeVector posOut = postStepPoint->GetPosition();
            writeToFile("Back (Right) Out", posOut.x()/cm, posOut.y()/cm, posOut.z()/cm, 0., momOut);
>>>>>>> 6019b63 (new update)
=======
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
>>>>>>> 012bc44 (new changes)
        }
    }

    // Left Back: z in [-70, -20] cm
    if (copyNumber >= 200 && copyNumber < 300 && z > -70 && z < -20) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
<<<<<<< HEAD
<<<<<<< HEAD
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
=======
            G4ThreeVector momIn = preStepPoint->GetMomentum();
            fEventAction->SetBackPoint(posPre);
            writeToFile("Back (Left) In", x, y, z, energyDep, momIn);
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            G4ThreeVector momOut = postStepPoint->GetMomentum();
            G4ThreeVector posOut = postStepPoint->GetPosition();
            writeToFile("Back (Left) Out", posOut.x()/cm, posOut.y()/cm, posOut.z()/cm, 0., momOut);
>>>>>>> 6019b63 (new update)
=======
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
>>>>>>> 012bc44 (new changes)
        }
    }

    // Left Front: z in [-70, -20] cm
    if (copyNumber >= 300 && copyNumber < 400 && z > -70 && z < -20) {
        if (preStepPoint->GetStepStatus() == fGeomBoundary) {
<<<<<<< HEAD
<<<<<<< HEAD
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
=======
            G4ThreeVector momIn = preStepPoint->GetMomentum();
            fEventAction->SetFrontPoint(posPre);
            writeToFile("Front (Left) In", x, y, z, energyDep, momIn);
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            G4ThreeVector momOut = postStepPoint->GetMomentum();
            G4ThreeVector posOut = postStepPoint->GetPosition();
            writeToFile("Front (Left) Out", posOut.x()/cm, posOut.y()/cm, posOut.z()/cm, 0., momOut);
>>>>>>> 6019b63 (new update)
=======
            writeToFile(out, x, y, z, preStepPoint->GetGlobalTime(), energyDep, preStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "in");
        }
        if (postStepPoint->GetStepStatus() == fGeomBoundary) {
            writeToFile(out, posPost.x()/cm, posPost.y()/cm, posPost.z()/cm, postStepPoint->GetGlobalTime(), 0., postStepPoint->GetMomentum(),
                        eventID, parentID, stepID, trackID, processName, particleName, volumeName, copyNumber, "out");
>>>>>>> 012bc44 (new changes)
        }
    }

    // Accumulate total energy deposition
    if (energyDep > 0.) {
        fEventAction->AddEnergyDep(energyDep);
    }
}