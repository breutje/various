#!/bin/sh
#
# lame settings: joint stereo variable bitrate
#
LAME_SETTINGS="-m j -p -b 112 -B 320 -q 0 -V 0 --vbr-new"

#
# convert each file
#
for track in *.aif
do
   RIFF="`echo \"$track\" | sed 's/.aif//'`.wav"
   ffmpeg -i "$track" "$RIFF"
   lame $LAME_SETTINGS "$RIFF"
   rm "$RIFF"
done
