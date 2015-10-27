#include "GenParticleTreeD0.cc"

//decaychannel 2 is D0

void run_GenParticleTreeD0(int startFile, int endFile, char *filelist, int decaychannel)
{
    GenParticleTreeD0 *a = new GenParticleTreeD0;

    a->LoopOverFile(startFile, endFile, filelist, decaychannel);

    //delete a; 
}
