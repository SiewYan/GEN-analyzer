#ifndef JETANALYZER_H
#define JETANALYZER_H

#include <iostream>
#include <fstream>
#include <cmath>
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
//#include "RecoParticleFlow/PFProducer/interface/Utils.h"
#include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TRandom3.h"

class JetAnalyzer {
    public:
        JetAnalyzer(edm::ParameterSet&, edm::ConsumesCollector&&);
        virtual ~JetAnalyzer();
	virtual std::vector<reco::GenJet> FillAK4GenJetVector(const edm::Event&);
	virtual std::vector<reco::GenJet> FillAK8GenJetVector(const edm::Event&);
        //virtual std::vector<pat::Jet> FillJetVector(const edm::Event&);
        virtual void CleanJetsFromMuons(std::vector<pat::Jet>&, std::vector<pat::Muon>&, float);
        virtual void CleanJetsFromElectrons(std::vector<pat::Jet>&, std::vector<pat::Electron>&, float);
        //virtual pat::MET FillMetVector(const edm::Event&);
	virtual reco::GenMET FillCaloMetVector(const edm::Event&);
	virtual reco::GenMET FillTrueMetVector(const edm::Event&);
      
    private:

	edm::EDGetTokenT<std::vector<reco::GenJet> > Jetak4Token;
	edm::EDGetTokenT<std::vector<reco::GenJet> > Jetak8Token;
	edm::EDGetTokenT<std::vector<reco::GenMET> > MetcaloToken;
	edm::EDGetTokenT<std::vector<reco::GenMET> > MettrueToken;;

};

#endif
