void compare_primary_reco() {
    // Load primary counts from ROOT file
    TFile *f = TFile::Open("Project_AntiPulse_output.root"); // Change to your output file
    TH1 *h_primary = (TH1*)f->Get("PrimaryCount");

    // Load reconstructed counts from text file
    std::ifstream fin("RecoCountPerEvent.txt");
    std::map<int, int> recoCounts;
    int evt, count;
    while (fin >> evt >> count) recoCounts[evt] = count;

    // Make histogram for reconstructed counts
    TH1I *h_reco = new TH1I("h_reco", "Reconstructed Vertices per Event", h_primary->GetNbinsX(), h_primary->GetXaxis()->GetXmin(), h_primary->GetXaxis()->GetXmax());
    for (const auto& [evt, n] : recoCounts) h_reco->Fill(n);

    h_primary->SetLineColor(kBlue);
    h_reco->SetLineColor(kRed);

    h_primary->Draw();
    h_reco->Draw("SAME");

    TLegend *leg = new TLegend(0.7,0.7,0.9,0.9);
    leg->AddEntry(h_primary, "Primaries", "l");
    leg->AddEntry(h_reco, "Reconstructed", "l");
    leg->Draw();
}