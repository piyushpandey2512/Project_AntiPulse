//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May  8 12:10:43 2025 by ROOT version 6.22/02
// from TTree Output/Output
// found on file: Project_AntiPulse_1lac.root
//////////////////////////////////////////////////////////

#ifndef AntiProton_h
#define AntiProton_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

// Header file for the classes stored in the TTree if any.

class AntiProton {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           fEvent;
   Double_t        fXcor;
   Double_t        fYcor;
   Double_t        fZcor;
   Int_t           fParentID;
   Int_t           fTrackID;
   Int_t           fStepID;
   Char_t         fProcess[22];
   Double_t        fTimeL;
   Double_t        fTimeG;
   Double_t        fEnergyDeposition;
   Int_t           fCopyID;
   Char_t         fParticles[13];

   // List of branches
   TBranch        *b_fEvent;   //!
   TBranch        *b_fXcor;   //!
   TBranch        *b_fYcor;   //!
   TBranch        *b_fZcor;   //!
   TBranch        *b_fParentID;   //!
   TBranch        *b_fTrackID;   //!
   TBranch        *b_fStepID;   //!
   TBranch        *b_fProcess;   //!
   TBranch        *b_fTimeL;   //!
   TBranch        *b_fTimeG;   //!
   TBranch        *b_fEnergyDeposition;   //!
   TBranch        *b_fCopyID;   //!
   TBranch        *b_fParticles;   //!

   AntiProton(TTree *tree=0);
   virtual ~AntiProton();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef AntiProton_cxx
AntiProton::AntiProton(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Project_AntiPulse_stl_10lac.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("Project_AntiPulse_stl_10lac.root");
      }
      f->GetObject("Output",tree);

   }
   Init(tree);
}

AntiProton::~AntiProton()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t AntiProton::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t AntiProton::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void AntiProton::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fEvent", &fEvent, &b_fEvent);
   fChain->SetBranchAddress("fXcor", &fXcor, &b_fXcor);
   fChain->SetBranchAddress("fYcor", &fYcor, &b_fYcor);
   fChain->SetBranchAddress("fZcor", &fZcor, &b_fZcor);
   fChain->SetBranchAddress("fParentID", &fParentID, &b_fParentID);
   fChain->SetBranchAddress("fTrackID", &fTrackID, &b_fTrackID);
   fChain->SetBranchAddress("fStepID", &fStepID, &b_fStepID);
   fChain->SetBranchAddress("fProcess", fProcess, &b_fProcess);
   fChain->SetBranchAddress("fTimeL", &fTimeL, &b_fTimeL);
   fChain->SetBranchAddress("fTimeG", &fTimeG, &b_fTimeG);
   fChain->SetBranchAddress("fEnergyDeposition", &fEnergyDeposition, &b_fEnergyDeposition);
   fChain->SetBranchAddress("fCopyID", &fCopyID, &b_fCopyID);
   fChain->SetBranchAddress("fParticles", fParticles, &b_fParticles);
   Notify();
}

Bool_t AntiProton::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void AntiProton::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t AntiProton::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef AntiProton_cxx
