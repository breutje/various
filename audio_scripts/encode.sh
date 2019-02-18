#!/bin/sh
#
# iTunes2mp3
#
# Unfortunately, my car stereo does not play itunes m4a files, only mp3 CD-ROMs.
# lame solution: convert mp4/aac to mp3 ;-). There is some loss as we re-encode
# AAC to MPEG Layer III audio, but using a good encoder with reasonable settings
# the output is acceptable (i.e. I cannot hear the difference). It does not copy
# the lyrics, but my car stereo can't display them anyway....
#
# Requirements: mplayer, AtomicParsley, mp3unicode, eyeD3 and Lame
#
# Copyright © 2010 Bert Reuling
#
#
DEBUG="no"
FILENUMBER="0"
EXT="ogg"

#
# lame settings: joint stereo variable bitrate
#
LAME_SETTINGS="-m j -p -b 112 -B 320 -q 0 -V 0 --vbr-new"

#
# convert each file
#
for track in *.${EXT}
do
   #
   # encode to mp3
   #
   cp "$track" track.${EXT}
   mplayer -vo null -ao pcm:file=track.wav -benchmark track.${EXT}
   lame $LAME_SETTINGS track.wav

   name="`basename \"$track\" .$EXT`"
   mv track.mp3 "${name}.mp3"
   rm track.wav
done
