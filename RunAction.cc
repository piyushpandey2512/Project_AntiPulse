#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

MyRunAction::MyRunAction()
    : fOutputFileName("Project_AntiPulse") 
{
}

MyRunAction::~MyRunAction()
{
    if (fPionFile.is_open()) {
        fPionFile.close();
    }
}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
    // Generate timestamp string
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *parts = std::localtime(&now_c);

    std::ostringstream ts;
    ts << std::put_time(parts, "%Y%m%d_%H%M%S");
    fTimestamp = ts.str();

    // Open timestamped PionInteractions.dat file
    std::string pionFileName = "PionInteractions_" + fTimestamp + ".dat";
    fPionFile.open(pionFileName, std::ios::out | std::ios::trunc);

    // Open timestamped ROOT file
    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    manager->SetDefaultFileType("root");
    manager->Reset();

    G4String rootFileName = fOutputFileName + "_" + fTimestamp + ".root";
    fCurrentFileName = rootFileName;
    manager->OpenFile(rootFileName);

    // Create ntuples
    manager->CreateNtuple("Output", "Output");
    manager->CreateNtupleIColumn("fEvent");
    manager->CreateNtupleDColumn("fXcor");
    manager->CreateNtupleDColumn("fYcor");
    manager->CreateNtupleDColumn("fZcor");
    manager->CreateNtupleIColumn("fParentID");
    manager->CreateNtupleIColumn("fTrackID");
    manager->CreateNtupleIColumn("fStepID");
    manager->CreateNtupleSColumn("fProcess");
    manager->CreateNtupleDColumn("fTimeL");
    manager->CreateNtupleDColumn("fTimeG");
    manager->CreateNtupleDColumn("fEnergyDeposition");
    manager->CreateNtupleIColumn("fCopyID");
    manager->CreateNtupleSColumn("fParticles");
    manager->FinishNtuple(0);

    manager->CreateNtuple("IntScoring", "IntScoring");
    manager->CreateNtupleDColumn("Energy Deposition");
    manager->FinishNtuple(1);

    // Create histograms
    CreateHistogramWithTitles(manager, "ScintillatorHits", "Scintillator Copy Numbers (Copy Number)", 400, 0, 400);
    CreateHistogramWithTitles(manager, "PionEnergyDep", "Energy Deposition by Pions (MeV)", 100, 0, 10);
    CreateHistogramWithTitles(manager, "AngularDeviation", "Angular Deviation of Pion Tracks (deg)", 100, 0, 20);
    manager->CreateH2("OriginYZ", "Extrapolated origin at x = 0",
        200, -100, 100,   // z
        200, -20, 20);    // y
}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager *manager = G4AnalysisManager::Instance();
    manager->Write();
    manager->CloseFile();

    if (fPionFile.is_open()) {
        fPionFile.close();
    }
}

void MyRunAction::SetOutputFileName(const G4String& fileName)
{
    fOutputFileName = fileName;
}

std::ofstream& MyRunAction::GetPionFile()
{
    return fPionFile;
}

std::ofstream& MyRunAction::GetPionFile() const
{
    // const_cast is safe here because fPionFile is mutable in practice (file stream)
    return const_cast<std::ofstream&>(fPionFile);
}

void MyRunAction::CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double min, G4double max)
{
    manager->CreateH1(name, title, bins, min, max);
}