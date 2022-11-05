#!/usr/bin/env python

## main.py
##
## This is an example of a simple sound capture script.
##
## The script opens an ALSA pcm forsound capture. Set
## various attributes of the capture, and reads in a loop,
## writing the data to standard out.
##
## To test it out do the following:
## python recordtest.py out.raw # talk to the microphone
## aplay -r 8000 -f S16_LE -c 1 out.raw


# Footnote: I'd normally use print instead of sys.std(out|err).write,
# but we're in the middle of the conversion between python 2 and 3
# and this code runs on both versions without conversion

import time
import alsaaudio

if __name__ == '__main__':

    card = 'dsnoop:CARD=sndrpii2scard,DEV=0'

    f = open("test2.wav", 'wb')

    # Open the device in nonblocking capture mode. The last argument could
    # just as well have been zero for blocking mode. Then we could have
    # left out the sleep call in the bottom of the loop
    inp = alsaaudio.PCM(type=alsaaudio.PCM_CAPTURE, mode=alsaaudio.PCM_NONBLOCK, rate=48000, channels=2,
                        format=alsaaudio.PCM_FORMAT_S16_LE, periodsize=32, device=card)

    # The period size controls the internal number of frames per period.
    # The significance of this parameter is documented in the ALSA api.
    # For our purposes, it is suficcient to know that reads from the device
    # will return this many frames. Each frame being 2 bytes long.
    # This means that the reads below will return either 320 bytes of data
    # or 0 bytes of data. The latter is possible because we are in nonblocking
    # mode.

    total = 0
    seconds = 5
    while total < seconds * 48000:
        l, data = inp.read()
        if l:
            total += l
            f.write(data)
            time.sleep(.001)