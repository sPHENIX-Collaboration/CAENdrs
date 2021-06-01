#! /bin/bash

link=$1
if [ -z "$link" ]; then
    echo " Usage: $0 link_nr"
    exit
fi

serial=$(caen_client -d $link identify | grep Serial | awk '{print $NF}')

sn=$(printf %04d $serial)


ispeed=0
for speed in 5G 2G 1G ; do
    rm -f calib_${sn}_${speed}.dat
    for chip in {0..3} ; do 
	caen_client -d ${link} PrintCalibrationData $ispeed $chip >> calib_${sn}_${speed}.dat
    done
    ispeed=$(expr $ispeed + 1)
done


