// -- create phi distributions with various kinematic cuts (eta,pt,E) for each spinbit
//    (kinematic cuts are set as Double_t's below)
// -- phi distributions for "+ -" and "- +" are weighted by relative luminosity
//    A_LL = 1/(P_b*P_y)*(N++ + N-- - RN+- -RN-+)/(N++ + N-- + RN+- + RN-+)
// -- phi distributions are written to phiset/ directory with similar name; 
//    they are named: phi_s[spinbit]_g[eta bin]_p[pt bin]_e[en bin]

void PhiDists4(const char * filename="RedOutputset088a.root",Bool_t debug=false)
{
  if(debug) system("rm debug/debug.dat");
  enum ew_enum {kE,kW};
  enum ud_enum {kU,kD};
  enum io_enum {kI,kO};
  enum ns_enum {kN,kS};

  // load polarization and rellum data
  gSystem->Load("src/RunInfo.so");
  RunInfo * RD = new RunInfo();
  //LevelTwo * T = new LevelTwo();
  Environ * env = new Environ();
  EventClass * ev = new EventClass();
  /*
  TriggerBoolean * trg_bool = 
    new TriggerBoolean(env->STG1,env->STG2,env->MIPN,env->USE_TCU_BITS);
  trg_bool->PrintParameters();
  BBCtiles * bbc = new BBCtiles();
  */

  // get bins from environment
  Int_t phi_bins0 = env->PhiBins; const Int_t phi_bins = phi_bins0;
  Int_t eta_bins0 = env->EtaBins; const Int_t eta_bins = eta_bins0;
  Int_t en_bins0 = env->EnBins; const Int_t en_bins = en_bins0;
  Int_t pt_bins0 = env->PtBins; const Int_t pt_bins = pt_bins0;

  // get number of event classes
  Int_t N_CLASS_tmp = ev->N;
  const Int_t N_CLASS = N_CLASS_tmp;


  // read redset tree and set output file name
  Int_t runnum,bx,blue,yell,pattern,TrigBits;
  Float_t M12,N12,E12,Z,Phi,Eta,Pt,b_pol,y_pol;
  Bool_t kicked,isConsistent,RP_nonzero;
  UInt_t L2sum[2];
  UInt_t lastdsm[8];
  char setname[32];
  sscanf(filename,"RedOutputset%s",setname);
  sprintf(filename,"redset/%s",filename);
  TFile * infile = new TFile(filename,"READ");
  char outname[256];
  sprintf(outname,"phiset/phi%s",setname);
  TTree * tree = (TTree*) infile->Get("str");
  tree->SetBranchAddress("runnum",&runnum);
  tree->SetBranchAddress("Bunchid7bit",&bx);
  tree->SetBranchAddress("M12",&M12);
  tree->SetBranchAddress("N12",&N12);
  tree->SetBranchAddress("E12",&E12);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("Phi",&Phi);
  tree->SetBranchAddress("Eta",&Eta);
  tree->SetBranchAddress("Pt",&Pt);
  tree->SetBranchAddress("TrigBits",&TrigBits);
  //tree->SetBranchAddress("L2sum",L2sum);
  //tree->SetBranchAddress("lastdsm",trg_bool->TCU->lastdsm);

  /*
  tree->SetBranchAddress("RPE_QTN",&(trg_bool->RPSCI->N[kE]));
  tree->SetBranchAddress("RPW_QTN",&(trg_bool->RPSCI->N[kW]));
  tree->SetBranchAddress("RPE_Idx",trg_bool->RPSCI->Idx[kE]);
  tree->SetBranchAddress("RPE_TAC",trg_bool->RPSCI->TAC[kE]);
  tree->SetBranchAddress("RPE_ADC",trg_bool->RPSCI->ADC[kE]);
  tree->SetBranchAddress("RPW_Idx",trg_bool->RPSCI->Idx[kW]);
  tree->SetBranchAddress("RPW_TAC",trg_bool->RPSCI->TAC[kW]);
  tree->SetBranchAddress("RPW_ADC",trg_bool->RPSCI->ADC[kW]);
  tree->SetBranchAddress("RPvertex",&(trg_bool->RPSCI->vertex));
  tree->SetBranchAddress("BBCvertex",&(trg_bool->BBCvertex));

  tree->SetBranchAddress("BBCE_QTN",&(bbc->QTN[kE][0]));
  tree->SetBranchAddress("BBCW_QTN",&(bbc->QTN[kW][0]));
  tree->SetBranchAddress("BBCE_Idx",bbc->Idx[kE][0]);
  tree->SetBranchAddress("BBCE_ADC",bbc->ADC[kE][0]);
  tree->SetBranchAddress("BBCE_TAC",bbc->TAC[kE][0]);
  tree->SetBranchAddress("BBCW_Idx",bbc->Idx[kW][0]);
  tree->SetBranchAddress("BBCW_ADC",bbc->ADC[kW][0]);
  tree->SetBranchAddress("BBCW_TAC",bbc->TAC[kW][0]);
  */


  // define spinbit strings
  char spinbit_t[4][4];
  char spinbit_n[4][4];
  strcpy(spinbit_t[0],"--"); strcpy(spinbit_n[0],"nn");
  strcpy(spinbit_t[1],"-+"); strcpy(spinbit_n[1],"np");
  strcpy(spinbit_t[2],"+-"); strcpy(spinbit_n[2],"pn");
  strcpy(spinbit_t[3],"++"); strcpy(spinbit_n[3],"pp");


  // get number of runs in the redset file and build array of run numbers
  Int_t runnum_arr[30]; // (n.b. maximum size arbitrarily defined)
  Int_t runnum_tmp=0;
  Int_t NRUNS_tmp = 0;
  if(tree->GetEntries() == 0)
  {
    fprintf(stderr,"ERROR: no str entries for %s\n --> phi file not produced!\n",filename);
    return;
  };
  for(Int_t i=0; i<tree->GetEntries(); i++)
  {
    tree->GetEntry(i);
    if(runnum != runnum_tmp)
    {
      runnum_tmp = runnum;
      if(NRUNS_tmp<30) runnum_arr[NRUNS_tmp] = runnum;
      else 
      {
        fprintf(stderr,"ERROR: more than 30 runs in root file; increase arbitrarily defined max\n");
        return;
      };
      NRUNS_tmp++;
    };
  };
  const Int_t NRUNS = NRUNS_tmp;
  for(Int_t r=0; r<NRUNS; r++) printf("%d\n",runnum_arr[r]);
  printf("NRUNS=%d\n",NRUNS);



  // define kinematic distributions ("wdist" = weighting distribution)
  // -- these are distributions in energy and phi, but with finer binning; they are used 
  //    to weight the horizontal position of points on the asymmetry plots
  // -- one pt wdist for each en bin (and eta bin) 
  // -- one en wdist for each pt bin (and eta bin)
  // -- one invariant mass (mm) wdist for each eta,pt,en bin
  const Int_t NWBINS = 100;
  TH1D * pt_wdist[N_CLASS][eta_bins][en_bins][NRUNS];
  TH1D * en_wdist[N_CLASS][eta_bins][pt_bins][NRUNS];
  TH1D * mm_wdist[N_CLASS][eta_bins][pt_bins][en_bins][NRUNS];
  char pt_wdist_n[N_CLASS][eta_bins][en_bins][NRUNS][64];
  char en_wdist_n[N_CLASS][eta_bins][pt_bins][NRUNS][64];
  char mm_wdist_n[N_CLASS][eta_bins][pt_bins][en_bins][NRUNS][64];
  for(Int_t c=0; c<N_CLASS; c++)
  {
    for(Int_t r=0; r<NRUNS; r++)
    {
      for(Int_t g=0; g<eta_bins; g++)
      {
        for(Int_t e=0; e<en_bins; e++)
        {
          sprintf(pt_wdist_n[c][g][e][r],"pt_wdist_%s_g%d_e%d_r%d",ev->Name(c),g,e,runnum_arr[r]);
          pt_wdist[c][g][e][r] = new TH1D(pt_wdist_n[c][g][e][r],pt_wdist_n[c][g][e][r],NWBINS,
            env->PtLow,env->PtHigh);
        };
        for(Int_t p=0; p<pt_bins; p++)
        {
          sprintf(en_wdist_n[c][g][p][r],"en_wdist_%s_g%d_p%d_r%d",ev->Name(c),g,p,runnum_arr[r]);
          en_wdist[c][g][p][r] = new TH1D(en_wdist_n[c][g][p][r],en_wdist_n[c][g][p][r],NWBINS,
            env->EnLow,env->EnHigh);
        };
        for(Int_t p=0; p<pt_bins; p++)
        {
          for(Int_t e=0; e<en_bins; e++)
          {
            sprintf(mm_wdist_n[c][g][p][e][r],"mm_wdist_%s_g%d_p%d_e%d_r%d",ev->Name(c),g,p,e,runnum_arr[r]);
            mm_wdist[c][g][p][e][r] = new TH1D(mm_wdist_n[c][g][p][e][r],mm_wdist_n[c][g][p][e][r],NWBINS,0,1);
          };
        };
      };
    };
  };


  // define phi distributions  for each spinbit and kinematic bin
  // ---> ROOT is limited in the sense that I cannot make a pointer to
  //      a dim>=6 array... I have thus folded the spinbit and event class
  //      index into one array dimension, 10*(event class) + spinbit
  //      ... if you have a better idea, good. implment it. 
  TH1D * phi_dist[10*N_CLASS+4][eta_bins][pt_bins][en_bins][NRUNS]; // [10*event_class+spinbit] [..] ..
  TString phi_dist_n[10*N_CLASS+4][eta_bins][pt_bins][en_bins][NRUNS];
  TString phi_dist_t[10*N_CLASS+4][eta_bins][pt_bins][en_bins][NRUNS];
  char tmp_str[256];

  for(Int_t r=0; r<NRUNS; r++)
  {
    for(Int_t c=0; c<N_CLASS; c++)
    {
      for(Int_t s=0; s<4; s++)
      {
        for(Int_t g=0; g<eta_bins; g++)
        {
          for(Int_t p=0; p<pt_bins; p++)
          {
            for(Int_t e=0; e<en_bins; e++)
            {
              sprintf(tmp_str,"phi_%s_s%d_g%d_p%d_e%d_r%d",ev->Name(c),
                s,g,p,e,runnum_arr[r]);
              phi_dist_n[10*c+s][g][p][e][r] = TString(tmp_str);

              sprintf(tmp_str,
  "%s #phi distribution :: spin=(%s) #eta#in[%.2f,%.2f) p_{T}#in[%.2f,%.2f) E#in[%.2f,%.2f) :: r%d",
               ev->Title(c),spinbit_t[s],env->EtaDiv(g),env->EtaDiv(g+1),
               env->PtDiv(p),env->PtDiv(p+1),env->EnDiv(e),env->EnDiv(e+1),runnum_arr[r]);
              phi_dist_t[10*c+s][g][p][e][r] = TString(tmp_str);

              phi_dist[10*c+s][g][p][e][r] = new TH1D(phi_dist_n[10*c+s][g][p][e][r].Data(),
                phi_dist_t[10*c+s][g][p][e][r].Data(),phi_bins,env->PhiLow,env->PhiHigh);

              phi_dist[10*c+s][g][p][e][r]->Sumw2();
            };
          }; 
        };
      };
    };
  };


  // fill phi distributions and wdists 
  Int_t ss,gg,pp,ee,rr;
  Bool_t EVPinRange;
  rr=-1; runnum_tmp=0;
  printf("fill phi dists...\n");

  for(Int_t x=0; x<tree->GetEntries(); x++)
  {
    if((x%10000)==0) printf("%.2f%%\n",100*((Float_t)x)/((Float_t)tree->GetEntries()));
    ss=gg=pp=ee=-1; // reset 

    //trg_bool->RPSCI->ResetBranches(); // to ensure full arrays get reset between every event
    //trg_bool->RPSCI->ResetBits(); // to ensure bits get reset between every event
    tree->GetEntry(x);

    // run number --> array index
    if(runnum != runnum_tmp)
    {
      rr=-1;
      for(Int_t r=0; r<NRUNS; r++) { if(runnum_arr[r] == runnum) rr=r; }; 
      runnum_tmp = runnum;
      isConsistent = RD->RellumConsistent(runnum);
      pattern = RD->Pattern(runnum);
      b_pol = RD->BluePol(runnum);
      y_pol = RD->YellPol(runnum);
      //RP_nonzero = RD->RPnonzero(runnum);
    };

    // check for rellum consistency and valid polarization and FMS FP201 output bit
    if(isConsistent==1 && b_pol>0 && y_pol>0 && TrigBits&0xFFF /*&& (L2sum[1]&T->Mask(runnum,env->TriggerType,1))*/)
    {
      blue = RD->BlueSpin(runnum,bx);
      yell = RD->YellSpin(runnum,bx);
      kicked = RD->Kicked(runnum,bx);

      // check spin and kicked status
      if(abs(blue)==1 && abs(yell)==1 && kicked==0)
      {
        // spin --> array index
        ss = 2 * (blue>0) + (yell>0);

        // kinematic bins --> array indices
        for(Int_t g=0; g<eta_bins; g++) { if(Eta>=env->EtaDiv(g) && Eta<env->EtaDiv(g+1)) { gg=g; break; }; };
        for(Int_t p=0; p<pt_bins;  p++) { if(Pt>=env->PtDiv(p)   && Pt<env->PtDiv(p+1)  ) { pp=p; break; }; };
        for(Int_t e=0; e<en_bins;  e++) { if(E12>=env->EnDiv(e)  && E12<env->EnDiv(e+1) ) { ee=e; break; }; };


        // check for valid array indices (filters out events outside kinematic boundaries)
        //printf("%d %d %d %d %d\n",ss,gg,pp,ee,rr);
        /*if(ss>=0 && gg>=0 && pp>=0 && ee>=0 && rr>=0 && fabs(Phi)>3.1415/2.0)*/ /* north cells only */
        /*if(ss>=0 && gg>=0 && pp>=0 && ee>=0 && rr>=0 && fabs(Phi)<3.1415/2.0)*/ /* south cells only */
        if(ss>=0 && gg>=0 && pp>=0 && ee>=0 && rr>=0)
        {
          // set kinematics variables for event, tcu bits, rp bits
          ev->SetKinematics(runnum,E12,Pt,Eta,Phi,M12,Z,N12);

          /*
          // bbc EVP in range
          bbc->UpdateEvent();

          //EVPinRange = bbc->IsVertical();
          EVPinRange = bbc->IsHorizontal();
          //EVPinRange = true;
          */
            

          // determine event class(es)
          for(Int_t c=0; c<N_CLASS; c++)
          {
            if(ev->Valid(c))
            {
              /*
              if(debug)
              {
                gSystem->RedirectOutput("debug/debug.dat");
                trg_bool->Diagnostic(runnum,x);
                gSystem->RedirectOutput(0);
              };
              */

              phi_dist[10*c+ss][gg][pp][ee][rr]->Fill(Phi);
              pt_wdist[c][gg][ee][rr]->Fill(Pt);
              en_wdist[c][gg][pp][rr]->Fill(E12);
              mm_wdist[c][gg][pp][ee][rr]->Fill(M12);
            };
          };
        };
      };
    };
  };
        

  // make object arrays
  TFile * outfile = new TFile(outname,"RECREATE");
  TObjArray * phi_dist_arr[4][eta_bins][pt_bins][en_bins][N_CLASS];
  TObjArray * pt_wdist_arr[N_CLASS][eta_bins][en_bins];
  TObjArray * en_wdist_arr[N_CLASS][eta_bins][pt_bins];
  TObjArray * mm_wdist_arr[N_CLASS][eta_bins][pt_bins][en_bins];
  char phi_dist_arr_name[4][eta_bins][pt_bins][en_bins][N_CLASS][64];
  char pt_wdist_arr_name[N_CLASS][eta_bins][en_bins][64];
  char en_wdist_arr_name[N_CLASS][eta_bins][pt_bins][64];
  char mm_wdist_arr_name[N_CLASS][eta_bins][pt_bins][en_bins][64];
  for(Int_t c=0; c<N_CLASS; c++)
  {
    for(Int_t e=0; e<en_bins; e++)
    {
      for(Int_t g=0; g<eta_bins; g++)
      {
        for(Int_t p=0; p<pt_bins; p++)
        {
          for(Int_t s=0; s<4; s++)
          {
            phi_dist_arr[s][g][p][e][c] = new TObjArray();

            sprintf(phi_dist_arr_name[s][g][p][e][c],"phi_dist_%s_s%d_g%d_p%d_e%d",ev->Name(c),s,g,p,e);
            
            for(Int_t r=0; r<NRUNS; r++)
            {
              phi_dist_arr[s][g][p][e][c]->AddLast(phi_dist[10*c+s][g][p][e][r]);
            };
          };
        };
      };
    };
  };
  for(Int_t g=0; g<eta_bins; g++)
  {
    for(Int_t e=0; e<en_bins; e++)
    {
      for(Int_t c=0; c<N_CLASS; c++) 
      {
        pt_wdist_arr[c][g][e] = new TObjArray();
        sprintf(pt_wdist_arr_name[c][g][e],"pt_wdist_%s_g%d_e%d",ev->Name(c),g,e);
        for(Int_t r=0; r<NRUNS; r++) pt_wdist_arr[c][g][e]->AddLast(pt_wdist[c][g][e][r]);
      };
    };
    for(Int_t p=0; p<pt_bins; p++)
    {
      for(Int_t c=0; c<N_CLASS; c++) 
      {
        en_wdist_arr[c][g][p] = new TObjArray();
        sprintf(en_wdist_arr_name[c][g][p],"en_wdist_%s_g%d_p%d",ev->Name(c),g,p);
        for(Int_t r=0; r<NRUNS; r++) en_wdist_arr[c][g][p]->AddLast(en_wdist[c][g][p][r]);
      };
    };
    for(Int_t p=0; p<pt_bins; p++)
    {
      for(Int_t e=0; e<en_bins; e++)
      {
        for(Int_t c=0; c<N_CLASS; c++) 
        {
          mm_wdist_arr[c][g][p][e] = new TObjArray();
          sprintf(mm_wdist_arr_name[c][g][p][e],"mm_wdist_%s_g%d_p%d_e%d",ev->Name(c),g,p,e);
          for(Int_t r=0; r<NRUNS; r++) mm_wdist_arr[c][g][p][e]->AddLast(mm_wdist[c][g][p][e][r]);
        };
      };
    };
  };

  
  // write object arrays
  for(Int_t c=0; c<N_CLASS; c++)
  {
    outfile->mkdir(ev->Name(c));
    outfile->cd(ev->Name(c));
    for(Int_t e=0; e<en_bins; e++)
    {
      for(Int_t g=0; g<eta_bins; g++)
      {
        for(Int_t p=0; p<pt_bins; p++)
        {
          for(Int_t s=0; s<4; s++)
          {
            phi_dist_arr[s][g][p][e][c]->Write(phi_dist_arr_name[s][g][p][e][c],TObject::kSingleKey);
          };
        };
      };
    };
    outfile->cd();
  };

  // write wdists
  for(Int_t g=0; g<eta_bins; g++)
  {
    for(Int_t c=0; c<N_CLASS; c++)
    {
      for(Int_t e=0; e<en_bins; e++) pt_wdist_arr[c][g][e]->Write(pt_wdist_arr_name[c][g][e],TObject::kSingleKey);
      for(Int_t p=0; p<pt_bins; p++) en_wdist_arr[c][g][p]->Write(en_wdist_arr_name[c][g][p],TObject::kSingleKey);
      for(Int_t p=0; p<pt_bins; p++) for(Int_t e=0; e<en_bins; e++)
        mm_wdist_arr[c][g][p][e]->Write(mm_wdist_arr_name[c][g][p][e],TObject::kSingleKey);
    };
  };
};
