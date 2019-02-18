#!/bin/sh
#
# lame settings: joint stereo variable bitrate
#
LAME_SETTINGS="-m j -p -b 112 -B 320 -q 0 -V 0 --vbr-new"

#
# convert each file
#
for track in *.wav
do
   lame $LAME_SETTINGS "$track"
done
