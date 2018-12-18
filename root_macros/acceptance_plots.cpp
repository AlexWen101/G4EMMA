#include "TROOT.h"
#include <TCanvas.h>
#include <TF1.h>
#include <TF2.h>
#include <TObject.h>
#include <TFile.h>
#include <TTree.h>

void acceptance_plots() {

// /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\

  // call the data files
  TFile *ang_file = new TFile("/home/awen/G4EMMA_data/Ang_acceptance_test_10k/Results/GEMMAoutput.root");
  TFile *energy_file = new TFile("/home/awen/G4EMMA_data/Energy_acceptance_1k/Results/GEMMAoutput.root");

  // create a new file in case I want to write anything to save
  //TFile *file = new TFile("acceptance_plots.root","RECREATE");

  // call the trees in the data files
  TTree *ang_tree = (TTree*)ang_file->Get("fphits");
  TTree *energy_tree = (TTree*)energy_file->Get("fphits");
  TTree *ang_tree_target = (TTree*)ang_file->Get("targetplane");

  // data variables
  Double_t fp_posX_a, fp_posY_a, tar_angX_a, tar_angY_a; //angular data from events that made it to the focal plane
  Double_t fp_posX_e, fp_posY_e, tar_energy; // energy data from events taht made it to the focal plane
  Double_t target_angX_total, target_angY_total;  //angular data from the region of interest that were TOTAL emitted

// /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\

  // get the values of the data from the tree and populate our variables
  ang_tree->SetBranchAddress("fp_posX",&fp_posX_a);
  ang_tree->SetBranchAddress("fp_posY",&fp_posY_a);
  ang_tree->SetBranchAddress("target_angX",&tar_angX_a);
  ang_tree->SetBranchAddress("target_angY",&tar_angY_a);
  Int_t nentries = (Int_t)ang_tree->GetEntries();
  energy_tree->SetBranchAddress("fp_posX",&fp_posX_e);
  energy_tree->SetBranchAddress("fp_posY",&fp_posY_e);
  energy_tree->SetBranchAddress("target_Ekin",&tar_energy);
  Int_t pentries = (Int_t)energy_tree->GetEntries();

  ang_tree_target->SetBranchAddress("target_xang", &target_angX_total);
  ang_tree_target->SetBranchAddress("target_yang", &target_angY_total);
  Int_t qentries = (Int_t)ang_tree_target->GetEntries();

  // make the histograms to fill

  TH2F* ang_XY = new TH2F("angle distribution","Target angle distribution from focal plane",100,-0.1,0.1,100,-0.1,0.1);

  TH1F* energy = new TH1F("energy","Target energy distribution",100,50,150);

  TH2F* ang_XY_total = new TH2F("angle distribution","TOTAL Target angle distribution in acceptance region",100,-0.1,0.1,100,-0.1,0.1);


  // get the max and min values of energy and angles that made it to the focal plane
  Double_t max_e = 100;
  Double_t min_e = 100;
  Double_t max_ang_x = 0;
  Double_t min_ang_x = 0;
  Double_t max_ang_y = 0;
  Double_t min_ang_y = 0;


// /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^



  // fill the angular acceptance histograms
  for (Int_t i=0; i<nentries; i++) {

    ang_tree->GetEntry(i);

    //ang_X->Fill(tar_angX_a);
    //ang_Y->Fill(tar_angY_a);
    ang_XY->Fill(tar_angX_a,tar_angY_a);

    //-----------------------------------------------------------------------------------//
    // find min and max angles in the x and y directions
    if (tar_angX_a >= max_ang_x)
      max_ang_x = tar_angX_a;

    if (tar_angX_a <= min_ang_x)
      min_ang_x = tar_angX_a;

    if (tar_angY_a >= max_ang_y)
      max_ang_y = tar_angY_a;

    if (tar_angY_a <= min_ang_y)
      min_ang_y = tar_angY_a;



    //-----------------------------------------------------------------------------------//

  }

  // fill the histogram that is independent of the focal plane, for comparison
  // essentially "cut" by the max and min angles of the events that did make it through to the focal plane

  for (Int_t i=0; i<qentries; i++) {

    ang_tree_target->GetEntry(i);

    if (target_angX_total<max_ang_x
        && target_angX_total>min_ang_x
        && target_angY_total<max_ang_y
        && target_angY_total>min_ang_y)
      ang_XY_total->Fill(target_angX_total,target_angY_total);

  }



// /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^

  // fill the energy acceptance histograms
  for (int i=0; i<pentries; i++) {
    energy_tree->GetEntry(i);
    energy->Fill(tar_energy);

    //-----------------------------------------------------------------------------------//
    // find the max and min energy
    if (tar_energy >= max_e) {
      max_e = tar_energy;
    }
    if (tar_energy <= min_e) {
      min_e = tar_energy;
    }
    //-----------------------------------------------------------------------------------//
  }

// /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\


// make a bunch of canvases and display the histograms
TCanvas * c1 = new TCanvas("c1");
c1->Divide(3,2);
c1->cd(1);
TH1F* ang_X = (TH1F*)ang_XY->ProjectionX();
ang_X->GetXaxis()->SetTitle("X (rad)");
ang_X->Draw();

c1->cd(2);
TH1F* ang_Y = (TH1F*)ang_XY->ProjectionY();
ang_Y->GetXaxis()->SetTitle("Y (rad)");
ang_Y->Draw();

c1->cd(3);
ang_XY->GetXaxis()->SetTitle("X (rad)");
ang_XY->GetYaxis()->SetTitle("Y (rad)");
ang_XY->Draw();

c1->cd(4);
TH1F * ang_X_total = (TH1F*)ang_XY_total->ProjectionX();
ang_X_total->GetXaxis()->SetTitle("X (rad)");
ang_X_total->Draw();

c1->cd(5);
TH1F* ang_Y_total = (TH1F*)ang_XY_total->ProjectionY();
ang_Y_total->GetXaxis()->SetTitle("Y (rad)");
ang_Y_total->Draw();

c1->cd(6);
ang_XY_total->GetXaxis()->SetTitle("X (rad)");
ang_XY_total->GetYaxis()->SetTitle("Y (rad)");
ang_XY_total->Draw();

TCanvas * misc = new TCanvas("misc");
ang_XY->Draw("colz");


TCanvas * c3 = new TCanvas("c3");
energy->GetXaxis()->SetTitle("MeV");
TH1F* energy_all = (TH1F*)energy_file->Get("targetEkin");
energy->Draw();
energy_all->SetLineColor(2);
energy_all->Draw("same");


// Display some important max/min values
std::cout << "Max energy accepted: " << max_e << " Min energy accepted: " << min_e << std::endl;
std::cout << "Max X angle: " << max_ang_x << " Min X angle: " << min_ang_x << std::endl;
std::cout << "Max Y angle: " << max_ang_y << " Min Y angle: " << min_ang_y << std::endl;


}
