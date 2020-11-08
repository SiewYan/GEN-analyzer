// -*- C++ -*-
//
// Package:    edmAna/LambdaAnayzer
// Class:      LambdaAnayzer
// 
/**\class LambdaAnayzer LambdaAnayzer.cc edmAna/LambdaAnayzer/plugins/LambdaAnayzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Siew Yan Hoh
//         Created:  Tue, 12 Feb 2019 20:23:44 GMT
//
//

#include "Analyzer.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
Analyzer::Analyzer(const edm::ParameterSet& iConfig):
  GenPSet(iConfig.getParameter<edm::ParameterSet>("genSet")),
  GenJetPSet(iConfig.getParameter<edm::ParameterSet>("genjetSet")),
  HistFile(iConfig.getParameter<std::string>("histFile")),
  Verbose(iConfig.getParameter<bool>("verbose"))
{
   //now do what ever initialization is needed
   usesResource("TFileService");

   // Initialize Objects
   theGenAnalyzer      = new GenAnalyzer(GenPSet, consumesCollector());
   theJetAnalyzer      = new JetAnalyzer(GenJetPSet, consumesCollector());

   //  ---------- Plots Initialization ----------
   TFileDirectory allDir=fs->mkdir("All/");
   TFileDirectory genDir=fs->mkdir("Gen/");
   TFileDirectory ggfDir=fs->mkdir("ggF/");
   TFileDirectory vbfDir=fs->mkdir("VBF/");
 
   //make TH1F
   //std::vector<std::string> nLabels={"All", "Trigger", "Iso Lep #geq 2", "Z cand ", "Jets #geq 2", "Z mass ", "h mass ", "Top veto", "bJets #geq 1", "bJets #geq 2"};
   std::vector<std::string> nLabels={"All", "MET > 200", "MET > 250", "Z cand ", "Jets #geq 2", "Z mass ", "h mass ", "Top veto", "bJets #geq 1", "bJets #geq 2"};

   int nbins;
   float min, max;
   std::string name, title, opt;

   std::ifstream histFile(HistFile);
   if(!histFile.is_open()) {
     throw cms::Exception("Analyzer", HistFile + " file not found");
   }
   while(histFile >> name >> title >> nbins >> min >> max >> opt) {

     if(name.find('#')==std::string::npos) {
       while(title.find("~")!=std::string::npos) title=title.replace(title.find("~"), 1, " "); // Remove ~
       if(name.substr(0, 2)=="a_") Hist[name] = allDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
       if(name.substr(0, 2)=="g_") Hist[name] = genDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
       if(name.substr(0, 2)=="f_") Hist[name] = ggfDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
       if(name.substr(0, 2)=="v_") Hist[name] = vbfDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);

     Hist[name]->Sumw2();
     Hist[name]->SetOption(opt.c_str());
     if(name=="a_nEvents" || name=="e_nEvents" || name=="m_nEvents") for(unsigned int i=0; i<nLabels.size(); i++) Hist[name]->GetXaxis()->SetBinLabel(i+1, nLabels[i].c_str());
     }
   }
   histFile.close();

   std::cout << "---------- STARTING ----------" << std::endl;
}

Analyzer::~Analyzer(){
  
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
  std::cout << "---------- ENDING  ----------" << std::endl;
  delete theGenAnalyzer;
  delete theJetAnalyzer;
  
}


//
// member functions
//

float Analyzer::deltaPhi(float phi1, float phi2){
  float PHI = fabs(phi1-phi2);
  if (PHI<=3.14159265)
    return PHI;
  else
    return 2*3.14159265-PHI; 
}

std::vector<reco::GenParticle> Analyzer::IndexByPtGen(std::vector<reco::GenParticle> vector)
{
  compgen comparatorgen;
  std::sort (vector.begin() , vector.end() , comparatorgen);
  return vector;
}

std::vector<reco::GenJet> Analyzer::IndexByPtGenJet(std::vector<reco::GenJet> vector)
{
  comppat comparatorpat;
  std::sort (vector.begin() , vector.end() , comparatorpat);
  return vector;
}

float Analyzer::deltaR(float phi1, float eta1, float phi2, float eta2){
  return sqrt(pow((eta2-eta1),2)+pow(deltaPhi(phi1,phi2),2));
}

// ------------ method called for each event  ------------
void
Analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
 
   //Initialize event variable
   nJets=0; EventWeight=1.;
   
   // Gen Particle
   std::vector<int> LepIds = {11,13,-11,-13};
   std::vector<int> TauIds = {15,-15};
   std::vector<int> NeuIds = {12,14,16,-12,-14,-16};
   std::vector<int> HadIds = {1,2,3,4,5,6,-1,-2,-3,-4,-5,6,21};
   std::vector<int> WIds = {24,-24};
   std::vector<int> ZIds = {23};
   std::vector<int> HIds = {25};
   // https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookGenParticleCandidate
   // HepMC sherpa hard outgoing --> 3
   // Madgraph hard outgoing --> 23

   // GenPart
   std::vector<reco::GenParticle> GenPVect = theGenAnalyzer->FillGenVector(iEvent);

   // Higgs
   std::vector<reco::GenParticle> GenHVect = theGenAnalyzer->FilterGenVectorbyId( GenPVect , HIds );
   std::vector<reco::GenParticle> GenHFVect = IndexByPtGen( theGenAnalyzer->FilterGenVectorbyStatus( GenHVect , {3,62} ) );
   // filter GenH
   int nGenH=0;
   reco::Particle::LorentzVector HH;
   reco::Particle::LorentzVector H1(0.,0.,0.,0.);
   reco::Particle::LorentzVector H2(0.,0.,0.,0.);
   for(unsigned int i = 0; i < GenHVect.size(); i++){
     reco::GenParticle* higgs = &(GenHVect[i]);
     if ( ( ( higgs->status() == 62 ) && ( theGenAnalyzer->FindLastDaughterGen(higgs)->status() == 62 ) ) || (higgs->status() == 3) ){
       Hist["g_H1status"]->Fill( (theGenAnalyzer->FindLastDaughterGen(higgs))->status());
       nGenH++;
       if (nGenH<3){
	 Hist[("g_H"+std::to_string(nGenH)+"pt").c_str()]->Fill(higgs->pt(), EventWeight);
	 Hist[("g_H"+std::to_string(nGenH)+"eta").c_str()]->Fill(higgs->eta(), EventWeight);
	 Hist[("g_H"+std::to_string(nGenH)+"phi").c_str()]->Fill(higgs->phi(), EventWeight);
	 Hist[("g_H"+std::to_string(nGenH)+"pdgid").c_str()]->Fill(higgs->pdgId(), EventWeight);
	 Hist[("g_H"+std::to_string(nGenH)+"mass").c_str()]->Fill(higgs->mass(), EventWeight);
	 Hist[("g_H"+std::to_string(nGenH)+"status").c_str()]->Fill(higgs->status(), EventWeight);
       }
       if (nGenH == 1) H1 = higgs->p4();
       if (nGenH == 2) H2 = higgs->p4();
     }
   }
   
   Hist["g_nH"]->Fill(nGenH);
   Hist["g_HHdPhi"]->Fill(deltaPhi( H1.Phi() , H2.Phi() ), EventWeight);
   Hist["g_HHdR"]->Fill(deltaR( H1.Phi(), H1.Eta(), H2.Phi(), H2.Eta() ), EventWeight);
   
   // Higgs composite variables.
   HH = H1 + H2;
   Hist["g_HHpt"]->Fill(HH.Pt(), EventWeight);
   Hist["g_HHeta"]->Fill(HH.Eta(), EventWeight);
   Hist["g_HHphi"]->Fill(HH.Phi(), EventWeight);
   Hist["g_HHmass"]->Fill(HH.M(), EventWeight);
   
   std::vector<reco::GenParticle> GenQVect = theGenAnalyzer->FilterGenVectorbyId( GenPVect , HadIds );
   std::vector<reco::GenParticle> GenQFVect = IndexByPtGen( theGenAnalyzer->FilterGenVectorbyStatus( GenQVect , {3,23} ) ); // filter only outgoing hard parton
   //
   int nhardquark=0;
   for(unsigned int i = 0; i < GenQFVect.size(); i++){
     reco::GenParticle* quarks = &(GenQFVect[i]);
     if ( 
	 theGenAnalyzer->FindLastDaughterGen(quarks)->status() == quarks->status() // last copy of outgoing hard parton
	 && theGenAnalyzer->FindLastDaughterGen(quarks)->pdgId() == quarks->pdgId() // last copy pf outgoing hard parton
	 && theGenAnalyzer->FindMother(quarks)->pdgId() != 25 // exclude quarks decay from Higgs
	 //&& theGenAnalyzer->FindMother(quarks)->status() == 22 // only parton decay from intermediate particle, hard scattering
	  ){
       nhardquark++;
       if (i<3){
	 Hist[("g_Had"+std::to_string(i+1)+"pt").c_str()]->Fill(quarks->pt(), EventWeight);
	 Hist[("g_Had"+std::to_string(i+1)+"eta").c_str()]->Fill(quarks->eta(), EventWeight);
	 Hist[("g_Had"+std::to_string(i+1)+"phi").c_str()]->Fill(quarks->phi(), EventWeight);
	 Hist[("g_Had"+std::to_string(i+1)+"pdgid").c_str()]->Fill(quarks->pdgId(), EventWeight);
	 Hist[("g_Had"+std::to_string(i+1)+"status").c_str()]->Fill(quarks->status(), EventWeight);
	 Hist[("g_Had"+std::to_string(i+1)+"momstatus").c_str()]->Fill( theGenAnalyzer->FindMother(quarks)->status() , EventWeight);
	 Hist[("g_Had"+std::to_string(i+1)+"mompdgid").c_str()]->Fill( theGenAnalyzer->FindMother(quarks)->pdgId() , EventWeight);
       }
     }
   }
   Hist["g_nHad"]->Fill(nhardquark);
   
   // GenJet AK4
   std::vector<std::pair<reco::GenJet*,reco::GenParticle*>> MatchAK4Jets;
   std::vector<reco::GenJet> aK4JetsVect = IndexByPtGenJet( theJetAnalyzer->FillAK4GenJetVector(iEvent) );
   // match to genparticle 
   for(unsigned int i = 0; i < aK4JetsVect.size(); i++){
     reco::GenJet* ak4jet = &(aK4JetsVect[i]);
     float drmin = 99999.;
     int QIdx= -1;
     for(unsigned int j = 0; j < GenQFVect.size(); j++){
       reco::GenParticle* quarks = &(GenQFVect[j]);
       // a match
       float dr = deltaR( ak4jet->phi() , ak4jet->eta() , quarks->phi() , quarks->eta() );
       if( dr <= 0.4 && dr < drmin ){
	 drmin = dr;
	 QIdx = j;
       }
     }// gen loops
     //std::cout<<"final dr : "<< drmin <<std::endl;
     if ( QIdx !=-1 ) MatchAK4Jets.push_back( std::make_pair( ak4jet , &(GenQFVect[QIdx]) ) );
   }// ak4 jet quark loops
   Hist["g_GenAK4nJet"]->Fill(MatchAK4Jets.size());
  
   reco::Particle::LorentzVector AK4jj;
   reco::Particle::LorentzVector AK4j1(0.,0.,0.,0.);
   reco::Particle::LorentzVector AK4j2(0.,0.,0.,0.);
   
   for(unsigned int i = 0; i < MatchAK4Jets.size(); i++){
     reco::GenJet* ak4jet = MatchAK4Jets[i].first;
     reco::GenParticle* quarks = MatchAK4Jets[i].second;
     if (i<3){
       Hist[("g_GenAK4Jet"+std::to_string(i+1)+"pt").c_str()]->Fill(ak4jet->pt(), EventWeight);
       Hist[("g_GenAK4Jet"+std::to_string(i+1)+"eta").c_str()]->Fill(ak4jet->eta(), EventWeight);
       Hist[("g_GenAK4Jet"+std::to_string(i+1)+"phi").c_str()]->Fill(ak4jet->phi(), EventWeight);
       Hist[("g_GenAK4Jet"+std::to_string(i+1)+"mass").c_str()]->Fill(ak4jet->mass(), EventWeight);
       Hist[("g_GenAK4Jet"+std::to_string(i+1)+"pdgid").c_str()]->Fill(quarks->pdgId() , EventWeight);
       if (i==0) AK4j1 = ak4jet->p4();
       if (i==1) AK4j2 = ak4jet->p4();
     }
   }
   
   AK4jj = AK4j1 + AK4j2;
   Hist["g_GenAK4j12dPhi"]->Fill(deltaPhi( AK4j1.Phi() , AK4j2.Phi() ), EventWeight);
   Hist["g_GenAK4j12dR"]->Fill(deltaR( AK4j1.Phi(), AK4j1.Eta(), AK4j2.Phi(), AK4j2.Eta() ), EventWeight);
   // Jet composite variable
   Hist["g_GenAK4j12m"]->Fill(AK4jj.M(), EventWeight);
   Hist["g_HHGenAK4j12dPhi"]->Fill( deltaPhi( HH.Phi() , AK4jj.Phi() ) , EventWeight );
   Hist["g_HHGenAK4j12Pt"]->Fill( (AK4jj+HH).Pt() , EventWeight );
   
   tree->Fill();
   
   //std::cout << "Filling finished" << std::endl;
   //std::cout << std::endl;
}


// ------------ method called once each job just before starting event loop  ------------
void 
Analyzer::beginJob()
{
  tree=fs->make<TTree>("tree", "tree");
}

// ------------ method called once each job just after ending the event loop  ------------
void 
Analyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Analyzer);
