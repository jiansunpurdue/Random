#!/bin/sh

echo `hostname`
echo `date`
export SCRAM_ARCH=slc6_amd64_gcc491
source /apps/osg/cmssoft/cmsset_default.sh
export X509_USER_PROXY=/home/sun229/.myproxy
DIR="Bfinder/Bfinder/Dntuple"
cd /home/sun229/DmesonAna/Run2015Ana/CMSSW_7_5_8_patch2/src 
eval `scramv1 runtime -sh`
cd $DIR

INFILELIST=$1
isPbPb=$2
STARTFILE=$3
ENDFILE=$4
OUTPUTDIR=$5

echo "INFILELIST: $INFILELIST"
echo "STARTFILE: $STARTFILE, ENDFILE: $ENDFILE"

if [ ! -d $OUTPUTDIR ]; then
	mkdir $OUTPUTDIR
fi

fileCounter=-1

while read line
do
	fileCounter=$((fileCounter+1))
	if [ $fileCounter -lt $STARTFILE ] || [ $fileCounter -ge $ENDFILE ]; then
		continue
	fi
	echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
	echo "<<<<<<<<<<<<< NEW INPUTFILE >>>>>>>>>>>>>>>>>"
	echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
	echo "fileCounter: $fileCounter, inputfile: $line"

	Inputfile=$line
	Outputfile="$OUTPUTDIR/Dntuple_$(basename ${Inputfile})_${fileCounter}.root"
	echo "Outputfile: $Outputfile"
	./loop.exe $Inputfile $Outputfile
	
	echo "<<<<<<<<<<<<< DONE!!!!!!! <<<<<<<<<<<<<<<<<"
done<$INFILELIST
echo `date`
