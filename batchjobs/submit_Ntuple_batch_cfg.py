#!/usr/bin/env python
import os,sys
import optparse
import commands
import time
import re

#command line configuration
usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-q', '--queue'      ,dest='queue'  ,help='batch queue'          ,default='1nh')
#parser.add_option('-q', '--queue'      ,dest='queue'  ,help='batch queue'          ,default='8nh')
#parser.add_option('-q', '--queue'      ,dest='queue'  ,help='batch queue'          ,default='')
parser.add_option('-j', '--jobs'       ,dest='jobs'   ,help='number of jobs'       ,default=1,    type=int)
parser.add_option('-i', '--inputF'      ,dest='inputF' ,help='input file list'     ,default='express.list', type='string')
parser.add_option('-n', '--nevts'      ,dest='nevts'  ,help='number of evetns/job' ,default=100,  type=int)
parser.add_option(      '--proxy'      ,dest='proxy'  ,help='proxy to be used'     ,default=None, type='string')
parser.add_option('-o', '--output'     ,dest='output' ,help='output directory'     ,default='/store/group/phys_heavyions/mverweij/Express/Forest')

(opt, args) = parser.parse_args()

#prepare working directory
cmsswBase=os.environ['CMSSW_BASE']
workBase=os.getcwd()
jobsBase='%s/FARM%s'%(workBase,time.time())
os.system('mkdir -p %s'%jobsBase)

#init a new proxy if none has been passed
if opt.proxy is None:
    print 'Initiating a new proxy'
    os.system('voms-proxy-init --voms cms --valid 72:00')
    os.system('cp /tmp/x509up_u`id -u \`whoami\`` %s/proxyforprod' % workBase)
    print 'Production proxy is now available in %s/proxyforprod (valid for 72h)' % workBase

#loop over the required number of jobs
jobCounter=0

EvtPerJob=opt.nevts
NJOB=opt.jobs
inputfilename=os.path.basename(opt.inputF)

for jobCounter in range(0, NJOB):
	outdir = '%s' % (opt.output)
	startevent=EvtPerJob*jobCounter
	endevent=EvtPerJob*(jobCounter+1)
	outputfile='ntD_%s_Evt_%dto%d.root'%(inputfilename, startevent, endevent)
	#create bash script to be submitted
	scriptFile = open('%s/runJob_%d.sh'%(jobsBase,jobCounter), 'w')
	scriptFile.write('#!/bin/bash\n')
	scriptFile.write('export X509_USER_PROXY=%s/proxyforprod\n' % workBase)
	scriptFile.write('cd %s/src\n'%cmsswBase)
	scriptFile.write('eval `scram r -sh`\n')
	scriptFile.write('cd -\n')
	scriptFile.write('cp %s/exec_test.sh %s \n' % (workBase,jobsBase))
	scriptFile.write('cp %s/exec_test.sh .\n' % workBase)
	scriptFile.write('cp %s/loop.C %s \n' % (workBase,jobsBase))
	scriptFile.write('cp %s/loop.C .\n' % workBase)
	scriptFile.write('cp %s/loop.h %s \n' % (workBase,jobsBase))
	scriptFile.write('cp %s/loop.h .\n' % workBase)
	scriptFile.write('./exec_test.sh %s %s %d %d\n' % (opt.inputF, outputfile, startevent, endevent) )
	scriptFile.write('cmsMkdir %s\n' % outdir)
	scriptFile.write('ls\n')
	scriptFile.write('cmsStage -f  %s %s/%s\n' % (outputfile, outdir, outputfile) )
	scriptFile.write('rm %s\n' % (outputfile))
	scriptFile.close()
	#preare to run it
	os.system('chmod u+rwx %s/runJob_%d.sh' % (jobsBase,jobCounter))
	#submit it to the batch or run it locally
	if opt.queue=='':
		print 'Job #%d will run locally' % jobCounter
		os.system('%s/runJob_%d.sh' % (jobsBase,jobCounter) )
	else:
		print 'Job #%d will run remotely' % jobCounter
		os.system("bsub -q %s -R \"swp>1000 && pool>30000\" -J FOREST_%d \'%s/runJob_%d.sh\'" % (opt.queue,jobCounter,jobsBase,jobCounter) )
	jobCounter+=1

