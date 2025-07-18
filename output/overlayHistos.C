void overlayHistos() {
    TFile *f1 = TFile::Open("Project_AntiPulse_20250703_131704_1Cr_FullModule__OriginalWidth.root");
    TFile *f2 = TFile::Open("Project_AntiPulse_20250703_145434_1Cr_FullModule__DoubleWidth.root");
    TFile *f3 = TFile::Open("Project_AntiPulse_20250703_155052_1Cr_FullModule__HalfWidth.root");

    TH1 *h1 = (TH1*)f1->Get("PionEnergyDep");
    TH1 *h2 = (TH1*)f2->Get("PionEnergyDep");
    TH1 *h3 = (TH1*)f3->Get("PionEnergyDep");

    if (!h1 || !h2 || !h3) {
        std::cerr << "One or more histograms not found!" << std::endl;
        return;
    }

    // Normalize all histograms to area = 1 (optional)
    h1 = (TH1*)h1->Clone("h1_clone"); h1->Scale(1.0 / h1->Integral());
    h2 = (TH1*)h2->Clone("h2_clone"); h2->Scale(1.0 / h2->Integral());
    h3 = (TH1*)h3->Clone("h3_clone"); h3->Scale(1.0 / h3->Integral());

    // Set line styles
    h1->SetLineColor(kRed);    h1->SetLineWidth(2);
    h2->SetLineColor(kBlue);   h2->SetLineWidth(2);
    h3->SetLineColor(kGreen);  h3->SetLineWidth(2);

    // Get global axis limits
    double maxY = std::max({h1->GetMaximum(), h2->GetMaximum(), h3->GetMaximum()});

    // Set titles and draw
    TCanvas *c = new TCanvas("c", "Overlayed Histos", 800, 600);
    h1->SetTitle("PionEnergyDep (Normalized);Energy;Probability");
    h1->SetMaximum(maxY * 1.2);  // leave headroom
    h1->Draw("HIST");
    h2->Draw("HIST SAME");
    h3->Draw("HIST SAME");

    // Add legend
    TLegend *leg = new TLegend(0.65, 0.7, 0.88, 0.88, "Width Variants");
    leg->AddEntry(h1, "OriginalWidth", "l");
    leg->AddEntry(h2, "DoubleWidth", "l");
    leg->AddEntry(h3, "HalfWidth", "l");
    leg->Draw();

    // Open the ROOT browser for further edits
    new TBrowser();
}
