#include <fstream>
#include <iostream>
#include <vector>
#include "TH1F.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TBranch.h"
#include "TMath.h"
#include <TF1.h>
#include <TLorentzVector.h>

#include "GenParticleTreeD0.hh"
#include <iomanip>

#define MASSD0 1.8645  //value from pythia

////for D0 Hydjet samples
#define NPTHATBIN 5
int MCentries[NPTHATBIN] = { 19240, 19321, 18992, 20010, 22437};
int pthatbin[NPTHATBIN+1] = { 0, 15, 30, 50, 80, 1000};
double pthatweight_xsection[NPTHATBIN+1] = {41.30, 0.2035, 1.087E-2, 1.014E-03, 1.004E-04, 1.756E-15};
double filtereff[NPTHATBIN+1] = { 0.01194, 0.09132, 0.12752, 0.15206, 0.1694, 0.0945};

//
GenParticleTreeD0::GenParticleTreeD0()
{
}

//
GenParticleTreeD0::~GenParticleTreeD0()
{
	//    delete filename;
	delete result;

}
//
void GenParticleTreeD0::Init(int startFile, int endFile, char *filelist)
{//.. called right after constructor 
	char * outfilename = new char[1000];
	sprintf(outfilename, "GenParticleTreeD0_%s_%dTo%d.root",  filelist, startFile, endFile);
	result = new TFile(outfilename, "recreate");

	//

	genparticletree = 0;

    	gendmesontree = new TTree("gendmesontree","gendmesontree");
    	gendmesontree->Branch("ngend",&ngend,"ngend/I");
    	gendmesontree->Branch("pthat",&pthat, "pthat/F");
    	gendmesontree->Branch("weight_pthat",&weight_pthat, "weight_pthat/D");
    	gendmesontree->Branch("dpt", dpt, "dpt[ngend]/F");
    	gendmesontree->Branch("deta", deta, "deta[ngend]/F");
		gendmesontree->Branch("dy", dy, "dy[ngend]/F");
    	gendmesontree->Branch("dphi", dphi, "dphi[ngend]/F");
    	gendmesontree->Branch("dpdg", dpdg, "dpdg[ngend]/I");
    	gendmesontree->Branch("pt_ddau", pt_ddau, "pt_ddau[ngend][3]/F");
    	gendmesontree->Branch("eta_ddau", eta_ddau, "eta_ddau[ngend][3]/F");
    	gendmesontree->Branch("phi_ddau", phi_ddau, "phi_ddau[ngend][3]/F");
    	gendmesontree->Branch("pdg_ddau", pdg_ddau, "pdg_ddau[ngend][3]/I");
		gendmesontree->Branch("dnofmom", dnofmom, "dnofmom[ngend]/I");
		gendmesontree->Branch("pdg_dmom", pdg_dmom, "pdg_dmom[ngend]/I");
		gendmesontree->Branch("pt_dmom", pt_dmom, "pt_dmom[ngend]/F");
		gendmesontree->Branch("pdg_Bmom", pdg_Bmom, "pdg_Bmom[ngend]/I");
		gendmesontree->Branch("pt_Bmom", pt_Bmom, "pt_Bmom[ngend]/F");

}


void GenParticleTreeD0::get_tree_info()
{
        genparticletree->SetBranchAddress("mult", &mult);
    	genparticletree->SetBranchAddress("pt", genpt);
    	genparticletree->SetBranchAddress("eta", geneta);
    	genparticletree->SetBranchAddress("phi", genphi);
    	genparticletree->SetBranchAddress("pdg", genpdg);
    	genparticletree->SetBranchAddress("chg", genchg);
    	genparticletree->SetBranchAddress("matchingID", genmatchingID);
    	genparticletree->SetBranchAddress("nMothers", gennMothers);
    	genparticletree->SetBranchAddress("motherIdx", genmotherIdx);
    	genparticletree->SetBranchAddress("nDaughters", gennDaughters);
    	genparticletree->SetBranchAddress("daughterIdx", gendaughterIdx);
    	genparticletree->SetBranchAddress("sta", gensta);
}
//
//
void GenParticleTreeD0::LoopOverFile(int startFile, int endFile, char *filelist, int decaychannel) 
{//.. loop over files in the "*filelist" from startFile to endFile in sequence
	//

	Init(startFile, endFile, filelist);

	//
	ifstream file_stream(filelist);
	int ifile = 0;
	while (file_stream >> filename) {

		ifile++;
		if(ifile-1 >= endFile) {
			break;
		} else if(ifile-1 < startFile) {
			continue;
		}


		cout << "file  = " << ifile<<" name = "<<filename <<endl;

		if (!TFile::Open(filename.c_str()))   { cout << " fail to open file" << endl; continue;}

		TFile *f = TFile::Open(filename.c_str());

		genparticletree = (TTree*)f->Get("HiGenParticleAna/hi");

		get_tree_info();

		LoopOverEvt();

		f->Close();
	}

	write();
}

void GenParticleTreeD0::LoopOverEvt()
{
	for(int entry = 0; entry<genparticletree->GetEntries(); entry++) 
		//  for(int entry = 0; entry<100; entry++)
	{
		genparticletree->GetEntry(entry);

		    FindGenDmeson();
	     	gendmesontree->Fill();
	}
}

void GenParticleTreeD0::FindGenDmeson()
{
	ngend = 0;
	for( int particle = 0; particle < mult; particle++ )
	{
		if( abs(genpdg[particle]) == 421 ) //D0
		{

//			if( geneta[particle] < -2.0 || geneta[particle] > 2.0 )  continue;
			if( gennDaughters[particle] != 2 )   continue;
			if( gensta[particle] != 2 )  continue;
			int dau1 = gendaughterIdx[particle][0];
			int dau2 = gendaughterIdx[particle][1];

			if( !( ( abs(genpdg[dau1]) == 321 && abs(genpdg[dau2]) == 211 ) || ( abs(genpdg[dau1]) == 211 && abs(genpdg[dau2]) == 321 ) ) )   continue;
			if( genchg[dau1] * genchg[dau2]  > 0 ) continue;

			TLorentzVector gend0;
			gend0.SetPtEtaPhiM(genpt[particle], geneta[particle], genphi[particle], MASSD0);

			dpt[ngend] = genpt[particle];
			deta[ngend] = geneta[particle];
			dy[ngend] = gend0.Rapidity();
			dphi[ngend] = genphi[particle];
			dpdg[ngend] = genpdg[particle];
			dnofdau[ngend] = gennDaughters[particle];

			pt_ddau[ngend][0] = genpt[dau1];
			pt_ddau[ngend][1] = genpt[dau2];
			pt_ddau[ngend][2] = -999;

			eta_ddau[ngend][0] = geneta[dau1];
			eta_ddau[ngend][1] = geneta[dau2];
			eta_ddau[ngend][2] = -999;

			phi_ddau[ngend][0] = genphi[dau1];
			phi_ddau[ngend][1] = genphi[dau2];
			phi_ddau[ngend][2] = -999;

			pdg_ddau[ngend][0] = genpdg[dau1];
			pdg_ddau[ngend][1] = genpdg[dau2];
			pdg_ddau[ngend][2] = -999;

			dnofmom[ngend] = gennMothers[particle];
			
			if ( gennMothers[particle] == 1 )
			{
				 pdg_dmom[ngend] = genpdg[genmotherIdx[particle][0]];
				 pt_dmom[ngend] = genpt[genmotherIdx[particle][0]];
			}
			else
			{
				pdg_dmom[ngend] = -999;
				pt_dmom[ngend] = -999;
			}

            // decide if the D0 is from B feed down
			int pdg_Bmom_temp = -999;
			float pt_Bmom_temp = -999;
			int motherindex = -999;
			int daughterindex = particle;
			//loop up the decay chain to see if there is B meson
			while( gennMothers[daughterindex] == 1 && pt_Bmom_temp < 0 )
			{
				motherindex = genmotherIdx[daughterindex][0];
				if( ( TMath::Abs( genpdg[motherindex] ) > 500 && TMath::Abs( genpdg[motherindex] ) < 600 ) || ( TMath::Abs( genpdg[motherindex] ) > 5000 && TMath::Abs( genpdg[motherindex] ) < 6000 ) )   //decide if there is B mesons or B baryons in the decay chain
				{
					pt_Bmom_temp = genpt[motherindex];
					pdg_Bmom_temp = genpdg[motherindex];
				}
				daughterindex = motherindex;
			}

			pdg_Bmom[ngend] = pdg_Bmom_temp;
			pt_Bmom[ngend] = pt_Bmom_temp;

			ngend++;
		}
		else if ( abs(genpdg[particle]) == 413 )
		{

		}
	}

}

//
void GenParticleTreeD0::write()
{//.. results 
	result->cd();
	gendmesontree->Write();
	result->Close();
}
