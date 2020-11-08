#ifndef ANALYZER_H
#define ANALYZER_H

// system include files                                                                                                                    
#include <memory>
#include <vector>
#include <algorithm>

// user include files                                                                                                                                                  
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

// Consumes
#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

// miniAOD
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

// Gen Info
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHECommonBlocks.h"

// dR and dPhi
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "TTree.h"
#include "TH2.h"
#include "TH1.h"

#include "GenAnalyzer.h"
#include "JetAnalyzer.h"

//                                                                                                                                                                            
// class declaration
//
// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");" 
// This will improve performance in multithreaded jobs.                                                                                                                                                               

class Analyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
 public:
  explicit Analyzer(const edm::ParameterSet&);
  ~Analyzer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


 private:
  virtual float deltaPhi(float,float);
  virtual float deltaR(float,float,float,float);
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  // ----------member data ---------------------------
  std::vector<reco::GenParticle> IndexByPtGen(std::vector<reco::GenParticle> vector);
  std::vector<reco::GenJet> IndexByPtGenJet(std::vector<reco::GenJet> vector);
  std::vector<std::pair<reco::GenJet*,reco::GenParticle*>> MatchJetwParton( std::vector<reco::GenJet> genjetV , std::vector<reco::GenParticle> genpartV );
  
  struct compgen {
    bool operator() (reco::GenParticle& i, reco::GenParticle& j) { return ( (i.pt()) > (j.pt()) ); } // sort in descending order 
  };

  struct comppat {
    bool operator() (reco::GenJet& i, reco::GenJet& j) { return ( (i.pt()) > (j.pt()) ); } // sort in descending order     
  };

  
  /*
  struct comppat {
    bool operator() (pat::Jet& i,pat::Jet& j) { return ( (i.pt()) > (j.pt()) ); } // sort in descending order 
    };*/

  
  edm::ParameterSet GenPSet;
  edm::ParameterSet GenJetPSet;

  GenAnalyzer* theGenAnalyzer;
  JetAnalyzer* theJetAnalyzer;

  std::string HistFile;
  bool Verbose;
  std::map<std::string, TH1F*> Hist;
  edm::Service<TFileService> fs;
  TTree* tree;

  //Variables
  int nJets;
  float EventWeight;
  float METphi;
  float METeta;
  float RecoJphi;
  float RecoJeta;
    
};

#endif
