#include "EventAction.hh"

MyEventAction::MyEventAction(MyRunAction*)
{
	fEnergyDeposition=0;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
	fEnergyDeposition=0; // Important to reinitialize in starting of event
}

void MyEventAction::EndOfEventAction(const G4Event* event)
{
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
