#include "EventAction.hh"

MyEventAction::MyEventAction(MyRunAction*)
{
<<<<<<< HEAD
	fEnergyDeposition=0;
=======
    fEnergyDeposition = 0;
>>>>>>> 3ac4172 (Initial fresh commit)
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
<<<<<<< HEAD
	fEnergyDeposition=0; // Important to reinitialize in starting of event
=======
    fEnergyDeposition = 0; // Reset energy deposition at the start of each event
>>>>>>> 3ac4172 (Initial fresh commit)
}

void MyEventAction::EndOfEventAction(const G4Event* event)
{
<<<<<<< HEAD
	//G4cout<<"Energy deposition = "<<fEnergyDeposition<<G4endl;
	G4AnalysisManager *manager =  G4AnalysisManager::Instance();
	manager->FillNtupleDColumn(1,0,fEnergyDeposition);
	manager->AddNtupleRow(1);

	auto eventID = event->GetEventID();
	auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
	if ((printModulo > 0) && (eventID % printModulo == 0)) {
		G4cout << "--> Event " << eventID << "start" << "\n" << G4endl;
		G4cout << "Toal energy absorbed by Sensitive Detector: " << std::setw(7) << G4BestUnit(fEnergyDeposition, "Energy");
		G4cout << "--> End of event " << eventID << "\n" << G4endl;
	}
}
=======
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
>>>>>>> 3ac4172 (Initial fresh commit)
