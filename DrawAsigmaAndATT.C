void DrawAsigmaAndATT(TString dir="output")
{
  TString filename="asym_plots/"+dir+"/spin_pi0.root";
  TFile * infile = new TFile(filename.Data(),"READ");

  infile->cd("A_Sigma");
  pt_dep_z0_a3_g0_e0->SetLineColor(kRed);
  pt_dep_z0_a3_g0_e0->SetLineWidth(3);
  pt_dep_z0_a3_g0_e0->SetMarkerStyle(kFullCircle);
  pt_dep_z0_a3_g0_e0->SetMarkerColor(kBlack);
  pt_dep_z0_a3_g0_e0->SetMarkerSize(1.5);
  TCanvas * canv_A_sigma = new TCanvas("canv_A_sigma","canv_A_sigma",1000,800);
  canv_A_sigma->SetGrid(1,1);
  gStyle->SetOptFit(1);
  pt_dep_z0_a3_g0_e0->Fit("pol0","","",3,10);
  pt_dep_z0_a3_g0_e0->Draw("APE");

  infile->cd("A_TT");
  pt_dep_z1_a3_g0_e0->SetLineColor(kRed);
  pt_dep_z1_a3_g0_e0->SetLineWidth(3);
  pt_dep_z1_a3_g0_e0->SetMarkerStyle(kFullCircle);
  pt_dep_z1_a3_g0_e0->SetMarkerColor(kBlack);
  pt_dep_z1_a3_g0_e0->SetMarkerSize(1.5);
  TCanvas * canv_A_TT = new TCanvas("canv_A_TT","canv_A_TT",1000,800);
  canv_A_TT->SetGrid(1,1);
  gStyle->SetOptFit(1);
  pt_dep_z1_a3_g0_e0->Fit("pol0","","",3,10);
  pt_dep_z1_a3_g0_e0->Draw("APE");
};


