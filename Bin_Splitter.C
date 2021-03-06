// defines binning that is needed in the analysis
//
// all the binning is controlled by the variables in the section CONTROL VARIABLES
// -- low = lower bound
// -- high = upper bound
// -- bins = number of bins 
//    -- if bins=0, then a manually defined binning scheme is used (defined in the code
//       below)
//
// -- these bins do not define the final kinematic cuts used for the jet types; the 
//    final kinematic boundaries are set in PhiDists3.C (and additionally Diagnostics.C)

void Bin_Splitter(Int_t phi_bins = 10,
                  Int_t eta_bins = 1,
                  Int_t pt_bins = 0,
                  Int_t en_bins = 0,
                  Int_t STG1_in = 2,
                  Int_t STG2_in = 1,
                  Int_t MIPN_in = 0,
                  Int_t USE_TCU_BITS_in = 0 
)
{
  // CONTROL VARIABLES

  // trigger
  char trigger[16]; strcpy(trigger,"All");

  // en or pt dependent mass cuts
  char cut_type[16]; strcpy(cut_type,"en");

  // RP selecction
  // use "N" for no restriction
  char RP_select[16]; strcpy(RP_select,"N");


  Int_t STG1,STG2,MIPN,USE_TCU_BITS;
  STG1 = STG1_in;
  STG2 = STG2_in;
  MIPN = MIPN_in;
  USE_TCU_BITS = USE_TCU_BITS_in;




  Double_t phi_low=-3.15;
  Double_t phi_high=3.15;


  /////////////////////////////
  Int_t whichEtaCut = 2; // 0-full, 1-large_cells, 2-small_cells
  /////////////////////////////
  Double_t eta_border = 3.1; // determined 10.08.17

  Double_t eta_low=2.65; // see one_bin.root for full eta distribution
  Double_t eta_high=3.9;
  Int_t eta_bins=1;

  switch(whichEtaCut)
  {
    case 1:
      eta_high=eta_border;
      break;
    case 2:
      eta_low=eta_border;
      break;
  };
  /////////////////////////////



  Double_t pt_low=2.0;   // lowest of the longitudinal low cutoff (NEED TO CHECK pT dists)
  Double_t pt_high=10.0; // pi0 reconstruction is unreliable for pT>15

  Double_t en_low=30; // E>30 cutoff point for pi0s; should allow sph & thr to go lower
  Double_t en_high=100; // can go up to 255, but cluster merging for pi0s is problem for E>100

  char efile[64];
  strcpy(efile,"env_bins.sh");

  gSystem->RedirectOutput(efile,"w");
  printf("#!/bin/bash\n");
  printf("# This file was generated by Bin_Splitter.C\n");
  printf("# ** source this file while in this directory\n\n");
  printf("export SPINDIR=$(pwd)\n");


  // phi
  printf("\nexport PHI_LOW=%f\n",phi_low);
  printf("export PHI_HIGH=%f\n",phi_high);
  Double_t phi_width;
  if(phi_bins>0)
  {
    printf("export PHI_BINS=%d\n",phi_bins);
    phi_width = (phi_high-phi_low)/phi_bins;
    for(Int_t i=0; i<=phi_bins; i++) printf("export PHI_DIV_%d=%f\n",i,phi_low+i*phi_width);
  }
  else
  {
    phi_bins=1;
    printf("export PHI_BINS=%d\n",phi_bins);
    printf("export PHI_DIV_0=%f\n",phi_low);
    printf("export PHI_DIV_1=%f\n",phi_high);
  };


  // eta
  printf("\nexport ETA_LOW=%f\n",eta_low);
  printf("export ETA_HIGH=%f\n",eta_high);
  Double_t eta_width;
  if(eta_bins>0)
  {
    printf("export ETA_BINS=%d\n",eta_bins);
    eta_width = (eta_high-eta_low)/eta_bins;
    for(Int_t i=0; i<=eta_bins; i++) printf("export ETA_DIV_%d=%f\n",i,eta_low+i*eta_width);
  }
  else
  {
    eta_bins=1;
    printf("export ETA_BINS=%d\n",eta_bins);
    printf("export ETA_DIV_0=%f\n",eta_low);
    printf("export ETA_DIV_1=%f\n",eta_high);
  };


  // pt
  printf("\nexport PT_LOW=%f\n",pt_low);
  printf("export PT_HIGH=%f\n",pt_high);
  Double_t pt_width;
  if(pt_bins>0)
  {
    printf("export PT_BINS=%d\n",pt_bins);
    pt_width = (pt_high-pt_low)/pt_bins;
    for(Int_t i=0; i<=pt_bins; i++) printf("export PT_DIV_%d=%f\n",i,pt_low+i*pt_width);
  }
  else
  {
    if(whichEtaCut==0 || whichEtaCut==1)
    {
      /*
      pt_bins=5;
      printf("export PT_BINS=%d\n",pt_bins);
      printf("export PT_DIV_0=%f\n",pt_low);
      printf("export PT_DIV_1=%f\n",3);
      printf("export PT_DIV_2=%f\n",4);
      printf("export PT_DIV_3=%f\n",5);
      printf("export PT_DIV_4=%f\n",6.5);
      printf("export PT_DIV_5=%f\n",pt_high);
      */
      pt_bins=4;
      printf("export PT_BINS=%d\n",pt_bins);
      printf("export PT_DIV_0=%f\n",pt_low);
      printf("export PT_DIV_1=%f\n",3.75);
      printf("export PT_DIV_2=%f\n",5.0);
      printf("export PT_DIV_3=%f\n",6.5);
      printf("export PT_DIV_4=%f\n",pt_high);
    }
    else if(whichEtaCut==2)
    {
      /*
      pt_bins=3;
      printf("export PT_BINS=%d\n",pt_bins);
      printf("export PT_DIV_0=%f\n",pt_low);
      printf("export PT_DIV_1=%f\n",3);
      printf("export PT_DIV_2=%f\n",4);
      printf("export PT_DIV_3=%f\n",pt_high);
      */
      pt_bins=4;
      printf("export PT_BINS=%d\n",pt_bins);
      printf("export PT_DIV_0=%f\n",pt_low);
      printf("export PT_DIV_1=%f\n",3.4);
      printf("export PT_DIV_2=%f\n",4.4);
      printf("export PT_DIV_3=%f\n",5.3);
      printf("export PT_DIV_4=%f\n",pt_high);
    };
  };

    
  // en
  if(whichEtaCut==0 || whichEtaCut==1) en_high=70;
  else if(whichEtaCut==2) en_high=100;
  printf("\nexport EN_LOW=%f\n",en_low);
  printf("export EN_HIGH=%f\n",en_high);
  Double_t en_width;
  if(en_bins>0)
  {
    printf("export EN_BINS=%d\n",en_bins);
    en_width = (en_high-en_low)/en_bins;
    for(Int_t i=0; i<=en_bins; i++) printf("export EN_DIV_%d=%f\n",i,en_low+i*en_width);
  }
  else if(en_bins==0)
  {
    /*
    // for pt-dependnce for E<100 & E>100
    en_bins=2;
    printf("export EN_BINS=%d\n",en_bins);
    printf("export EN_DIV_0=%f\n",en_low);
    printf("export EN_DIV_1=%f\n",100);
    printf("export EN_DIV_2=%f\n",en_high);
    ///*
    // for pt_bins=1 and eta_bins=1
    en_bins=6;
    printf("export EN_BINS=%d\n",en_bins);
    printf("export EN_DIV_0=%f\n",en_low);
    printf("export EN_DIV_1=%f\n",25);
    printf("export EN_DIV_2=%f\n",50);
    printf("export EN_DIV_3=%f\n",75);
    printf("export EN_DIV_4=%f\n",100);
    printf("export EN_DIV_5=%f\n",150);
    printf("export EN_DIV_6=%f\n",en_high);
    */
    /*
    en_bins=7;
    printf("export EN_BINS=%d\n",en_bins);
    printf("export EN_DIV_0=%f\n",en_low);
    printf("export EN_DIV_1=%f\n",20);
    printf("export EN_DIV_2=%f\n",28);
    printf("export EN_DIV_3=%f\n",36);
    printf("export EN_DIV_4=%f\n",50);
    printf("export EN_DIV_5=%f\n",65);
    printf("export EN_DIV_6=%f\n",80);
    printf("export EN_DIV_7=%f\n",en_high);
    */
    en_bins=1;
    printf("export EN_BINS=%d\n",en_bins);
    printf("export EN_DIV_0=%f\n",en_low);
    printf("export EN_DIV_1=%f\n",en_high);
  }
  else if(en_bins==-1)
  {
    // matching heppels xF bins for run15 prelim
    en_bins=6;
    printf("export EN_BINS=%d\n",en_bins);
    printf("export EN_DIV_0=%f\n",15);
    printf("export EN_DIV_1=%f\n",25);
    printf("export EN_DIV_2=%f\n",35);
    printf("export EN_DIV_3=%f\n",45);
    printf("export EN_DIV_4=%f\n",55);
    printf("export EN_DIV_5=%f\n",65);
    printf("export EN_DIV_6=%f\n",75);
  };

  printf("\nexport TRIGGER_TYPE=\"%s\"\n",trigger);
  printf("export MASS_CUT_TYPE=\"%s\"\n",cut_type);
  printf("export RP_SELECT=\"%s\"\n",RP_select);
  printf("\n");
  printf("export STG1=%d\n",STG1);
  printf("export STG2=%d\n",STG2);
  printf("export MIPN=%d\n",MIPN);
  printf("export USE_TCU_BITS=%d\n",USE_TCU_BITS);
  
  gSystem->RedirectOutput(0);
  char cat[128];
  sprintf(cat,".! cat %s",efile);
  printf("\n");
  gROOT->ProcessLine(cat);
  printf("\n%s built; source it by executing\n. env_bins.sh\n",efile);
};
