#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh" 
#include "G4Run.hh"
#include <fstream>


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

    // Output file accessors
    std::ofstream& GetPionFile();        // non-const version
    std::ofstream& GetPionFile() const;  // const version

    const G4String& GetTimestamp() const { return fTimestamp; }

    // These "adders" will be called by the EventAction to update the totals.
    void AddPassedG1() { fPassedG1Counter++; }
    void AddPassedG2() { fPassedG2Counter++; }
    void AddHitCounter() { fHitCounterCounter++; }
    void AddAbsorbedG1() { fAbsorbedG1Counter++; }
    void AddAbsorbedG2() { fAbsorbedG2Counter++; }

private:
    G4String fOutputFileName;
    G4String fCurrentFileName;
    std::ofstream fPionFile;
    G4String fTimestamp;

    // Helper method to create histograms
    void CreateHistogramWithTitles(G4AnalysisManager* manager, const G4String& name, const G4String& title, G4int bins, G4double min, G4double max);

    // These will store the total counts for the entire run.
    G4int fPassedG1Counter;
    G4int fPassedG2Counter;
    G4int fHitCounterCounter;
    G4int fAbsorbedG1Counter;
    G4int fAbsorbedG2Counter;
};

#endif