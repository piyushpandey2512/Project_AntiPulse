#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh" 
#include "G4Run.hh"

class MyRunAction : public G4UserRunAction
{
public:
    MyRunAction();
    ~MyRunAction();
    
    // Methods for beginning and end of run
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run* run);

    // Set the output file name
    void SetOutputFileName(const G4String& fileName);

private:
    G4String fOutputFileName;

    // Helper method to create histograms
    void CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double min, G4double max);
};

#endif