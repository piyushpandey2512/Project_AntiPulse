void viewReconsHist() {
    TFile* f = TFile::Open("ReconstructedVertices.root");
    if (!f || f->IsZombie()) {
        std::cout << "Error opening ROOT file\n";
        return;
    }

    TH2D* h2 = (TH2D*)f->Get("h2_yz");
    if (!h2) {
        std::cout << "Histogram not found\n";
        return;
    }

    TCanvas* c = new TCanvas("c", "Histogram with grid and palette", 800, 600);
    gStyle->SetPalette(kBird); // nice palette
    c->SetGridx();
    c->SetGridy();

    h2->Draw("COLZ");
    c->Update();
}
