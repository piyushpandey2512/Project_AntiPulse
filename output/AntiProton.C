#define AntiProton_cxx
#include "AntiProton.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void AntiProton::Loop()
{
//   In a ROOT session, you can do:
//      root> .L AntiProton.C
//      root> AntiProton t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch


   if (fChain == 0) return;

   // Define 1D histograms for x, y, and z coordinates
   TH1D *hXcor = new TH1D("hXcor", "X Coordinate Distribution;X (cm);Entries", 100, -100, 100);
   TH1D *hYcor = new TH1D("hYcor", "Y Coordinate Distribution;Y (cm);Entries", 100, -100, 100);
   TH1D *hZcor = new TH1D("hZcor", "Z Coordinate Distribution;Z (cm);Entries", 100, -100, 100);

   // Define 2D histograms for xy, yz, and zx coordinates
   TH2D *hXY = new TH2D("hXY", "XY Coordinate Distribution;X (cm);Y (cm)", 100, -100, 100, 100, -100, 100);
   TH2D *hYZ = new TH2D("hYZ", "YZ Coordinate Distribution;Y (cm);Z (cm)", 100, -100, 100, 100, -100, 100);
   TH2D *hZX = new TH2D("hZX", "ZX Coordinate Distribution;Z (cm);X (cm)", 100, -100, 100, 100, -100, 100);

    // Define a 3D histogram for xyz coordinates
    TH3D *hXYZ = new TH3D("hXYZ", "XYZ Coordinate Distribution;X (cm);Y (cm);Z (cm)", 
                          50, -100, 100, 50, -100, 100, 50, -100, 100);

   // Define a new histogram for reconstructed data
   TH1D *hReconstructedDistance = new TH1D("hReconstructedDistance", "Reconstructed Distance;Distance (cm);Entries", 100, 0, 100);                          

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      // Fill 1D histograms with x, y, and z coordinates
      hXcor->Fill(fXcor);
      hYcor->Fill(fYcor);
      hZcor->Fill(fZcor);

      // Fill 2D histograms with xy, yz, and zx coordinates
      hXY->Fill(fXcor, fYcor);
      hYZ->Fill(fYcor, fZcor);
      hZX->Fill(fZcor, fXcor);

      // Fill 3D histogram with xyz coordinates
      hXYZ->Fill(fXcor, fYcor, fZcor);

      // Reconstruction: Combine the raw coordinates into a reconstructed position or energy
      // Example: Calculate the distance from the origin (0, 0, 0)
      Double_t distance = sqrt(fXcor * fXcor + fYcor * fYcor + fZcor * fZcor);
      hReconstructedDistance->Fill(distance);


      // cout<<"Its working"<<endl;
      // cout << "X Cor: " << fXcor << ", Y Cor: " << fYcor << ", Z Cor: " << fZcor << endl;
      // if (Cut(ientry) < 0) continue;
   }

   // Create canvas and draw histogram
   TCanvas *cXY = new TCanvas("cXY", "XY Histogram with Intensity Bar", 800, 600);
   gStyle->SetPalette(kRainBow);
   gStyle->SetPadRightMargin(0.15);
   cXY->cd();
   hXY->Draw("COLZ");
   cXY->Update();

   // Repeat for YZ
   TCanvas *cYZ = new TCanvas("cYZ", "YZ Histogram with Intensity Bar", 800, 600);
   cYZ->cd();
   hYZ->Draw("COLZ");
   cYZ->Update();

   // Repeat for ZX
   TCanvas *cZX = new TCanvas("cZX", "ZX Histogram with Intensity Bar", 800, 600);
   cZX->cd();
   hZX->Draw("COLZ");
   cZX->Update();

   // Write histograms to a ROOT file
   TFile *outputFile = new TFile("xyz_cor_reconstructed.root", "RECREATE");

   // Write 1D histograms
   hXcor->Write();
   hYcor->Write();
   hZcor->Write();

   // Write 2D histograms
   hXY->Write();
   hYZ->Write();
   hZX->Write();

   // Write reconstructed histograms
   // hReconstructedEnergy->Write();
   hReconstructedDistance->Write();

   // Write canvases (with color bars and style)
   cXY->Write();
   cYZ->Write();
   cZX->Write();

   // Create and draw 3D histogram canvas
   TCanvas *cXYZ = new TCanvas("cXYZ", "XYZ 3D Histogram", 800, 600);
   cXYZ->cd();
   gStyle->SetPalette(kRainBow);
   gStyle->SetCanvasPreferGL(true); // Optional: use OpenGL viewer for better 3D

   hXYZ->Draw("LEGO2"); // or "BOX" or "SURF1" depending on style preference
   cXYZ->Update();

   // Save 3D histogram and canvas to file
   hXYZ->Write();
   cXYZ->Write();

   outputFile->Close();
}
