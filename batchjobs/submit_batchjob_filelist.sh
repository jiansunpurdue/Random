#!/bin/sh

#need to know total number of events in the file first
python submit_filelist_Ntuple_batch_cfg.py -i test_2.list -o /afs/cern.ch/work/j/jisun/HIN/sl6/Forest_755/CMSSW_7_5_5_patch4/src/DntupleRunII/CodeForNtupleProduction/test_2 -c 0 -q '8nm'
