#include <TFile.h>
#include <TH3D.h>
#include <TH2D.h> 
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>

void viewReconsHist() {
    TFile* f = TFile::Open("../build/Project_AntiPulse_20250825_055254.root");
    if (!f || f->IsZombie()) {
        std::cout << "Error opening ROOT file\n";
        return;
    }

    TH3D* h3 = (TH3D*)f->Get("SourceXYZDistribution");
    if (!h3) {
        std::cout << "Histogram not found\n";
        return;
    }

    // Project to XY plane
    TH2D* h2_xy = (TH2D*)h3->Project3D("xy");

    TCanvas* c = new TCanvas("c", "XY Projection with grid and palette", 800, 600);
    gStyle->SetPalette(kBird); // nice palette
    c->SetGridx();
    c->SetGridy();

    h2_xy->Draw("COLZ");
    c->Update();
}