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

MyEventAction::MyEventAction(MyRunAction*)
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

    // ---- Vertex reconstruction: z = +45 cm plane ----
    for (const auto& front : frontHits45) {
        G4int trackID = front.first;
        if (backHits45.count(trackID)) {
            const auto& p1 = front.second.pos_cm;
            const auto& p2 = backHits45[trackID].pos_cm;

            G4double dx = p2.x() - p1.x();
            if (dx == 0) continue;

            G4double t = -p1.x() / dx;
            G4double y0 = p1.y() + t * (p2.y() - p1.y());
            G4double z0 = p1.z();  // z stays same in this plane

            manager->FillH2(0, z0, y0);
        }
    }

    // ---- Vertex reconstruction: z = -45 cm plane ----
    for (const auto& front : frontHitsMinus45) {
        G4int trackID = front.first;
        if (backHitsMinus45.count(trackID)) {
            const auto& p1 = front.second.pos_cm;
            const auto& p2 = backHitsMinus45[trackID].pos_cm;

            G4double dx = p2.x() - p1.x();
            if (dx == 0) continue;

            G4double t = -p1.x() / dx;
            G4double y0 = p1.y() + t * (p2.y() - p1.y());
            G4double z0 = p1.z();  // z stays same in this plane

            manager->FillH2(0, z0, y0);
        }
    }

    // Clear buffers after processing
    ClearHitBuffers();

    // ---- Angular deviation calculation ----
    if (hasFront && hasBack) {
        G4ThreeVector direction = fBackPosition - fFrontPosition;
        G4ThreeVector reference = G4ThreeVector(0, 0, 1); // Z-axis reference

        if (direction.mag() > 0) {
            G4double cosTheta = direction.unit().dot(reference);
            cosTheta = std::clamp(cosTheta, -1.0, 1.0);
            G4double angleRad = std::acos(cosTheta);
            G4double angleDeg = angleRad * (180.0 / CLHEP::pi);

            manager->FillH1(manager->GetH1Id("AngularDeviation"), angleDeg);
        }
    }

    // ---- Print event summary ----
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ((printModulo > 0) && (eventID % printModulo == 0)) {
        G4cout << "--> Event " << eventID << " completed. Total energy: "
               << G4BestUnit(fEnergyDeposition, "Energy") << G4endl;
    }
}

void MyEventAction::SetFrontPoint(const G4ThreeVector& front) {
    fFrontPosition = front;
    hasFront = true;
}

void MyEventAction::SetBackPoint(const G4ThreeVector& back) {
    fBackPosition = back;
    hasBack = true;
}
