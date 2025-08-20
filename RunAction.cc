#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh" 
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


    manager->CreateH3("SourceXYZDistribution", "Primary Particle Source Position;X (cm);Y (cm);Z (cm)",
                      50, -12.0*cm, -4.0*cm,    // X-axis: 50 bins from -12 cm to -4 cm
                      50,  0.0*cm,  7.0*cm,     // Y-axis: 50 bins from 0 cm to 7 cm
                      100, -60.0*cm, 60.0*cm);   // Z-axis: 100 bins from -60 cm to 60 cm

    CreateHistogramWithTitles(manager, "ScintillatorHits", "Scintillator Copy Numbers (Copy Number)", 400, 0, 400);
    CreateHistogramWithTitles(manager, "PionEnergyDep", "Energy Deposition by Pions (MeV)", 100, 0, 10);
    // CreateHistogramWithTitles(manager, "AngularDeviation", "Angular Deviation of Pion Tracks (deg)", 100, 0, 20);
    manager->CreateH2("Edep2DByProcess", "Energy Deposition by Process;Process Index;Edep (MeV)",
                    13, 0, 13,    // 13 process types, bins 0-12
                    100, 0, 10);  // 0-10 MeV, 100 bins
    manager->CreateH1("IntraModuleDeviation", "Intra-module angular deviation;Deviation (deg);Counts", 180, 0., 180.);
    manager->CreateH1("InterModuleDeviation", "Inter-module angular deviation;Deviation (deg);Counts", 180, 0., 180.);
    manager->CreateH1("SingleScintDeviation", "Angular Deviation in Test Scintillator;Angle (degrees);Counts", 100, 0, 10);
    manager->CreateH1("TwoScintB2BDeviation", "Angular Deviation Between B2B Scintillators;Angle (degrees);Counts", 100, 0, 10);   
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