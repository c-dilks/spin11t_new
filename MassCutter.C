// determines $CUT_TYPE-dependent mass cuts 

void MassCutter()
{
  // environment
  gSystem->Load("src/RunInfo.so");
  //LevelTwo * T = new LevelTwo();
  Environ * env = new Environ();

  // check mass cut type
  Int_t multiplier;
  if(!strcmp(env->MassCutType,"en")) multiplier=10;
  else if(!strcmp(env->MassCutType,"pt")) multiplier=1;
  else
  {
    fprintf(stderr,"ERROR: env->MassCutType must be \"en\" or \"pt\"\n");
    return;
  };


  // load mass dists for each en bin
  TFile * infile = new TFile("diagset/all.root","READ");
  TObjArray * mdist_arr[10];
  TH1D  * mdist[10];
  char mdist_arr_n[10][64];
  for(Int_t ii=0; ii<10; ii++)
  {
    sprintf(mdist_arr_n[ii],"mass_dist_for_%sbin_%d_arr",env->MassCutType,ii);
    mdist_arr[ii] = (TObjArray*) infile->Get(mdist_arr_n[ii]);
    printf("%s %p\n",mdist_arr_n[ii],(void*)mdist_arr[ii]);
    mdist[ii] = (TH1D*) mdist_arr[ii]->At(0/*T->Index(env->TriggerType)*/);
    mdist[ii]->GetXaxis()->SetLabelSize(0.06);
    mdist[ii]->GetYaxis()->SetLabelSize(0.06);
    mdist[ii]->SetLineWidth(2);
  };

  // find bin with maximum bin and determine upper and lower bounds 
  // of energy-dependent mass cut by setting the bounds in the distribution
  // where the mass is reduced from the maximum by a factor of "factor"
  Double_t mdist_max[10];
  Double_t mdist_max_mass[10];
  Int_t mdist_max_bin[10];
  Int_t mdist_nbins[10];
  Double_t bc;
  Double_t factor=0.2; // "factor"
  Double_t ub[10];
  Double_t lb[10];
  Bool_t found;
  for(Int_t ii=0; ii<10; ii++)
  {
    if(mdist[ii]->GetEntries()>0)
    {
      mdist_max[ii] = mdist[ii]->GetMaximum();
      mdist_max_bin[ii] = mdist[ii]->GetMaximumBin();
      mdist_max_mass[ii] = mdist[ii]->GetBinCenter(mdist_max_bin[ii]);
      mdist_nbins[ii] = mdist[ii]->GetNbinsX();
      
      // cheap hack to get ignore mass spikes (hot towers)
      // -- DEPRECATED since remaining hot towers killed by
      //    hard low pT cutoff!
      // -- remaining hot towers are all for masses > 0.3
      // -- pi0 peak really never goes > 0.3
      // -- thus if we find a maximum with mass > 0.3, we simply set the 
      //    bin contents to 0... this is not a nice thing to do, but I niided 
      //    to write this script quickly; TO BE UPDATED SOMEDAY!
      /*
      if(mdist_max_mass[ii]>0.3)
      {
        while(mdist_max_mass[ii]>0.3)
        {
          mdist[ii]->SetBinContent(mdist_max_bin[ii],0);
          mdist_max[ii] = mdist[ii]->GetMaximum();
          mdist_max_bin[ii] = mdist[ii]->GetMaximumBin();
          mdist_max_mass[ii] = mdist[ii]->GetBinCenter(mdist_max_bin[ii]);
        };
        mdist[ii] = (TH1D*) infile->Get(mdist_n[ii]);
      };
      */


      // search for upper bound
      found=false;
      for(Int_t b=mdist_max_bin[ii]; b<=mdist_nbins[ii]; b++)
      {
        bc = mdist[ii]->GetBinContent(b);
        if(bc<factor*mdist_max[ii] && found==false)
        {
          ub[ii] = mdist[ii]->GetBinCenter(b);
          found=true;
        };
      };

      // search for lower bound
      found=false;
      for(Int_t b=mdist_max_bin[ii]; b>=1; b--)
      {
        bc = mdist[ii]->GetBinContent(b);
        if(bc<factor*mdist_max[ii] && found==false)
        {
          lb[ii] = mdist[ii]->GetBinCenter(b);
          found=true;
        };
      };
    };
  };


  // define TLines and draw TCanvases
  TLine * max_line[10];
  TLine * lb_line[10];
  TLine * ub_line[10];
  TCanvas * cc = new TCanvas("cc","cc",700,500);
  Int_t cnt=0;
  gROOT->ProcessLine(".! touch mass_cuts.dat; rm mass_cuts.dat; touch mass_cuts.dat");
  printf("%s_low %s_high mass_low mass_at_max_bin mass_high\n",env->MassCutType,env->MassCutType);
  for(Int_t ii=0; ii<10; ii++)
  {
    if(mdist[ii]->GetEntries()>0)
    {
      cc->Clear();
      max_line[ii] = new TLine(mdist_max_mass[ii],0,mdist_max_mass[ii],mdist_max[ii]);
      lb_line[ii] = new TLine(lb[ii],0,lb[ii],mdist_max[ii]);
      ub_line[ii] = new TLine(ub[ii],0,ub[ii],mdist_max[ii]);
      max_line[ii]->SetLineWidth(2.5);
      lb_line[ii]->SetLineWidth(2.5);
      ub_line[ii]->SetLineWidth(2.5);
      max_line[ii]->SetLineColor(kRed);
      lb_line[ii]->SetLineColor(kBlue);
      ub_line[ii]->SetLineColor(kGreen+2);
      mdist[ii]->Draw();
      max_line[ii]->Draw();
      lb_line[ii]->Draw();
      ub_line[ii]->Draw();
      gSystem->RedirectOutput("mass_cuts.dat","a");
      printf("%f %f %f %f %f\n",ii*multiplier,(ii+1)*multiplier,lb[ii],mdist_max_mass[ii],ub[ii]);
      gSystem->RedirectOutput(0);
      //printf("%f %f %f %f %f\n",ii*multiplier,(ii+1)*multiplier,lb[ii],mdist_max_mass[ii],ub[ii]);
      if(cnt==0) cc->Print("mass_cuts.pdf(","pdf");
      else cc->Print("mass_cuts.pdf","pdf");
    };
  };
  cc->Clear(); cc->Print("mass_cuts.pdf)","pdf");
  system("cat mass_cuts.dat");
};
