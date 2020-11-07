//#include "RecoilCorrector.hh" // From: https://github.com/cms-met/MetTools/tree/master/RecoilCorrections

#include "JetAnalyzer.h"


JetAnalyzer::JetAnalyzer(edm::ParameterSet& PSet, edm::ConsumesCollector&& CColl):
  Jetak4Token(CColl.consumes<std::vector<reco::GenJet> >(PSet.getParameter<edm::InputTag>("ak4genJets"))),
  Jetak8Token(CColl.consumes<std::vector<reco::GenJet> >(PSet.getParameter<edm::InputTag>("ak8genJets"))),
  MetcaloToken(CColl.consumes<std::vector<reco::GenMET> >(PSet.getParameter<edm::InputTag>("genmetCalo"))),
  MettrueToken(CColl.consumes<std::vector<reco::GenMET> >(PSet.getParameter<edm::InputTag>("genmetTrue")))
{
    
    std::cout << " --- JetAnalyzer initialization ---" << std::endl;
    /**
    std::cout << "  jet Id            :\t" << JetId << std::endl;
    std::cout << "  jet pT [1, 2]     :\t" << Jet1Pt << "\t" << Jet2Pt << std::endl;
    std::cout << "  jet eta           :\t" << JetEta << std::endl;
    std::cout << "  b-tagging algo    :\t" << BTag << std::endl;
    std::cout << "  b-tag cut [1, 2]  :\t" << Jet1BTag << "\t" << Jet2BTag << std::endl;
    std::cout << "  apply recoil corr :\t" << (UseRecoil ? "YES" : "NO") << std::endl;
    std::cout << "  recoil file MC    :\t" << RecoilMCFile << std::endl;
    std::cout << "  recoil file Data  :\t" << RecoilDataFile << std::endl;
    std::cout << std::endl;
    **/
}

JetAnalyzer::~JetAnalyzer() {

}
std::vector<reco::GenJet> JetAnalyzer::FillAK4GenJetVector(const edm::Event& iEvent) {
  std::vector<reco::GenJet> Vect;
  // Declare and open collection
  edm::Handle<std::vector<reco::GenJet> > GenJetsCollection;
  iEvent.getByToken(Jetak4Token, GenJetsCollection);

  // Loop on Jet collection                                                                                                                                                             
  for(std::vector<reco::GenJet>::const_iterator it=GenJetsCollection->begin(); it!=GenJetsCollection->end(); ++it) {
    reco::GenJet genjet=*it;
    Vect.push_back(genjet);
  }
  return Vect;
}

std::vector<reco::GenJet> JetAnalyzer::FillAK8GenJetVector(const edm::Event& iEvent) {
  std::vector<reco::GenJet> Vect;
  // Declare and open collection                                                                                                                                                                                                            
  edm::Handle<std::vector<reco::GenJet> > GenJetsCollection;
  iEvent.getByToken(Jetak8Token, GenJetsCollection);

  // Loop on Jet collection                                                                                                                                                                                                              
  for(std::vector<reco::GenJet>::const_iterator it=GenJetsCollection->begin(); it!=GenJetsCollection->end(); ++it) {
  reco::GenJet genjet=*it;
  Vect.push_back(genjet);
  }
  return Vect;
}
//HERE
//void JetAnalyzer::MatchAK4JetwGenPart(std::vector<pat::Jet>& Jets, ####, float angle) {
//  for(unsigned int m = 0; m < Muons.size(); m++) {
//    for(unsigned int j = 0; j < Jets.size(); ) {
//      if(deltaR(Jets[j], Muons[m]) < angle) Jets.erase(Jets.begin() + j);
//      else j++;
//    }
//  }
//}

void JetAnalyzer::CleanJetsFromMuons(std::vector<pat::Jet>& Jets, std::vector<pat::Muon>& Muons, float angle) {
    for(unsigned int m = 0; m < Muons.size(); m++) {
        for(unsigned int j = 0; j < Jets.size(); ) {
            if(deltaR(Jets[j], Muons[m]) < angle) Jets.erase(Jets.begin() + j);
            else j++;
        }
    }
}

void JetAnalyzer::CleanJetsFromElectrons(std::vector<pat::Jet>& Jets, std::vector<pat::Electron>& Electrons, float angle) {
    for(unsigned int e = 0; e < Electrons.size(); e++) {
        for(unsigned int j = 0; j < Jets.size(); ) {
            if(deltaR(Jets[j], Electrons[e]) < angle) Jets.erase(Jets.begin() + j);
            else j++;
        }
    }
}

reco::GenMET JetAnalyzer::FillCaloMetVector(const edm::Event& iEvent) {
  edm::Handle<std::vector<reco::GenMET> > MetCollection;
  iEvent.getByToken(MetcaloToken, MetCollection);
  reco::GenMET MEt = MetCollection->front();
  return MEt;
}

reco::GenMET JetAnalyzer::FillTrueMetVector(const edm::Event& iEvent) {
  edm::Handle<std::vector<reco::GenMET> > MetCollection;
  iEvent.getByToken(MettrueToken, MetCollection);
  reco::GenMET MEt = MetCollection->front();
  return MEt;
}
