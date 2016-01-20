void LookAtTrigBits(TString setname="088a")
{
  TString filename = "redset/RedOutputset"+setname+".root";
  TFile * infile = new TFile(filename.Data(),"READ");
  TH1D * dist = new TH1D("dist","FP201 output bits",16,0,16);
  TTree * tr = (TTree*) infile->Get("str");
  Int_t TrigBits;
  tr->SetBranchAddress("TrigBits",&TrigBits);

  dist->GetXaxis()->SetBinLabel(0+1,"HT_th0");
  dist->GetXaxis()->SetBinLabel(1+1,"HT_th1");
  dist->GetXaxis()->SetBinLabel(2+1,"SmBS_th0");
  dist->GetXaxis()->SetBinLabel(3+1,"SmBS_th1");
  dist->GetXaxis()->SetBinLabel(4+1,"SmBS_th2");
  dist->GetXaxis()->SetBinLabel(5+1,"LgBS_th0");
  dist->GetXaxis()->SetBinLabel(6+1,"LgBS_th1");
  dist->GetXaxis()->SetBinLabel(7+1,"LgBS_th2");
  dist->GetXaxis()->SetBinLabel(8+1,"JP_th0");
  dist->GetXaxis()->SetBinLabel(9+1,"JP_th1");
  dist->GetXaxis()->SetBinLabel(10+1,"JP_th2");
  dist->GetXaxis()->SetBinLabel(11+1,"dijet");
  dist->GetXaxis()->SetBinLabel(12+1,"unused1");
  dist->GetXaxis()->SetBinLabel(13+1,"unused2");
  dist->GetXaxis()->SetBinLabel(14+1,"FPE");
  dist->GetXaxis()->SetBinLabel(15+1,"unused3");

  Int_t ent = tr->GetEntries();
  //ent = 100000;
  for(int x=0; x<ent; x++)
  {
    if(x%1000==0) printf("%.2f%%\n",(Float_t)x/ent*100);
    tr->GetEntry(x);
    for(int t=0; t<16; t++)
    {
      if((TrigBits>>t)&0x1) dist->Fill(t);
    };
  };

  TCanvas * canv = new TCanvas("canv","canv",1000,1000);
  dist->Draw();
};



