#!/bin/sh

if [ "$#" -ne 4 ]; then
	echo "Wrong number of parameters. 4 expected, passed $#"
	exit
fi

INFILELIST=$1
isPbPb=$2
FilePerJob=$3
OUTPUTDIR=$4
TotalFiles=$(wc -l < "$INFILELIST")
Njobs=$((TotalFiles/FilePerJob+1))

echo "INFILELIST: $INFILELIST"
echo "Total number of file: $TotalFiles, FilePerJob: $FilePerJob"
echo "$Njobs jobs will be submitted"

rm loop.exe
g++ loop.C $(root-config --cflags --libs) -Wall -O2 -o loop.exe

if [ ! -d $OUTPUTDIR ]; then
    mkdir $OUTPUTDIR
fi

for ((count=1; count <= $Njobs; count++))
do
	echo "Job $count"

	#decide start file and end file
	STARTFILE=$(((count-1)*FilePerJob))
	ENDFILE=$((count*FilePerJob))
	if [ $ENDFILE -gt $TotalFiles ]; then
		ENDFILE=$TotalFiles
	fi
	echo "from $STARTFILE to $ENDFILE"

# make the condor file
cat > condor_${STARTFILE}to${ENDFILE}.job <<EOF
#======================================================================
#
# condor_submit condor_job
#
#======================================================================
universe             = vanilla
executable           = exec_condorfilelist.sh 
Arguments = $INFILELIST $isPbPb $STARTFILE $ENDFILE $OUTPUTDIR
initialdir           = /home/sun229/DmesonAna/Run2015Ana/CMSSW_7_5_8_patch2/src/Bfinder/Bfinder/Dntuple 
should_Transfer_Files = NO
Requirements   = Arch == "X86_64" && TARGET.UidDomain == "rcac.purdue.edu" && TARGET.Machine != "airplay-render1.rcac.purdue.edu" && TARGET.Machine != "airplay-render2.rcac.purdue.edu"
#======================================================================
output               = condor_${STARTFILE}to${ENDFILE}.out
error                = condor_${STARTFILE}to${ENDFILE}.err
log                  = condor_${STARTFILE}to${ENDFILE}.log
#======================================================================
Getenv               = True
#======================================================================

#======================================================================
+CMSJob = True
#+LENGTH="SHORT"

queue
EOF

	condor_submit condor_${STARTFILE}to${ENDFILE}.job
	echo "condor_${STARTFILE}to${ENDFILE}.job submitted"

done
