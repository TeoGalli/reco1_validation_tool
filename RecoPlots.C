#include <fstream>
#include <string>
#include <vector>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TEfficiency.h"
#include "TStyle.h"

const char* MISSING_VALUE = "999999"; // Uninitialised value

struct Plot {
    string name;
    double min;
    double max;
    double bins;
    bool log;
    string cut = "";
    string cutName = "";
    bool Weight;
};

TString GetLimitsString(Plot toPlot) {
    auto name = toPlot.name.c_str();

    TString limit = Form(
            "(%s >= %f) && (%s <= %f) && (%s == %s) && (%s != %s)",
            name, toPlot.min,   // >= Min Limit
            name, toPlot.max,   // <= Max Limit
            name, name,         // Value is equal itself (NaN check)
            name, MISSING_VALUE // Isn't an uninitialised value
    );

    if (toPlot.cut == "")
        return limit;

    return Form("(%s) && (%s)", limit.Data(), toPlot.cut.c_str());
}

TString GetBranchNameString(Plot toPlot) {
    auto name = toPlot.name.c_str();
    auto binWidth = (toPlot.max - toPlot.min) / toPlot.bins;

    auto branchToHist = Form(
            "%s>>currBranch(%f,%f,%f)",
            name,
            toPlot.bins + 2,       // Account for the 2 edge bins
            toPlot.min - binWidth,
            toPlot.max + binWidth
    );

    return branchToHist;
}

void RecoPlots(TString s_CurFile = "", TString s_treeName = "analysistree/tree") {

  const char* treeName = s_treeName.Data();
  const char* outputFolderName = "reco1AnaEff";

  TFile *inputFile = TFile::Open(s_CurFile.Data());
  TTree *inputTree = nullptr;

  if (inputFile == nullptr)
        throw std::runtime_error("Could not open input file.");

  inputFile->GetObject(treeName, inputTree);

  double entries = inputTree->GetEntries();


  if (inputTree == nullptr) {
        std::cout << "Unable to get tree at " << treeName << ", aborting!." << std::endl;
        throw std::runtime_error("Could not get tree from file.");
  }

  // Open the two output files
  TFile *histFile = new TFile("reco1_validation_histos.root","RECREATE");
  std::ofstream efficiencyFile("overall_efficiency.txt");
  efficiencyFile << Form(
          "%-25s, %-7s, %-8s, %-8s, %s\n",
          "Name", "Entries", "Mean", "Std Dev", "Cut"
  );

  std::vector<Plot> plotsToMake = {
      // To Plot                Min,  Max,  Bins, Log,  Cut,  Plot Name, Weight
      {"mode_truth",         -0.5, 2.5,   3,           false, "", "", false},
      {"nuPDG_truth",        11,   17,    entries/100, false, "", "", false},
      {"ccnc_truth",         0,    2,     entries/100, false, "", "", false},
      {"nuvtxx_truth",       -400, 400,   entries/200, false, "", "", false},
      {"nuvtxy_truth",       -700, 700,   entries/200, false, "", "", false},
      {"nuvtxz_truth",       -100, 1500,  entries/200, false, "", "", false},
      {"enu_truth",          0,    10,    entries/100, true,  "", "", false},
      {"lep_mom_truth",      0,    10,    entries/100, true,  "", "", false},
      {"nuWeight_truth",     0,    90e6,  entries/100, false, "", "", false},
      {"enu_truth",          0,    10,    entries/100, false, "", "", true},
      {"genie_no_primaries", 0,    50,    entries/200, false, "", "", false},
      {"hit_charge",         0,    2500,  entries/100, false, "", "", false},
      {"hit_peakT",          0,    5000,  entries/200, false, "", "", false},
      {"hit_width",          0,    150,   entries/150, false, "", "", false},
      {"hit_plane",          -1,   3,     4,           false, "", "", false},
      //{"hit_trueX",          -400, 400,   entries/200, false, "", "", false},
      {"no_hits",            0,    10000, entries/100, false, "", "", false},
      {"no_photons",         0,    1e4,   entries/200, false, "", "", false},
      {"nu_dcosx_truth",     -1,   1,     entries/100, false, "", "", false},
      {"nu_dcosy_truth",     -1,   1,     entries/200, false, "", "", false},
      {"nu_dcosz_truth",     -1,   1,     entries/100, false, "", "", false},
      {"zenith_truth",       0,    4,     entries/200, false, "", "", false},
      {"azimut_truth",       -2,   2,     entries/200, false, "", "", false},
      {"beamangleX_truth",   0,    4,     entries/200, false, "", "", false},
      {"beamangleY_truth",   0,    4,     entries/200, false, "", "", false},
      {"beamangleZ_truth",   0,    4,     entries/200, false, "", "", false},
      {"theta_truth",        0,    4,     entries/100, false, "", "", false},
      {"zenith_truth",       0,    4,     entries/200, false, "", "", true},
      {"azimut_truth",       -2,   2,     entries/200, false, "", "", true},
      {"e_dep",              0,    2.5,   entries/100, false, "", "", true},
      {"num_photons",        0,    5000,  entries/100, false, "", "", false},
      {"num_electrons",      0,    14000, entries/100, false, "", "", false},
      {"nu_dcosy_truth",     -1,   1,     entries/200, false, "", "", true},
      // To Plot                Min,  Max,  Bins, Log,  Cut,  Plot Name

      {"e_dep",              0,    0.3,   entries/100, false, "(ccnc_truth[0] == 0) && (abs(nuPDG_truth[0]) == 12)", "cc_nue", false},
      {"e_dep",              0,    0.3,   entries/100, false, "(ccnc_truth[0] == 0) && (abs(nuPDG_truth[0]) == 14)", "cc_numu", false},
      {"e_dep",              0,    0.3,   entries/100, false, "(ccnc_truth[0] == 1)", "nc", false},
      {"no_photons",              0,    5e5,   entries/200, false, "(ccnc_truth[0] == 0) && (abs(nuPDG_truth[0]) == 12)", "cc_nue", false},
      {"no_photons",              0,    5e5,   entries/200, false, "(ccnc_truth[0] == 0) && (abs(nuPDG_truth[0]) == 14)", "cc_numu", false},
      {"no_photons",              0,    8e4,   entries/200, false, "(ccnc_truth[0] == 1)", "nc", false},
      
  };

  // Sort style and output location
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);
  gStyle->SetHistLineColor(kBlack);
  gStyle->SetLabelColor(kBlack,"xyz");
  gStyle->SetTitleColor(kBlack);
  gSystem->Exec(Form("mkdir -p -v %s", outputFolderName));
  histFile->cd();
  gDirectory->mkdir(outputFolderName);
  histFile->cd(outputFolderName);

  TCanvas *c1 = new TCanvas("c1", "reco1AnaEff");
  c1->cd();

  // Draw the required branches, using a cut if one is given.
  for (auto plot : plotsToMake) {
    auto name = plot.name.c_str();
    
    if (plot.Weight == true) {
      inputTree->Draw(GetBranchNameString(plot), "nuWeight_truth" );
    } else {
      inputTree->Draw(GetBranchNameString(plot), GetLimitsString(plot));
    }
    c1->SetLogy(plot.log);
    c1->Update();

    TH1F *currBranch = (TH1F*)gPad->GetPrimitive("currBranch");
    currBranch->SetName(name);
    currBranch->SetTitle(name);

    if (plot.Weight == true) {
      currBranch->SetName(Form("%s_weighted", name));
      currBranch->SetTitle(Form("%s_weighted", name));
    }

    // Pull out the drawn plot, and get some stats to write to the file
    int nEntries = currBranch->GetEntries();
    double mean = currBranch->GetMean();
    double stdDev = currBranch->GetStdDev();

    // If a plot with a cut, include that cut name in the plot name.
    if (plot.cutName == "") {
        efficiencyFile << Form(
                "%-25s, %-7i, %-8.3f, %-8.3f\n",
                name, nEntries, mean, stdDev
        );
        c1->Print(Form("%s/%s.png", outputFolderName, name));
    } else {
        auto cutName = plot.cutName.c_str();

        currBranch->SetName(Form("%s_%s", name, cutName));
        currBranch->SetTitle(Form("%s_%s", name, cutName));
        c1->Print(Form("%s/%s_%s.png", outputFolderName, name, plot.cutName.c_str()));
        efficiencyFile << Form(
                "%-25s, %-7i, %-8.3f, %-8.3f, %s\n",
                name, nEntries, mean, stdDev, plot.cutName.c_str()
        );
    }
    currBranch->Write();
  }

  delete c1;

  efficiencyFile.close();
  histFile->Close();
}
