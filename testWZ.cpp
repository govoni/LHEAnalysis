#include "LHEF.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TLorentzVector.h"
#include <cmath>
//#include "hFactory.h"
//#include "h2Factory.h"
//#include "hFunctions.h"

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

/*
  c++ -o testWZ `root-config --glibs --cflags` \
     -lm testWZ.cpp
*/


using namespace std ;


double 
deltaPhi (double phi1, double phi2)
{

  double deltaphi=fabs(phi1-phi2);
  if (deltaphi > 6.283185308) deltaphi -= 6.283185308;
  if (deltaphi > 3.141592654) deltaphi = 6.283185308-deltaphi;
  return deltaphi;
}


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


struct ptSort: public std::binary_function<TLorentzVector &, TLorentzVector &, bool>
{
  bool operator() (TLorentzVector & x, TLorentzVector & y)
    {
      return x.Pt () < y.Pt () ;
    }
} ;


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


struct etaSort: public std::binary_function<TLorentzVector &, TLorentzVector &, bool>
{
  bool operator() (TLorentzVector & x, TLorentzVector & y)
    {
      return x.Eta () < y.Eta () ;
    }
} ;




// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


template <class TLV>
pair<int, int> findPairWithWMass (const vector<TLV> & v_f_quarks)
{
  double ref_deltaM = 100000. ;
  int one = 0 ;
  int two = 0 ;
  
  for (int iJ = 0 ; iJ < 4 ; ++iJ)
    for (int iJ2 = iJ + 1 ; iJ2 < 4 ; ++iJ2)
      {
        double deltaM = fabs ((v_f_quarks.at (iJ) + v_f_quarks.at (iJ2)).M () - 80.4) ;
        if (deltaM < ref_deltaM)
          {
            ref_deltaM = deltaM ;
            one = iJ ;
            two = iJ2 ;
          }      
      }
  return pair<int, int> (one, two) ;
  
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


TH1F *
addHistoToMap (map<string, TH1F *> & hmap, string name, int bin, float min, float max)
{
  TH1F * dummy = new TH1F (name.c_str (), name.c_str (), bin, min, max) ;
  dummy->Sumw2 () ;
  hmap[name] = dummy ;
  return dummy ;
}


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


void 
savemap (map<string, TH1F *> & hmap, TFile & outfile, float scale)
{
  outfile.cd () ;
  for (map<string, TH1F *>::iterator iMap = hmap.begin () ;
      iMap != hmap.end () ; ++ iMap)
    {
      iMap->second->Scale (scale) ;
      iMap->second->Write () ;
    }
}


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


map<string, TH1F *>
readSample (string sampleName, string radice, int maxevents = -1)
{
  cout << "reading " << sampleName << endl ;
  std::ifstream ifs (sampleName.c_str ()) ;
  LHEF::Reader reader (ifs) ;
  
  map<string, TH1F *> histos ;

  TH1F * h_initPart        = addHistoToMap (histos, string ("initPart_")         + radice, 135, -22.5, 22.5) ;
  TH1F * h_finalJetsPDGId  = addHistoToMap (histos, string ("finalJetsPDGId_")   + radice, 135, -22.5, 22.5) ;

  TH1F * h_lep0_eta  = addHistoToMap (histos, string ("lep0_eta_")   + radice, 40, -6, 6) ;
  TH1F * h_lep0_pt   = addHistoToMap (histos, string ("lep0_pt_")    + radice, 100, 0, 400) ;
  TH1F * h_lep0_phi  = addHistoToMap (histos, string ("lep0_phi_")   + radice, 30, -3.14, 3.14) ;
                                                                    
  TH1F * h_lep1_eta  = addHistoToMap (histos, string ("lep1_eta_")   + radice, 40, -6, 6) ;
  TH1F * h_lep1_pt   = addHistoToMap (histos, string ("lep1_pt_")    + radice, 100, 0, 400) ;
  TH1F * h_lep1_phi  = addHistoToMap (histos, string ("lep1_phi_")   + radice, 30, -3.14, 3.14) ;
                                                                    
  TH1F * h_lep2_eta  = addHistoToMap (histos, string ("lep2_eta_")   + radice, 40, -6, 6) ;
  TH1F * h_lep2_pt   = addHistoToMap (histos, string ("lep2_pt_")    + radice, 100, 0, 400) ;
  TH1F * h_lep2_phi  = addHistoToMap (histos, string ("lep2_phi_")   + radice, 30, -3.14, 3.14) ;
                                                                    
  TH1F * h_met_eta   = addHistoToMap (histos, string ("met_eta_")    + radice, 40, -6, 6) ;
  TH1F * h_met_pt    = addHistoToMap (histos, string ("met_pt_")     + radice, 100, 0, 400) ;
  TH1F * h_met_phi   = addHistoToMap (histos, string ("met_phi_")    + radice, 30, -3.14, 3.14) ;
                                                                    
  TH1F * h_vbf0_eta  = addHistoToMap (histos, string ("vbf0_eta_")   + radice, 40, -6, 6) ;
  TH1F * h_vbf0_pt   = addHistoToMap (histos, string ("vbf0_pt_")    + radice, 100, 0, 400) ;
  TH1F * h_vbf0_phi  = addHistoToMap (histos, string ("vbf0_phi_")   + radice, 30, -3.14, 3.14) ;
                                                                    
  TH1F * h_vbf1_eta  = addHistoToMap (histos, string ("vbf1_eta_")   + radice, 40, -6, 6) ;
  TH1F * h_vbf1_pt   = addHistoToMap (histos, string ("vbf1_pt_")    + radice, 100, 0, 250) ;
  TH1F * h_vbf1_phi  = addHistoToMap (histos, string ("vbf1_phi_")   + radice, 30, -3.14, 3.14) ;
                                                                    
  TH1F * h_mjj_vbf   = addHistoToMap (histos, string ("mjj_vbf_")    + radice, 25, 0, 2500) ;
  TH1F * h_NJ        = addHistoToMap (histos, string ("NJ_")         + radice, 5, 0, 5) ;

  TH1F * h_m4l       = addHistoToMap (histos, string ("m4l_")        + radice, 35, 0, 2275) ;
    
  int ieve = 0 ;
  int btagged = 0 ;
  int selected = 0 ;
  // loop over events
  while ( reader.readEvent () ) 
    {
      if (ieve % 10000 == 0) std::cout << "event " << ieve << "\n" ;
      if (maxevents > 0 && ieve >= maxevents) break ;
      ++ieve;
  
      vector<int> initialParticlesPDGId ;      
      vector<int> finalJetsPDGId ;      
      vector<TLorentzVector> finalLeptons ;      
      vector<TLorentzVector> finalNeutrinos ;      
      vector<TLorentzVector> initialQuarks ;      
      vector<TLorentzVector> finalJets ;      
      vector<TLorentzVector> finalBs ;      
      
      int leptonsIDproduct = 1 ;

      //PG loop over particles in the event
      //PG and fill the variables of leptons and quarks
      for (int iPart = 0 ; iPart < reader.hepeup.IDUP.size (); ++iPart)
        {
//          std::cout << "\t part type [" << iPart << "] " << reader.hepeup.IDUP.at (iPart)
//                    << "\t status " << reader.hepeup.ISTUP.at (iPart)
//                    << "\n" ;

          //PG incoming particle          
          if (reader.hepeup.ISTUP.at (iPart) == -1)
            {
              initialParticlesPDGId.push_back (reader.hepeup.IDUP.at (iPart)) ;
              initialQuarks.push_back (TLorentzVector
                (
                  reader.hepeup.PUP.at (iPart).at (0), //PG px
                  reader.hepeup.PUP.at (iPart).at (1), //PG py
                  reader.hepeup.PUP.at (iPart).at (2), //PG pz
                  reader.hepeup.PUP.at (iPart).at (3) //PG E
                )) ;
            } //PG incoming particle          

          //PG outgoing particles          
          if (reader.hepeup.ISTUP.at (iPart) == 1)
            {
              // jets  
              if (abs (reader.hepeup.IDUP.at (iPart)) < 7 ||  // quarks
                  abs (reader.hepeup.IDUP.at (iPart)) == 21 ) // gluons
                {
                  finalJetsPDGId.push_back (reader.hepeup.IDUP.at (iPart)) ;
                  finalJets.push_back (TLorentzVector
                    (
                      reader.hepeup.PUP.at (iPart).at (0), //PG px
                      reader.hepeup.PUP.at (iPart).at (1), //PG py
                      reader.hepeup.PUP.at (iPart).at (2), //PG pz
                      reader.hepeup.PUP.at (iPart).at (3)  //PG E
                    )) ;


                  if (abs (reader.hepeup.IDUP.at (iPart)) == 5)  // b's
                    {
                      finalBs.push_back (TLorentzVector
                        (
                          reader.hepeup.PUP.at (iPart).at (0), //PG px
                          reader.hepeup.PUP.at (iPart).at (1), //PG py
                          reader.hepeup.PUP.at (iPart).at (2), //PG pz
                          reader.hepeup.PUP.at (iPart).at (3)  //PG E
                        )) ;
                    } // b's
                } // jets
              else if (abs (reader.hepeup.IDUP.at (iPart)) == 11 ||
                       abs (reader.hepeup.IDUP.at (iPart)) == 13 ) // charged leptons
                {
                  leptonsIDproduct *= reader.hepeup.IDUP.at (iPart) ;
                  finalLeptons.push_back (TLorentzVector
                    (
                      reader.hepeup.PUP.at (iPart).at (0), //PG px
                      reader.hepeup.PUP.at (iPart).at (1), //PG py
                      reader.hepeup.PUP.at (iPart).at (2), //PG pz
                      reader.hepeup.PUP.at (iPart).at (3)  //PG E
                    )) ;
                } //PG charged leptons
              else if (abs (reader.hepeup.IDUP.at (iPart)) == 12 ||
                       abs (reader.hepeup.IDUP.at (iPart)) == 14 ) // neutrinos
                {
                  finalNeutrinos.push_back (TLorentzVector
                    (
                      reader.hepeup.PUP.at (iPart).at (0), //PG px
                      reader.hepeup.PUP.at (iPart).at (1), //PG py
                      reader.hepeup.PUP.at (iPart).at (2), //PG pz
                      reader.hepeup.PUP.at (iPart).at (3)  //PG E
                    )) ;
                } // neutrinos

            } //PG outgoing particles
        } //PG loop over particles in the event

      if (initialParticlesPDGId.size () != 2) exit (1) ; 

      if (finalJets.size () != 2) continue ;
      if (finalNeutrinos.size () != 1) continue ;
      if (finalLeptons.size () != 3) continue ;

//      //PG no b's in the initial state
//      if (fabs (initialParticlesPDGId.at (0)) > 4) continue ;
//      if (fabs (initialParticlesPDGId.at (1)) > 4) continue ;

//      //PG only b's in the initial state   
//      if (fabs (initialParticlesPDGId.at (0)) < 5) continue ;
//      if (fabs (initialParticlesPDGId.at (1)) < 5) continue ;
// (no events in the sample)

      //PG only one b in the initial state
      if ((fabs (initialParticlesPDGId.at (0)) > 4 &&
           fabs (initialParticlesPDGId.at (1)) > 4) ||
          (fabs (initialParticlesPDGId.at (0)) < 5 &&
           fabs (initialParticlesPDGId.at (1)) < 5)) continue ;

      // the following remove events with guons as initial particles
//      if (fabs (initialParticlesPDGId.at (0)) > 6) continue ;
//      if (fabs (initialParticlesPDGId.at (1)) > 6) continue ;
      
      sort (finalJets.rbegin (), finalJets.rend (), ptSort ()) ;
      if (finalJets.at (0).Pt () < 20) continue ;
      if (finalJets.at (1).Pt () < 20) continue ;
      if (finalJets.at (0).E () < 20) continue ;
      if (finalJets.at (1).E () < 20) continue ;
      if (fabs (finalJets.at (0).Eta ()) > 6.5) continue ;
      if (fabs (finalJets.at (1).Eta ()) > 6.5) continue ;

      sort (finalLeptons.rbegin (), finalLeptons.rend (), ptSort ()) ;
      if (finalLeptons.at (0).Pt () < 20) continue ;
      if (finalLeptons.at (1).Pt () < 20) continue ;
      if (finalLeptons.at (2).Pt () < 20) continue ;
      if (finalLeptons.at (0).E () < 20) continue ;
      if (finalLeptons.at (1).E () < 20) continue ;
      if (finalLeptons.at (2).E () < 20) continue ;
      if (fabs (finalLeptons.at (0).Eta ()) > 4) continue ;
      if (fabs (finalLeptons.at (1).Eta ()) > 4) continue ;
      if (fabs (finalLeptons.at (2).Eta ()) > 4) continue ;

      float mjj = (finalJets.at (0) + finalJets.at (1)).M () ;
      if (mjj < 600) continue ;
      if (fabs (finalJets.at (0).Eta () - finalJets.at (1).Eta ()) < 2) continue ;
      float m4l = (finalNeutrinos.at (0) + finalLeptons.at (0) + finalLeptons.at (1) + finalLeptons.at (2)).M () ;
      if (m4l < 130) continue ;

      ++selected ;      
      h_NJ->Fill (finalJets.size ()) ;
//      cout << "Njs = " << finalJets.size () << "\n" ;
//      cout << "pts: " 
//           << finalJets.at (0).Pt () 
//           << "\t"
//           << finalJets.at (1).Pt () 
//           << "\n" ;

      for (int i = 0 ; i < initialParticlesPDGId.size () ; ++i)
        h_initPart->Fill (initialParticlesPDGId.at (i)) ;
      for (int i = 0 ; i < finalJetsPDGId.size () ; ++i)
        h_finalJetsPDGId->Fill (finalJetsPDGId.at (i)) ;
      
      h_vbf0_eta->Fill (finalJets.at (0).Eta ()) ;            
      h_vbf0_phi->Fill (finalJets.at (0).Phi ()) ;            
      h_vbf0_pt-> Fill (finalJets.at (0).Pt ()) ;        

      h_vbf1_eta->Fill (finalJets.at (1).Eta ()) ;            
      h_vbf1_phi->Fill (finalJets.at (1).Phi ()) ;            
      h_vbf1_pt-> Fill (finalJets.at (1).Pt ()) ;        

      h_lep0_eta->Fill (finalLeptons.at (0).Eta ()) ;            
      h_lep0_phi->Fill (finalLeptons.at (0).Phi ()) ;            
      h_lep0_pt-> Fill (finalLeptons.at (0).Pt ()) ;        

      h_lep1_eta->Fill (finalLeptons.at (1).Eta ()) ;            
      h_lep1_phi->Fill (finalLeptons.at (1).Phi ()) ;            
      h_lep1_pt-> Fill (finalLeptons.at (1).Pt ()) ;        

      h_lep2_eta->Fill (finalLeptons.at (2).Eta ()) ;            
      h_lep2_phi->Fill (finalLeptons.at (2).Phi ()) ;            
      h_lep2_pt-> Fill (finalLeptons.at (2).Pt ()) ;        

      h_met_eta->Fill (finalNeutrinos.at (0).Eta ()) ;            
      h_met_phi->Fill (finalNeutrinos.at (0).Phi ()) ;            
      h_met_pt-> Fill (finalNeutrinos.at (0).Pt ()) ;        

      h_mjj_vbf->Fill (mjj) ;
      h_m4l->Fill (m4l) ;
      
    } // loop over events
  cout << "total eff  : " << selected * 1./ieve << endl ;
  return histos ;
}


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


int main (int argc, char **argv) 
{
  gROOT->SetStyle ("Plain") ;

  int N_MG = 50000 ;
  float XS_MG = 6.45 ; /* fb */
  map<string, TH1F *> hmap_MG = 
    readSample ("/Users/govoni/data/TP/compareEWK/WZ/madgraph.lhe", "MG", N_MG) ;

  int N_PH = 79938 ;
  float XS_PH = 7.854 ; /* fb */
  map<string, TH1F *> hmap_PH = 
    readSample ("/Users/govoni/data/TP/compareEWK/WZ/phantom.lhe", "PH", N_PH) ;

  TFile outfile ("testWZ.root", "recreate") ;
  savemap (hmap_MG,  outfile,  XS_MG * 1./N_MG) ;  /*result in fb */
  savemap (hmap_PH,  outfile,  XS_PH * 1./N_PH) ;  /*result in fb */
  
  gStyle->SetStatStyle (0) ; 
  gStyle->SetTitleStyle (0) ; 
  gStyle->SetCanvasBorderSize (0) ; 
  gStyle->SetFrameBorderSize (0) ; 
  gStyle->SetLegendBorderSize (0) ; 
  gStyle->SetStatBorderSize (0) ; 
  gStyle->SetTitleBorderSize (0) ; 
  gStyle->SetTitleYOffset (2) ;

  TCanvas c1 ;
  c1.SetLeftMargin (0.17) ; 
  c1.SetTopMargin (0.1) ; 
  
  map<string, TH1F *>::iterator iMap_PH = hmap_PH.begin () ;

  // plotting
  for (map<string, TH1F *>::iterator iMap_MG = hmap_MG.begin () ;
       iMap_MG != hmap_MG.end () ;
       ++iMap_MG)
    {
      iMap_MG->second->SetStats (0) ;
      iMap_PH->second->SetStats (0) ;
      
      iMap_MG->second->SetTitle ("") ;
      iMap_PH->second->SetTitle ("") ;
      
      iMap_MG->second->SetLineWidth (2) ;
      iMap_PH->second->SetLineWidth (2) ;
      
      iMap_MG->second->SetLineColor (30) ;
      iMap_PH->second->SetLineColor (9) ;
      
      iMap_MG->second->SetFillColor (30) ;

      TLegend leg (0.3, 0.9, 0.8, 1) ;
      leg.SetNColumns (3) ;
      leg.SetLineStyle (0) ;
      leg.SetFillStyle (0) ;
      leg.AddEntry (iMap_MG->second, "madgraph", "fl") ;
      leg.AddEntry (iMap_PH->second, "phantom", "fl") ;
      
      iMap_MG->second->GetXaxis ()->SetTitle (iMap_MG->first.c_str ()) ;        
      iMap_PH->second->GetXaxis ()->SetTitle (iMap_PH->first.c_str ()) ;        
      iMap_PH->second->Draw () ;           
      iMap_MG->second->Draw ("same") ;           
      iMap_MG->second->DrawCopy ("histsame") ;   
      iMap_PH->second->SetFillStyle (3001) ;
      iMap_PH->second->SetFillColor (9) ;
      iMap_PH->second->DrawCopy ("E2same") ;           
      iMap_PH->second->SetFillStyle (0) ;
      iMap_PH->second->SetFillColor (0) ;
      iMap_PH->second->Draw ("same") ;           
      leg.Draw () ;
      c1.Print (("WZ_" + iMap_MG->first + ".png").c_str (), "png") ;
      iMap_PH->second->DrawNormalized ("E2") ;           
      iMap_MG->second->DrawNormalized ("histsame") ;   
      iMap_PH->second->DrawNormalized ("E2same") ;           
      leg.Draw () ;
      c1.Print (("WZ_" + iMap_MG->first + "_norm.png").c_str (), "png") ;

      ++iMap_PH ;
    }   
  
  return 0 ;
}


