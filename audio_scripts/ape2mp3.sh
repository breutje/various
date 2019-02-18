#!/bin/sh
#
# lame settings: joint stereo variable bitrate
#
LAME_SETTINGS="-m j -p -b 112 -B 320 -q 0 -V 0 --vbr-new"

#
# convert each file
#
for track in *.ape
do
   RIFF="`echo \"$track\" | sed 's/.flac//'`.wav"
   avconv -i "$track" "$RIFF"
   lame $LAME_SETTINGS "$RIFF"
   rm "$RIFF"
done
