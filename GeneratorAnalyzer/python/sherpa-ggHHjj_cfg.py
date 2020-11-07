import os
import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
#mylist = FileUtils.loadListFromFile('---')

process = cms.Process("lambda")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.option = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring(
                #'file:/lustre/cmswork/hoh/GenStudy/ggHHjets_18/CMSSW_10_2_5/src/madgraphs/step1.root',
                #'file:$CMSSW_BASE/src/Sherpa-cmssw/Generator/ggHH2j_benchmark_2/sherpa_Run.root',
                #'file:$CMSSW_BASE/src/Sherpa-cmssw/Generator/ggHH_temp/ggHH2j_benchmark_shower/sherpa_Run.root',
                
                'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_10601204_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_11147341_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_12363099_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_15433042_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_17417123_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_17783338_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_17971087_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_18814367_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_2294202_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_26542034_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_26807679_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_27670248_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_28845897_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_29113899_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_30911405_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_34827962_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_35844622_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_38590525_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_40716182_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_40822238_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_41708135_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_4569953_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_5276496_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_6394232_gensim.root', 'file:/lustre/cmswork/hoh/NANO/whss_signal_prod_v0/nano/v1/ggHH2j_gensim/ggHH2j_slc6_amd64_gcc630_CMSSW_9_3_16_8174726_gensim.root'
        )
)

process.lamb = cms.EDAnalyzer("LambdaAnalyzer",
                              genSet = cms.PSet(
                                                genProduct = cms.InputTag('generator'),
                                                lheProduct = cms.InputTag('externalLHEProducer'),
                                                genParticles = cms.InputTag('genParticles'),
                                                pdgId = cms.vint32(1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 16, 21, 23, 24, 25, 52, 55, 54, 1023), #52 DM; 55 Zp; 54 hs 
                                                ),
                              genjetSet = cms.PSet(
                                                ak4genJets = cms.InputTag('ak4GenJets'),
                                                ak8genJets = cms.InputTag('ak8GenJets'),
                                                genmetCalo = cms.InputTag('genMetCalo'),
                                                genmetTrue = cms.InputTag('genMetTrue'),
                                                ),
                              histFile = cms.string('%s/src/EDManalyzer/LambdaAnalyzer/data/HistList.dat' % os.environ['CMSSW_BASE']),
                              verbose = cms.bool(False),
                              )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("Hist_madgraph_ggHHjj.root"),
                                   #fileName = cms.string("Hist_sherpa_ggHHjj.root"),
                                   #fileName = cms.string("Hist_sherpa_ggHH012j.root"),
                                   closeFileFast = cms.untracked.bool(True)
                                    )

process.p = cms.Path(process.lamb)
