#!/bin/bash
	#1508
#if [ $# -ne 1 ]
#then
#  echo "Usage: `basename $0` ilename"
#  exit $WRONG_ARGS
#fi

arg1=$1
command_type=${arg1::1}


if [ $command_type == - ]
then
	FILE_PATH=$0
	ARGS=$1
else
	FILE_PATH=$1
	ARGS=$2
fi

#echo $FILE_PATH

FIND_STR="user return code = 0"



SCRIPT=$(readlink -f "$0")
SCRIPT_PATH=`dirname "$SCRIPT"`
ABS_PATH=$SCRIPT_PATH
MANIFEST_NAME=data/manifest/SQLite.manifest
NVRAM_NAME=data/nvram/SQLite.nvram

absPath=$(readlink -f "$(dirname "$FILE_PATH")")/$(basename "$FILE_PATH")

#removing old manifest and nvram files
rm -f data/manifest/*.manifest data/nvram/*.nvram data/*dat

sed s@{ABS_PATH}@$ABS_PATH/@g data/manifest/SQLite.manifest.template > "$MANIFEST_NAME"
#sed s@{ABS_PATH}@$ABS_PATH/@g data/nvram/SQLite.nvram.template > data/nvram/SQLite.nvram
	
CONTENT_LENGTH=$(stat -c %s "$absPath")

sed s@{FILE_PATH}@$absPath@ data/nvram/SQLite.nvram.template | \
sed s@{ARGS}@"$ARGS"@ | \
sed s@{CONTENT_LENGTH}@$CONTENT_LENGTH@ > "$NVRAM_NAME"


$ZVM_PREFIX/bin/zerovm "$MANIFEST_NAME" > result.dat

RESULT_FILENAME=result.dat

cat $RESULT_FILENAME | while read LINE; do 
	if [ "$LINE" = "$FIND_STR" ]
	then
		echo "copy output to input!!!"
		cp data/output.data data/input.data
	fi
done
cat result.dat
cat data/stdout.dat
