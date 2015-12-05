#!/bin/sh

#need to know total number of events in the file first
python submit_Ntuple_batch_cfg.py -i root://eoscms//eos/cms//store/group/phys_heavyions/velicanu/forest/HIRun2015/HIExpressPhysics/Merged/HIForestExpress_run262656.root -n 50000 -j 8 -o /store/group/phys_heavyions/jisun/Dntuple2015run/PbPbExpress/run262837
