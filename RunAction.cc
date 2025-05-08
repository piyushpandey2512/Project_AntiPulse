#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

MyRunAction::MyRunAction()
    : fOutputFileName("Project_AntiPulse") 
{
    // G4cout << "[DEBUG] RunAction constructor called. Output file name set to: " << fOutputFileName << G4endl;
}

MyRunAction::~MyRunAction()
{
    // G4cout << "[DEBUG] RunAction destructor called." << G4endl;
}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
    // G4cout << "[DEBUG] BeginOfRunAction called. Initializing geometry..." << G4endl;
    // G4RunManager::GetRunManager()->ReinitializeGeometry();
    // G4cout << "[DEBUG] Geometry initialized." << G4endl;

    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    // G4cout << "[DEBUG] AnalysisManager instance retrieved." << G4endl;

    manager->SetDefaultFileType("root");
    // G4cout << "[DEBUG] Default file type set to ROOT." << G4endl;

    // Reset histograms at the beginning of the run
    manager->Reset();
    // G4cout << "[DEBUG] Histograms reset." << G4endl;

    // Opening the ROOT file with the dynamic name
    G4String fileName = fOutputFileName + ".root";
    manager->OpenFile(fileName);
    // G4cout << "[DEBUG] ROOT file opened: " << fileName << G4endl;

    // Create ntuple for detailed event information
    // G4cout << "[DEBUG] Creating ntuple for detailed event information." << G4endl;
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
    // G4cout << "[DEBUG] Ntuple for detailed event information created." << G4endl;

    // Create ntuple for integral information
    // G4cout << "[DEBUG] Creating ntuple for integral information." << G4endl;
    manager->CreateNtuple("IntScoring", "IntScoring");
    manager->CreateNtupleDColumn("Energy Deposition"); // Double column
    manager->FinishNtuple(1);
    // G4cout << "[DEBUG] Ntuple for integral information created." << G4endl;

    // Create histograms
    // G4cout << "[DEBUG] Creating histograms." << G4endl;
    CreateHistogramWithTitles(manager, "ScintillatorHits", "Scintillator Copy Numbers;Copy Number;Counts", 400, 0, 400);
    CreateHistogramWithTitles(manager, "PionEnergyDep", "Energy Deposition by Pions;Energy (MeV);Counts", 100, 0, 10);
    // G4cout << "[DEBUG] Histograms created." << G4endl;
}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
    // G4cout << "[DEBUG] EndOfRunAction called." << G4endl;

    // Get the number of events in the run
    G4int numberOfEvents = run->GetNumberOfEvent();
    // G4cout << "[DEBUG] Number of events processed: " << numberOfEvents << G4endl;

    // Print a message to indicate the run is complete
    // G4cout << "Run completed with " << numberOfEvents << " events." << G4endl;

    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    // G4cout << "[DEBUG] AnalysisManager instance retrieved in EndOfRunAction." << G4endl;

    // First write the file
    manager->Write();
    // G4cout << "[DEBUG] Data written to file." << G4endl;

    // Close the file but keep histograms available for visualization
    manager->CloseFile();
    // G4cout << "[DEBUG] File closed but histograms kept available for visualization." << G4endl;
}

void MyRunAction::SetOutputFileName(const G4String& fileName)
{
    fOutputFileName = fileName;
    // G4cout << "[DEBUG] Output file name set to: " << fOutputFileName << G4endl;
}

void MyRunAction::CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double min, G4double max)
{
    manager->CreateH1(name, title, bins, min, max);
    // G4cout << "[DEBUG] Histogram created: " << name << " with title: " << title << G4endl;
}