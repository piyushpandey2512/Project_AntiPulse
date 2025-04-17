#include "RunAction.hh"

MyRunAction::MyRunAction()
	: fOutputFileName("Project_AntiPulse") 
{}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
	G4AnalysisManager *manager = G4AnalysisManager::Instance();
	manager->SetDefaultFileType("root");

	// Opening the ROOT file with the dynamic name
	G4String fileName = fOutputFileName + ".root";
	manager->OpenFile(fileName);
	
	manager->CreateNtuple("Output","Output");
	// Lets we want to write information of Events, hit-position of detector,..
	manager->CreateNtupleIColumn("fEvent");
	manager->CreateNtupleDColumn("fXcor");
	manager->CreateNtupleDColumn("fYcor");
	manager->CreateNtupleDColumn("fZcor");
	//New
	manager->CreateNtupleIColumn("fParentID");
	manager->CreateNtupleIColumn("fTrackID");
	manager->CreateNtupleIColumn("fStepID");
	manager->CreateNtupleSColumn("fProcess");
	manager->CreateNtupleDColumn("fTimeL");
	manager->CreateNtupleDColumn("fTimeG");
	manager->CreateNtupleDColumn("fEnergyDeposition");
	manager->CreateNtupleIColumn("fCopyID");
	manager->CreateNtupleSColumn("fParticles");
	
	manager->FinishNtuple(0);  // it start with 0, for more tuple, change id's$
	
	// Create ntuple for the integral information
	manager->CreateNtuple("IntScoring", "IntScoring");
	manager->CreateNtupleDColumn("Energy Deposition");
	manager->FinishNtuple(1);

	 // Add histograms for specific volumes or properties
    CreateHistogramWithTitles(manager, "EnergyDep_5T", "Energy Deposition in 5T Volume (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_1T", "Energy Deposition in 1T Volume (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_Degrader", "Energy Deposition in Degrader (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_BoreHole", "Energy Deposition in Bore Hole (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_PlasticSupport", "Energy Deposition in Plastic Support (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_PrimaryCoil", "Energy Deposition in Primary Coil (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_CorrectionCoil", "Energy Deposition in Correction Coil (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_LHeVessel", "Energy Deposition in LHe Vessel (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_InnerLnVessel", "Energy Deposition in Inner LN Vessel (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_OuterLnVessel", "Energy Deposition in Outer LN Vessel (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_OuterCryostatWall", "Energy Deposition in Outer Cryostat Wall (keV)", 100, 0, 100 * keV);
	CreateHistogramWithTitles(manager, "EnergyDep_J-PET", "Energy Deposition on J-PET modules(keV)", 100, 0, 100 * keV);
	
}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
	// Get the number of events in the run
    G4int numberOfEvents = run->GetNumberOfEvent();

    // Print a message to indicate the run is complete
    G4cout << "Run completed with " << numberOfEvents << " events." << G4endl;

	G4AnalysisManager *manager = G4AnalysisManager::Instance();
	//First write the file
	manager->Write();
	//Close the file
	manager->CloseFile();
}


void MyRunAction::CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double xmin, G4double xmax) {
    G4int id = manager->CreateH1(name, title, bins, xmin, xmax);
    manager->SetH1XAxisTitle(id, "Energy [keV]");
    manager->SetH1YAxisTitle(id, "Counts");
}


void MyRunAction::SetOutputFileName(const G4String& name) {
    fOutputFileName = name;
}