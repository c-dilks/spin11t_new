// Implementation of L2 trigger daq ids, used for masking 
// L2sum in the trigger files

#ifndef LevelTwo_
#define LevelTwo_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <map>
#include <stdexcept>

#include "TSystem.h"
#include "TObject.h"
#include "TTree.h"
#include "TString.h"

class LevelTwo : public TObject
{
  public:
    LevelTwo();

    Int_t Mask(char * trigger0);
    Int_t Mask(Int_t num0);
    Int_t Index(char * trigger0);
    char * Name(Int_t num0);


    Int_t N; // number of FMS triggers

  private:
    char spindir[512];
    //TTree * id_tr;

    // MAPS:
    // here trigger idx refers to the enumerator in the namespace
    //
    std::map<Int_t, char*> trigger_name; // trigger idx --> trigger name
    std::map<Int_t, Int_t> trigger_bitmask; // trigger idx --> trigger FP201 bit mask
    std::map<std::string, Int_t> trigger_idx; // trigger name --> trigger idx
    std::map<Int_t, Int_t> trigger_bitmaskrev; // trigger FP201 bit mask --> trigger idx 
    // mask_map[2]: (it's a nested map) 
    // key = run number, value = trig_idx map
    //                           -->trig_idx map:
    //                              key = trigger index, value = daq trigger id
    //std::map<Int_t, std::map<Int_t,Long_t> > mask_map;

    // RP MAPS (same convention as above)
    /*
    std::map<Int_t, char*> rptrigger_name;
    std::map<Int_t, char*> rptrigger_dbname;
    std::map<std::string, Int_t> rptrigger_idx;
    std::map<std::string, Int_t> rptrigger_dbidx;
    std::map<Int_t, std::map<Int_t,Long_t> > rpmask_map;
    */

    ClassDef(LevelTwo,2);
};

#endif
