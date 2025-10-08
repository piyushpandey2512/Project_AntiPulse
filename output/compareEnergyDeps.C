void compareEnergyDeps() {
    // Load your specific ROOT files


    // TFile *f_orig = TFile::Open("/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/output/scintillator_testing/Project_AntiPulse_20250924_143023_SS_OW.root");
    // TFile *f_double = TFile::Open("/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/output/scintillator_testing/Project_AntiPulse_20250924_140858_SS_DW.root");
    // TFile *f_half = TFile::Open("/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/output/scintillator_testing/Project_AntiPulse_20250924_141919_SS_HW.root");

    // TFile *f_orig = TFile::Open("Project_AntiPulse_20250703_131704_1Cr_1FullModule__OriginalWidth.root");
    // TFile *f_double = TFile::Open("Project_AntiPulse_20250703_145434_1Cr_1FullModule__DoubleWidth.root");
    // TFile *f_half = TFile::Open("Project_AntiPulse_20250703_155052_1Cr_1FullModule__HalfWidth.root");

    // TFile *f_orig = TFile::Open("Project_AntiPulse_20250710_180914_50lac_2B2B_scin_Original_Width.root");
    // TFile *f_double = TFile::Open("Project_AntiPulse_20250731_153207_50lac_2B2B_scin_Double_width.root");
    // TFile *f_half = TFile::Open("Project_AntiPulse_20250731_154434_50lac_2B2B_scin_Half_width.root");

    // TFile *f_orig = TFile::Open("Project_AntiPulse_20250731_165536_50lac_Two_B2B_modules_Original_width.root");
    // TFile *f_double = TFile::Open("Project_AntiPulse_20250731_171314_50lac_Two_B2B_modules_Double_width.root");
    // TFile *f_half = TFile::Open("Project_AntiPulse_20250731_174323_50lac_Two_B2B_modules_Half_width.root");

    TFile *f_orig = TFile::Open("/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/output/scintillator_testing/Project_AntiPulse_20250921_194247_FM_OW.root");
    TFile *f_double = TFile::Open("/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/output/scintillator_testing/Project_AntiPulse_20250921_195224_FM_DW.root");
    TFile *f_half = TFile::Open("/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/output/scintillator_testing/Project_AntiPulse_20250921_200503_FM_HW.root");



    // Get the histograms
    TH1 *h_orig = (TH1*)f_orig->Get("PionEnergyDep");
    TH1 *h_double = (TH1*)f_double->Get("PionEnergyDep");
    TH1 *h_half = (TH1*)f_half->Get("PionEnergyDep");

    // Clone and normalize (optional, but helpful for shape comparison)
    h_orig = (TH1*)h_orig->Clone("h_orig_clone"); h_orig->Scale(1.0 / h_orig->Integral());
    h_double = (TH1*)h_double->Clone("h_double_clone"); h_double->Scale(1.0 / h_double->Integral());
    h_half = (TH1*)h_half->Clone("h_half_clone"); h_half->Scale(1.0 / h_half->Integral());

    // Styling
    h_orig->SetLineColor(kRed);
    h_double->SetLineColor(kBlue);
    h_half->SetLineColor(kGreen);

    h_orig->SetLineWidth(5);
    h_double->SetLineWidth(5);
    h_half->SetLineWidth(5);

    h_orig->SetTitle("PionEnergyDep (Normalized) for Single Scintillator;Deposited Energy (MeV);Normalized Counts");
    // h_orig->SetTitle("PionEnergyDep For 1FullModule (Normalized);Deposited Energy (MeV);Normalized Counts");
    // h_orig->SetTitle("PionEnergyDep For 2B2B Scintillator (Normalized);Deposited Energy (MeV);Normalized Counts"); 
    // h_orig->SetTitle("PionEnergyDep For 2B2B Modules (Normalized);Deposited Energy (MeV);Normalized Counts");
    // h_orig->SetTitle("PionEnergyDep (Normalized) for Four Modules ;Deposited Energy (MeV);Normalized Counts");


    // Compute global Y max for scaling
    double maxY = std::max({h_orig->GetMaximum(), h_double->GetMaximum(), h_half->GetMaximum()});
    h_orig->SetMaximum(maxY * 1.2);  // 20% headroom

    // Compute global X range
    double minX = std::min({h_orig->GetXaxis()->GetXmin(), h_double->GetXaxis()->GetXmin(), h_half->GetXaxis()->GetXmin()});
    double maxX = std::max({h_orig->GetXaxis()->GetXmax(), h_double->GetXaxis()->GetXmax(), h_half->GetXaxis()->GetXmax()});
    h_orig->GetXaxis()->SetLimits(minX, maxX);

    // Draw everything
    TCanvas *c = new TCanvas("c", "Pion Energy Deposition - All Widths", 1000, 600);
    c->SetTickx();
    c->SetTicky();
    c->SetGridx();
    c->SetGridy();
    h_orig->Draw("HIST");
    h_double->Draw("HIST SAME");
    h_half->Draw("HIST SAME");

    // Legend
    TLegend *leg = new TLegend(0.65, 0.7, 0.88, 0.88);
    leg->AddEntry(h_orig, "OriginalWidth (2.4 cm)", "l");
    leg->AddEntry(h_double, "DoubleWidth (4.8 cm)", "l");
    leg->AddEntry(h_half, "HalfWidth (1.2 cm)", "l");
    leg->Draw();

    // TLine *l1 = new TLine(2.254, 0, 2.254, 0.139);
    // l1->SetLineColor(kBlack);
    // l1->SetLineWidth(2);
    // l1->Draw();


    // double x0 = 0.45; // X coordinate for the arrow start
    // int bin = h_orig->FindBin(x0);
    // double y0 = h_orig->GetBinContent(bin);

    // TArrow *arr = new TArrow(1.0, 0.045, x0, y0);
    // arr->SetLineWidth(2);
    // arr->SetArrowSize(0.01);
    // arr->SetLineColor(kBlack);
    // arr->SetFillColor(kBlack);
    // arr->Draw();

    
    // TLatex *t = new TLatex(1.0, 0.045, "Pions as MIPs");
    // t->Draw();


    // Draw three arrows and annotate with LaTeX text

    // Arrow 1
    TArrow *arr1 = new TArrow(4.54, 0.04, 5.0, 0.04, 0.02, "<|");
    arr1->SetLineWidth(2);
    arr1->SetArrowSize(0.02);
    arr1->SetLineColor(kBlack);
    arr1->SetFillColor(kBlack);
    arr1->Draw();
    TLatex *t1 = new TLatex(5.05, 0.04, "4.54 MeV");
    t1->SetTextSize(0.03);
    t1->Draw();

    // Arrow 2
    TArrow *arr2 = new TArrow(9.25, 0.005, 9.7, 0.005, 0.02, "<|");
    arr2->SetLineWidth(2);
    arr2->SetArrowSize(0.02);
    arr2->SetLineColor(kBlack);
    arr2->SetFillColor(kBlack);
    arr2->Draw();
    TLatex *t2 = new TLatex(9.75, 0.005, "9.25 MeV");
    t2->SetTextSize(0.03);
    t2->Draw();

    // Arrow 3
    double x_arrow = 2.34;
    int bin = h_orig->FindBin(x_arrow);
    double y_arrow = h_orig->GetBinContent(bin);

    // Add a small offset if you want the arrow above the bin
    TArrow *arr3 = new TArrow(x_arrow, y_arrow, x_arrow + 0.45, y_arrow, 0.02, "<|");
    arr3->SetLineWidth(2);
    arr3->SetArrowSize(0.02);
    arr3->SetLineColor(kBlack);
    arr3->SetFillColor(kBlack);
    arr3->Draw();

    TLatex *t3 = new TLatex(x_arrow + 0.5, y_arrow, "2.25 MeV");
    t3->SetTextSize(0.03);
    t3->Draw();

    // Launch ROOT browser to allow editing
    new TBrowser();
}
