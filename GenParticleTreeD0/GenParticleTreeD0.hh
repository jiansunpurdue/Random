#ifndef GenParticleTreeD0_H
#define GenParticleTreeD0_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <TFile.h>
#include <TH1F.h>
#include <TNamed.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TCanvas.h>

#define MAXPARTICLES 150000
#define nch 3

#define MAXGENDMESON 100

//typedef std::vector<trigger::TriggerObject> trigO;
using namespace std;

class GenParticleTreeD0 
{
    private:
        string filename;

        TFile* result;  //.. result output file name

        Int_t mult;
        Float_t genpt[MAXPARTICLES];
        Float_t geneta[MAXPARTICLES];
		Float_t genphi[MAXPARTICLES];
        Int_t genpdg[MAXPARTICLES];
        Int_t genchg[MAXPARTICLES];
        Int_t gensube[MAXPARTICLES];
        Int_t gensta[MAXPARTICLES];
        Int_t genmatchingID[MAXPARTICLES];
        Int_t gennMothers[MAXPARTICLES];
        Int_t genmotherIdx[MAXPARTICLES][200];
        Int_t gennDaughters[MAXPARTICLES];
        Int_t gendaughterIdx[MAXPARTICLES][200];

        float pthat;


    private: 
	    //.. forest tree used in the analyses
        TTree *genparticletree;

    private:
		//output trees
		TTree * gendmesontree;

		int ngend;
		double weight_pthat;
		float dpt[MAXGENDMESON];
		float deta[MAXGENDMESON];
		float dy[MAXGENDMESON];
		float dphi[MAXGENDMESON];
		int    dpdg[MAXGENDMESON];
		int    dnofdau[MAXGENDMESON];
		float pt_ddau[MAXGENDMESON][3];
		float eta_ddau[MAXGENDMESON][3];
		float phi_ddau[MAXGENDMESON][3];
		int    pdg_ddau[MAXGENDMESON][3];
		int    dnofmom[MAXGENDMESON];
		int    pdg_dmom[MAXGENDMESON];
		float pt_dmom[MAXGENDMESON];
		int    pdg_Bmom[MAXGENDMESON];
		float pt_Bmom[MAXGENDMESON];


    private:
        void Init(int startFile, int endFile, char *filelist);
        void write();
		void get_tree_info();
		void LoopOverEvt();
		void FindGenDmeson();

    public:
        GenParticleTreeD0();
        virtual ~GenParticleTreeD0();

        void LoopOverFile(int startFile, int endFile, char *filelist, int decaychannel);

};

#endif //  GenParticleTreeD0_H
