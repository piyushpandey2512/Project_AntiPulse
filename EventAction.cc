#include "EventAction.hh"

MyEventAction::MyEventAction(MyRunAction*)
{
    fEnergyDeposition = 0;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
    fEnergyDeposition = 0; // Reset energy deposition at the start of each event
}

void MyEventAction::EndOfEventAction(const G4Event* event)
{
    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    manager->FillNtupleDColumn(1, 0, fEnergyDeposition); // Ensure column type matches
    manager->AddNtupleRow(1);

    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ((printModulo > 0) && (eventID % printModulo == 0)) {
        G4cout << "--> Event " << eventID << " completed. Total energy: "
               << G4BestUnit(fEnergyDeposition, "Energy") << G4endl;
    }
}