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

#
# lame settings: joint stereo variable bitrate
#
LAME_SETTINGS="-m j -p -b 112 -B 320 -q 0 -V 0 --vbr-new"

#
# convert each file
#
for iTunesFile in *.m4a
do
   #
   # filenames (unicode!)
   #
   AtomFile="`echo \"$iTunesFile\" | sed 's/.m4a//'`.atom"
   mp3File="`echo \"$iTunesFile\" | sed 's/.m4a//'`.mp3"
   coverart="`echo \"$iTunesFile\" | sed 's/.m4a//'`_artwork_1.jpg"

   #
   # generate atom file with AtomicParsley and get the (most important) meta data
   #
   AtomicParsley "$iTunesFile" -t >"$AtomFile"
   ALBUM="`cat \"$AtomFile\" | grep '©alb' | sed 's/^.*contains: //'`"
   ARTIST="`cat \"$AtomFile\" | grep '©ART' | sed 's/^.*contains: //'`"
   ALBUMARTIST="`cat \"$AtomFile\" | grep 'aART' | sed 's/^.*contains: //'`"
   COMMENT="`cat \"$AtomFile\" | grep '©cmt' | sed 's/^.*contains: //'`"
   YEAR="`cat \"$AtomFile\" | grep '©day' | sed 's/^.*contains: //' | cut -b1-4`"
   TITLE="`cat \"$AtomFile\" | grep '©nam' | sed 's/^.*contains: //'`"
   GENRE="`cat \"$AtomFile\" | grep '©gen' | sed 's/^.*contains: //'`"
   GNRE="`cat \"$AtomFile\" | grep 'gnre' | sed 's/^.*contains: //'`"
   TRACK="`cat \"$AtomFile\" | grep 'trkn' | sed 's/^.*contains: //'`"
   DISK="`cat \"$AtomFile\" | grep 'disk' | sed 's/^.*contains: //'`"

   #
   # extract album artwork
   #
   AtomicParsley "$iTunesFile" -E

   #
   # do some clever stuff here ??
   #
   if [ "$GENRE" = "" ]
   then
      GENRE="$GNRE"
   fi
   if [ "$COMMENT" = "" ]
   then
      COMMENT=""
   fi

   #
   # AtomicParsley reports "n of m", lame expects "n/m"
   #
   if [ "$TRACK" != "" ]
   then
      TRACK="`echo \"$TRACK\" | sed 's/ of /\//'`"
   fi

   if [ "$DEBUG" = "yes" ]
   then
      echo "Convert [$iTunesFile] -> [$mp3File]"
      echo "  +Album        [$ALBUM]"
      echo "  +Artist       [$ARTIST]"
      echo "   Album Artist [$ALBUMARTIST]"
      echo "  +Comment      [$COMMENT]"
      echo "  +Year         [$YEAR]"
      echo "  +Title        [$TITLE]"
      echo "  +Genre        [$GENRE]"
      echo "  +Track        [$TRACK]"
      echo "   Disk         [$DISK]"
      echo "   "
   fi

   if [ "$DEBUG" = "yes" ]
   then
      echo "lame $LAME_SETTINGS $MP3_TAGS $mp3File"
   else
      rm -f track.wav track.m4a
      cp "$iTunesFile" track.m4a

      #
      # convert AAC audio to PCM (in RIFF/WAV container) 
      #
      #mplayer -vo null -ao pcm:file=track.wav -benchmark track.m4a
      avconv -i track.m4a track.wav

      #
      # encode to mp3, set ID3 tags (as complete as possible)
      #
      lame $LAME_SETTINGS --tl "$ALBUM" --ta "$ARTIST" --tc "$COMMENT" --ty "$YEAR" --tt "$TITLE" --tg "$GENRE" --tn "$TRACK" track.wav


      #
      # re-attach cover art
      #
      if [ -f "$coverart" ]
      then
          eyeD3 --add-image="$coverart":FRONT_COVER track.mp3
      fi

      #
      # convert the ID3v2 tags from UTF-8 to unicode (for iTunes), remove ID3v1 tags
      #
      mp3unicode --source-encoding UTF-8 --id3v1-encoding none --id3v2-encoding unicode track.mp3

      #
      # move result to original filename with .mp3 extension
      #
      mv track.mp3 "$mp3File"

      #
      # apply replaygain
      #
      #mp3gain -r "$mp3File"

      #
      # cleanup
      #
      rm -f track.wav track.m4a track.mp3 track.atom track_artwork_1.jpg "$AtomFile"
   fi

done
