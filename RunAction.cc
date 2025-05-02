#include "RunAction.hh"
<<<<<<< HEAD

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
	
=======
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

MyRunAction::MyRunAction()
    : fOutputFileName("Project_AntiPulse") 
{
    G4cout << "[DEBUG] RunAction constructor called. Output file name set to: " << fOutputFileName << G4endl;
}

MyRunAction::~MyRunAction()
{
    G4cout << "[DEBUG] RunAction destructor called." << G4endl;
}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
    // G4cout << "[DEBUG] BeginOfRunAction called. Initializing geometry..." << G4endl;
    // G4RunManager::GetRunManager()->ReinitializeGeometry();
    // G4cout << "[DEBUG] Geometry initialized." << G4endl;

    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    G4cout << "[DEBUG] AnalysisManager instance retrieved." << G4endl;

    manager->SetDefaultFileType("root");
    G4cout << "[DEBUG] Default file type set to ROOT." << G4endl;

    // Reset histograms at the beginning of the run
    manager->Reset();
    G4cout << "[DEBUG] Histograms reset." << G4endl;

    // Opening the ROOT file with the dynamic name
    G4String fileName = fOutputFileName + ".root";
    manager->OpenFile(fileName);
    G4cout << "[DEBUG] ROOT file opened: " << fileName << G4endl;

    // Create ntuple for detailed event information
    G4cout << "[DEBUG] Creating ntuple for detailed event information." << G4endl;
    manager->CreateNtuple("Output", "Output");
    manager->CreateNtupleIColumn("fEvent"); // Integer column
    manager->CreateNtupleDColumn("fXcor"); // Double column
    manager->CreateNtupleDColumn("fYcor"); // Double column
    manager->CreateNtupleDColumn("fZcor"); // Double column
    manager->CreateNtupleIColumn("fParentID"); // Integer column
    manager->CreateNtupleIColumn("fTrackID"); // Integer column
    manager->CreateNtupleIColumn("fStepID"); // Integer column
    manager->CreateNtupleSColumn("fProcess"); // String column
    manager->CreateNtupleDColumn("fTimeL"); // Double column
    manager->CreateNtupleDColumn("fTimeG"); // Double column
    manager->CreateNtupleDColumn("fEnergyDeposition"); // Double column
    manager->CreateNtupleIColumn("fCopyID"); // Integer column
    manager->CreateNtupleSColumn("fParticles"); // String column
    manager->FinishNtuple(0);
    G4cout << "[DEBUG] Ntuple for detailed event information created." << G4endl;

    // Create ntuple for integral information
    G4cout << "[DEBUG] Creating ntuple for integral information." << G4endl;
    manager->CreateNtuple("IntScoring", "IntScoring");
    manager->CreateNtupleDColumn("Energy Deposition"); // Double column
    manager->FinishNtuple(1);
    G4cout << "[DEBUG] Ntuple for integral information created." << G4endl;

    // Create histograms
    G4cout << "[DEBUG] Creating histograms." << G4endl;
    CreateHistogramWithTitles(manager, "ScintillatorHits", "Scintillator Copy Numbers;Copy Number;Counts", 400, 0, 400);
    CreateHistogramWithTitles(manager, "PionEnergyDep", "Energy Deposition by Pions;Energy (MeV);Counts", 100, 0, 10);
    G4cout << "[DEBUG] Histograms created." << G4endl;
>>>>>>> 3ac4172 (Initial fresh commit)
}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
<<<<<<< HEAD
	// Get the number of events in the run
    G4int numberOfEvents = run->GetNumberOfEvent();
=======
    G4cout << "[DEBUG] EndOfRunAction called." << G4endl;

    // Get the number of events in the run
    G4int numberOfEvents = run->GetNumberOfEvent();
    G4cout << "[DEBUG] Number of events processed: " << numberOfEvents << G4endl;
>>>>>>> 3ac4172 (Initial fresh commit)

    // Print a message to indicate the run is complete
    G4cout << "Run completed with " << numberOfEvents << " events." << G4endl;

<<<<<<< HEAD
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
=======
    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    G4cout << "[DEBUG] AnalysisManager instance retrieved in EndOfRunAction." << G4endl;

    // First write the file
    manager->Write();
    G4cout << "[DEBUG] Data written to file." << G4endl;

    // Close the file but keep histograms available for visualization
    manager->CloseFile(false);
    G4cout << "[DEBUG] File closed but histograms kept available for visualization." << G4endl;
}

void MyRunAction::SetOutputFileName(const G4String& fileName)
{
    fOutputFileName = fileName;
    G4cout << "[DEBUG] Output file name set to: " << fOutputFileName << G4endl;
}

void MyRunAction::CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double min, G4double max)
{
    manager->CreateH1(name, title, bins, min, max);
    G4cout << "[DEBUG] Histogram created: " << name << " with title: " << title << G4endl;
>>>>>>> 3ac4172 (Initial fresh commit)
}