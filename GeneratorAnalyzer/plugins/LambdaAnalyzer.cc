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

#include "LambdaAnalyzer.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
LambdaAnalyzer::LambdaAnalyzer(const edm::ParameterSet& iConfig):
  GenPSet(iConfig.getParameter<edm::ParameterSet>("genSet")),
  GenJetPSet(iConfig.getParameter<edm::ParameterSet>("genjetSet")),
  HistFile(iConfig.getParameter<std::string>("histFile")),
  Verbose(iConfig.getParameter<bool>("verbose"))
{
   //now do what ever initialization is needed
   usesResource("TFileService");
   //TFileDirectory allDir=fs->mkdir("All/");

   // Initialize Objects
   theGenAnalyzer      = new GenAnalyzer(GenPSet, consumesCollector());
   theJetAnalyzer      = new JetAnalyzer(GenJetPSet, consumesCollector());

   //  ---------- Plots Initialization ----------
   TFileDirectory allDir=fs->mkdir("All/");
   TFileDirectory genDir=fs->mkdir("Gen/");
   
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

     Hist[name]->Sumw2();
     Hist[name]->SetOption(opt.c_str());
     if(name=="a_nEvents" || name=="e_nEvents" || name=="m_nEvents") for(unsigned int i=0; i<nLabels.size(); i++) Hist[name]->GetXaxis()->SetBinLabel(i+1, nLabels[i].c_str());
     }
   }
   histFile.close();

   std::cout << "---------- STARTING ----------" << std::endl;
}

LambdaAnalyzer::~LambdaAnalyzer(){
  
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
  std::cout << "---------- ENDING  ----------" << std::endl;
  delete theGenAnalyzer;
  delete theJetAnalyzer;
  
}


//
// member functions
//

float LambdaAnalyzer::deltaPhi(float phi1, float phi2){
  float PHI = fabs(phi1-phi2);
  if (PHI<=3.14159265)
    return PHI;
  else
    return 2*3.14159265-PHI; 
}

std::vector<reco::GenParticle> LambdaAnalyzer::IndexByPtGen(std::vector<reco::GenParticle> vector)
{
  compgen comparatorgen;

  std::sort (vector.begin() , vector.end() , comparatorgen);
  return vector;
}

std::vector<reco::GenJet> LambdaAnalyzer::IndexByPtGenJet(std::vector<reco::GenJet> vector)
{
  comppat comparatorpat;

  std::sort (vector.begin() , vector.end() , comparatorpat);
  return vector;
}

float LambdaAnalyzer::deltaR(float phi1, float eta1, float phi2, float eta2){
  return sqrt(pow((eta2-eta1),2)+pow(deltaPhi(phi1,phi2),2));
}

// ------------ method called for each event  ------------
void
LambdaAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
 
   //Initialize event variable
   nJets=0; EventWeight=1.;

   if(Verbose) std::cout<<"here1"<<std::endl;
  
   Hist["a_nEvents"]->Fill(1.,EventWeight);

   //GEN
   // Gen Weight
   //std::map<int, float> GenWeight = theGenAnalyzer->FillWeightsMap(iEvent);

   // Lhe Particles 
   //std::map<std::string, float> LheMap = theGenAnalyzer->FillLheMap(iEvent);

   // Gen Particle
   std::vector<int> LepIds = {11,13,15,-11,-13,-15};
   std::vector<int> MuIds = {13,-13};
   std::vector<int> ElIds = {11,-11};
   std::vector<int> NeuIds = {12,14,16,-12,-14,-16};
   std::vector<int> HadIds = {1,2,3,4,5,-1,-2,-3,-4,-5,22};

   std::vector<int> WIds = {24,-24};
   std::vector<int> ZIds = {23};
   std::vector<int> HIds = {25};
   std::vector<int> ZpIds = {1023};

   std::vector<reco::GenParticle> GenPVect = theGenAnalyzer->FillGenVector(iEvent);

   // Gen candidates
   // filter out leptons
   std::vector<reco::GenParticle> theGenMu;
   std::vector<reco::GenParticle> theGenEl;
   std::vector<reco::GenParticle> theGenHad;
   
   std::vector<reco::GenParticle> theGenmu = theGenAnalyzer->FilterGenVectorbyId(GenPVect, MuIds);
   std::vector<reco::GenParticle> theGenel = theGenAnalyzer->FilterGenVectorbyId(GenPVect, ElIds);
   std::vector<reco::GenParticle> theGenhad = theGenAnalyzer->FilterGenVectorbyId(GenPVect, HadIds);
   theGenMu = IndexByPtGen(theGenmu);
   theGenEl = IndexByPtGen(theGenel);
   theGenHad = IndexByPtGen(theGenhad);

   /*
   for(unsigned int i = 0; i < theGenmu.size(); i++){
     if (theGenmu[i].status()!=1 ) continue;

     std::cout<<"unsorted"<<std::endl;
     std::cout<<i<<"th gen mu pt = "<<theGenmu[i].pt()<<std::endl;
     std::cout<<"pdgid in filter = "<<theGenmu[i].pdgId()<<std::endl;
     std::cout<<i<<"th gen mu status = "<<theGenmu[i].status()<<std::endl;
     std::cout<<i<<"th gen mu charge = "<<theGenmu[i].charge()<<std::endl;
     }*/

   // boson
   reco::Candidate* theGenZ = theGenAnalyzer->FindGenParticle(GenPVect, 23);
   reco::Candidate* theGenW = theGenAnalyzer->FindGenParticle(GenPVect, 24);
   //reco::Candidate* theGenH = theGenAnalyzer->FindGenParticle(GenPVect, 25);

   std::vector<reco::GenParticle> thegenz = theGenAnalyzer->FilterGenVectorbyId(GenPVect, WIds);
   std::vector<reco::GenParticle> thegenw = theGenAnalyzer->FilterGenVectorbyId(GenPVect, ZIds);
   std::vector<reco::GenParticle> thegenh = theGenAnalyzer->FilterGenVectorbyId(GenPVect, HIds);
   std::vector<reco::GenParticle> thezp = theGenAnalyzer->FilterGenVectorbyId(GenPVect, ZpIds);

   if(Verbose) std::cout<<"here2"<<std::endl;
   
   for(unsigned int i = 0; i < thezp.size(); i++){
     reco::GenParticle* darkphoton = &(thezp[i]);
     //if ( darkphoton->status()!=62 ) continue;
     Hist["g_Zppt"]->Fill(darkphoton->pt(), EventWeight);
     Hist["g_Zpeta"]->Fill(darkphoton->eta(), EventWeight);
     Hist["g_Zpphi"]->Fill(darkphoton->phi(), EventWeight);
     Hist["g_Zpmass"]->Fill(darkphoton->mass(), EventWeight);
     Hist["g_Zppdgid"]->Fill(darkphoton->pdgId(), EventWeight);
     Hist["g_Zpstatus"]->Fill(darkphoton->status(), EventWeight);
   }

   // higgs
   reco::Particle::LorentzVector HH;
   reco::Particle::LorentzVector H1(0.,0.,0.,0.);
   reco::Particle::LorentzVector H2(0.,0.,0.,0.);

   unsigned int higgsc =0;
   std::vector<reco::GenParticle*> theChosenOneHiggs;
   //std::cout<<"check 0"<<std::endl;

   for(unsigned int i = 0; i < thegenh.size(); i++){
     reco::GenParticle* higgs = &(thegenh[i]);
     Hist["g_HiggsStatus"]->Fill(higgs->status() , EventWeight);
     Hist["g_ntotHiggs"]->Fill( thegenh.size() , EventWeight);
   }

   for(unsigned int i = 0; i < thegenh.size(); i++){
     reco::GenParticle* higgs = &(thegenh[i]);
     if (higgsc > 1 ) break;
     if ( ( higgs->status() == 62 ) || ( higgs->status() == 1 ) ){
       Hist[("g_H"+std::to_string(higgsc+1)+"pt").c_str()]->Fill(higgs->pt(), EventWeight);
       Hist[("g_H"+std::to_string(higgsc+1)+"eta").c_str()]->Fill(higgs->eta(), EventWeight);
       Hist[("g_H"+std::to_string(higgsc+1)+"phi").c_str()]->Fill(higgs->phi(), EventWeight);
       Hist[("g_H"+std::to_string(higgsc+1)+"pdgid").c_str()]->Fill(higgs->pdgId(), EventWeight);
       Hist[("g_H"+std::to_string(higgsc+1)+"mass").c_str()]->Fill(higgs->mass(), EventWeight);
       Hist[("g_H"+std::to_string(higgsc+1)+"status").c_str()]->Fill(higgs->status(), EventWeight);
       higgsc++;
       if (higgsc == 1 ) H1 = higgs->p4();
       if (higgsc == 2 ) H2 = higgs->p4();
       theChosenOneHiggs.push_back(higgs);
     }
   }
   Hist["g_nH"]->Fill(higgsc+1);
   
   HH = H1 + H2;
   //std::cout<<"check 1"<<std::endl;
   Hist["g_HHpt"]->Fill(HH.Pt(), EventWeight);
   Hist["g_HHeta"]->Fill(HH.Eta(), EventWeight);
   Hist["g_HHphi"]->Fill(HH.Phi(), EventWeight);
   Hist["g_HHmass"]->Fill(HH.M(), EventWeight);
   //std::cout<<"check 2"<<std::endl;
   if (theChosenOneHiggs.size()>0 || theChosenOneHiggs.size()>=2){
     //deltaPhi of two higgs
     Hist["g_HHdPhi"]->Fill(deltaPhi( theChosenOneHiggs[0]->phi() , theChosenOneHiggs[1]->phi() ), EventWeight);
     //deltaR of two higgs
     Hist["g_HHdR"]->Fill(deltaR( theChosenOneHiggs[0]->phi(), theChosenOneHiggs[0]->eta(), theChosenOneHiggs[1]->phi(), theChosenOneHiggs[1]->eta() ), EventWeight);
   } //here
   //std::cout<<"check 3"<<std::endl;

   if(Verbose) std::cout<<"here3"<<std::endl;

   // exotica
   //reco::Candidate* theZp = theGenAnalyzer->FindGenParticle(GenPVect, 1023);

   //FILL
   reco::Particle::LorentzVector V;
   reco::Particle::LorentzVector Mu1(0.,0.,0.,0.);
   reco::Particle::LorentzVector Mu2(0.,0.,0.,0.);

   if(Verbose) std::cout<<"here4"<<std::endl;

   unsigned int ct=0;
   std::vector<reco::GenParticle*> theChosenOneMuon;
   for(unsigned int i = 0; i < theGenMu.size(); i++){
     reco::GenParticle* part = &(theGenMu[i]);
     if (part->status()!=1 ) continue;

     //std::cout<<"Sorted"<<std::endl;
     //std::cout<<i<<"th theGenMu pt = "<<part->pt()<<std::endl;
     //std::cout<<"pdgid in filter = "<<part->pdgId()<<std::endl;
     //std::cout<<i<<"th theGenMu status = "<<part->status()<<std::endl;
     //std::cout<<i<<"th theGenMu charge = "<<part->charge()<<std::endl;

     if ( ct > 2 ) break;
     Hist[("g_Mu"+std::to_string(ct+1)+"pt").c_str()]->Fill(part->pt(), EventWeight);
     Hist[("g_Mu"+std::to_string(ct+1)+"eta").c_str()]->Fill(part->eta(), EventWeight);
     Hist[("g_Mu"+std::to_string(ct+1)+"phi").c_str()]->Fill(part->phi(), EventWeight);
     Hist[("g_Mu"+std::to_string(ct+1)+"pdgid").c_str()]->Fill(part->pdgId(), EventWeight);
     Hist[("g_Mu"+std::to_string(ct+1)+"status").c_str()]->Fill(part->status(), EventWeight);
     Hist[("g_Mu"+std::to_string(ct+1)+"ch").c_str()]->Fill(part->charge(), EventWeight);
     ct++;
     if (ct == 1 ) Mu1 = part->p4();
     if (ct == 2 ) Mu2 = part->p4();
     theChosenOneMuon.push_back(part);
   }
   Hist["g_nMu"]->Fill(ct+1);

   if(Verbose) std::cout<<"here5"<<std::endl;
   
   V = Mu1 + Mu2;
   Hist["g_Vpt"]->Fill(V.Pt(), EventWeight);
   Hist["g_Veta"]->Fill(V.Eta(), EventWeight);
   Hist["g_Vphi"]->Fill(V.Phi(), EventWeight);
   Hist["g_Vmass"]->Fill(V.M(), EventWeight);

   if(Verbose) std::cout<<"here6"<<std::endl;

   for(unsigned int i = 0; i < theGenEl.size(); i++){
     reco::GenParticle* part = &(theGenEl[i]);
     if (i>2) break;
     Hist[("g_El"+std::to_string(i+1)+"pt").c_str()]->Fill(part->pt(), EventWeight);
     Hist[("g_El"+std::to_string(i+1)+"eta").c_str()]->Fill(part->eta(), EventWeight);
     Hist[("g_El"+std::to_string(i+1)+"phi").c_str()]->Fill(part->phi(), EventWeight);
     Hist[("g_El"+std::to_string(i+1)+"pdgid").c_str()]->Fill(part->pdgId(), EventWeight);
   }

   if(Verbose) std::cout<<"here7"<<std::endl;

   for(unsigned int i = 0; i < theGenHad.size(); i++){
     reco::GenParticle* part = &(theGenHad[i]);
     if (i>2) break;
     if (part->status()!=23) continue;
     Hist[("g_Had"+std::to_string(i+1)+"pt").c_str()]->Fill(part->pt(), EventWeight);
     Hist[("g_Had"+std::to_string(i+1)+"eta").c_str()]->Fill(part->eta(), EventWeight);
     Hist[("g_Had"+std::to_string(i+1)+"phi").c_str()]->Fill(part->phi(), EventWeight);
     Hist[("g_Had"+std::to_string(i+1)+"pdgid").c_str()]->Fill(part->pdgId(), EventWeight);
     Hist[("g_Had"+std::to_string(i+1)+"status").c_str()]->Fill(part->status(), EventWeight);
   }

   if(Verbose) std::cout<<"here8"<<std::endl;
   
   if ( theChosenOneMuon.size()>=2 ){
     //deltaPhi of two muon
     Hist["g_Mu12dPhi"]->Fill(deltaPhi( theChosenOneMuon[0]->phi() , theChosenOneMuon[1]->phi() ), EventWeight);
     //deltaR of two muon
     Hist["g_Mu12dR"]->Fill(deltaR( theChosenOneMuon[0]->phi(), theChosenOneMuon[0]->eta(), theChosenOneMuon[1]->phi(), theChosenOneMuon[1]->eta() ), EventWeight);
   }

   if(Verbose) std::cout<<"here9"<<std::endl;
   
   if( theGenEl.size()>=2 ){
     //deltaPhi of two electron
     Hist["g_El12dPhi"]->Fill(deltaPhi( theGenEl[0].phi() , theGenEl[1].phi() ), EventWeight);
     //deltaR of two electron
     Hist["g_El12dR"]->Fill(deltaR( theGenEl[0].phi(), theGenEl[0].eta(), theGenEl[1].phi(), theGenEl[1].eta() ), EventWeight);
   }

   if(Verbose) std::cout<<"here10"<<std::endl;
   
   if (theGenZ!=NULL){
     Hist["g_GenZpt"]->Fill(theGenZ->pt(), EventWeight);
     Hist["g_GenZeta"]->Fill(theGenZ->eta(), EventWeight);
     Hist["g_GenZphi"]->Fill(theGenZ->phi(), EventWeight);
     Hist["g_GenZmass"]->Fill(theGenZ->mass(), EventWeight);
     Hist["g_GenZpdgid"]->Fill(theGenZ->pdgId(), EventWeight);
   }

   if(Verbose) std::cout<<"here11"<<std::endl;

   if (theGenW!=NULL){
     Hist["g_GenWpt"]->Fill(theGenW->pt(), EventWeight);
     Hist["g_GenWeta"]->Fill(theGenW->eta(), EventWeight);
     Hist["g_GenWphi"]->Fill(theGenW->phi(), EventWeight);
     Hist["g_GenWmass"]->Fill(theGenW->mass(), EventWeight);
     Hist["g_GenWpdgid"]->Fill(theGenW->pdgId(), EventWeight);
   }

   if(Verbose) std::cout<<"here12"<<std::endl;

   // *****************************************************

   // Gen Jet
   std::vector<reco::GenJet> AK4JetsVect;
   std::vector<reco::GenJet> AK8JetsVect;
   std::vector<reco::GenJet> aK4JetsVect = theJetAnalyzer->FillAK4GenJetVector(iEvent);
   std::vector<reco::GenJet> aK8JetsVect = theJetAnalyzer->FillAK8GenJetVector(iEvent);
   AK4JetsVect = IndexByPtGenJet(aK4JetsVect);
   AK8JetsVect = IndexByPtGenJet(aK8JetsVect);

   reco::Particle::LorentzVector AK4jj;
   reco::Particle::LorentzVector AK4j1(0.,0.,0.,0.);
   reco::Particle::LorentzVector AK4j2(0.,0.,0.,0.);
   unsigned int countj=0;
   //AK4
   Hist["g_GenAK4nJet"]->Fill(AK4JetsVect.size(), EventWeight);
   for(unsigned int i = 0; i < AK4JetsVect.size(); i++){
     reco::GenJet* jet = &(AK4JetsVect[i]);
     if (i>2) break;
     Hist[("g_GenAK4Jet"+std::to_string(i+1)+"pt").c_str()]->Fill(jet->pt(), EventWeight);
     Hist[("g_GenAK4Jet"+std::to_string(i+1)+"eta").c_str()]->Fill(jet->eta(), EventWeight);
     Hist[("g_GenAK4Jet"+std::to_string(i+1)+"phi").c_str()]->Fill(jet->phi(), EventWeight);
     Hist[("g_GenAK4Jet"+std::to_string(i+1)+"mass").c_str()]->Fill(jet->mass(), EventWeight);
     Hist[("g_GenAK4Jet"+std::to_string(i+1)+"pdgid").c_str()]->Fill(jet->pdgId(), EventWeight);
     countj++;
     if (i==1) AK4j1 = jet->p4();
     if (i==2) AK4j2 = jet->p4();
   }
   //std::cout<<"check 4"<<std::endl;
   
   AK4jj = AK4j1 + AK4j2;
   if ( AK4jj.M() > 100 )
     Hist["g_GenAK4j12m"]->Fill(AK4jj.M(), EventWeight);
   Hist["g_HHGenAK4j12dPhi"]->Fill( deltaPhi( HH.Phi() , AK4jj.Phi() ) , EventWeight );
   Hist["g_HHGenAK4j12Pt"]->Fill( (AK4jj+HH).Pt() , EventWeight );

   //std::cout<<"check 5"<<std::endl;

   if(Verbose) std::cout<<"here13"<<std::endl;
  
   if (AK4JetsVect.size()>0 || AK4JetsVect.size()>=2){
     //deltaPhi of two lepton                                                                                                                                                                                     
     Hist["g_GenAK4j12dPhi"]->Fill(deltaPhi( AK4JetsVect[0].phi() , AK4JetsVect[1].phi() ), EventWeight);
     //deltaR of two lepton                                                                                                                                                                                       
     Hist["g_GenAK4j12dR"]->Fill(deltaR( AK4JetsVect[0].phi(), AK4JetsVect[0].eta(), AK4JetsVect[1].phi(), AK4JetsVect[1].eta() ), EventWeight);
   }

   //std::cout<<"check 6"<<std::endl;

   if(Verbose) std::cout<<"here14"<<std::endl;
   
   //AK8
   Hist["g_GenAK8nJet"]->Fill(AK8JetsVect.size(), EventWeight);
   for(unsigned int i = 0; i < AK8JetsVect.size(); i++){
     reco::GenJet* jet = &(AK8JetsVect[i]);
     if (i>2) break;
     Hist[("g_GenAK8Jet"+std::to_string(i+1)+"pt").c_str()]->Fill(jet->pt(), EventWeight);
     Hist[("g_GenAK8Jet"+std::to_string(i+1)+"eta").c_str()]->Fill(jet->eta(), EventWeight);
     Hist[("g_GenAK8Jet"+std::to_string(i+1)+"phi").c_str()]->Fill(jet->phi(), EventWeight);
     Hist[("g_GenAK8Jet"+std::to_string(i+1)+"mass").c_str()]->Fill(jet->mass(), EventWeight);
     Hist[("g_GenAK8Jet"+std::to_string(i+1)+"pdgid").c_str()]->Fill(jet->pdgId(), EventWeight);
   }

   if(Verbose) std::cout<<"here15"<<std::endl;

   if (AK8JetsVect.size()>0 || AK8JetsVect.size()>=2){
     //deltaPhi of two lepton                                                                                                                                                                                 
     Hist["g_GenAK8j12dPhi"]->Fill(deltaPhi( AK8JetsVect[0].phi() , AK8JetsVect[1].phi() ), EventWeight);
     //deltaR of two lepton
     Hist["g_GenAK8j12dR"]->Fill(deltaR( AK8JetsVect[0].phi(), AK8JetsVect[0].eta(), AK8JetsVect[1].phi(), AK8JetsVect[1].eta() ), EventWeight);
   }

   if(Verbose) std::cout<<"here16"<<std::endl;

   // Gen MET
   reco::GenMET caloMET = theJetAnalyzer->FillCaloMetVector(iEvent);   
   reco::GenMET trueMET = theJetAnalyzer->FillTrueMetVector(iEvent); 
   
   Hist["a_genCaloMETpt"]->Fill(caloMET.pt(), EventWeight);
   Hist["a_genCaloMETphi"]->Fill(caloMET.phi(), EventWeight);
   Hist["a_genTrueMETpt"]->Fill(trueMET.pt(), EventWeight);
   Hist["a_genTrueMETphi"]->Fill(trueMET.phi(), EventWeight);

   if(Verbose) std::cout<<"here17"<<std::endl;

   // Fill number of events when MET > 200 GeV
   if ( trueMET.pt() > 200. ) Hist["a_nEvents"]->Fill(2.,EventWeight);
   // Fill number of events when MET > 200 GeV  
   if ( trueMET.pt() > 250. ) Hist["a_nEvents"]->Fill(3.,EventWeight);

   if(Verbose) std::cout<<"here18"<<std::endl;

   tree->Fill();
   
   //std::cout << "Filling finished" << std::endl;
   //std::cout << std::endl;
}


// ------------ method called once each job just before starting event loop  ------------
void 
LambdaAnalyzer::beginJob()
{
  tree=fs->make<TTree>("tree", "tree");
}

// ------------ method called once each job just after ending the event loop  ------------
void 
LambdaAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
LambdaAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(LambdaAnalyzer);
