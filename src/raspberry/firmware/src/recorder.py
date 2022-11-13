#!/usr/bin/env python

import alsaaudio
import time
import struct
import numpy as np


class Recorder:

    def __init__(self):

        self.seconds = 10
        self.fs = 48000
        self.overlap = 0.5
        self.card = 'dsnoop:CARD=sndrpii2scard,DEV=0'
        self.filename = "record.wav"

    def audio_to_file(self):

        # Open the file
        f = open(self.filename, 'wb')

        # Open the device in nonblocking capture mode. The last argument could
        # just as well have been zero for blocking mode. Then we could have
        # left out the sleep call in the bottom of the loop
        inp = alsaaudio.PCM(type=alsaaudio.PCM_CAPTURE, mode=alsaaudio.PCM_NONBLOCK, rate=self.fs, channels=2,
                            format=alsaaudio.PCM_FORMAT_S32_LE, periodsize=32, device=self.card)

        # The period size controls the internal number of frames per period.
        # The significance of this parameter is documented in the ALSA api.
        # For our purposes, it is sufficient to know that reads from the device
        # will return this many frames. Each frame being 2 bytes long.
        # This means that the reads below will return either 320 bytes of data
        # or 0 bytes of data. The latter is possible because we are in nonblocking
        # mode.

        print("recording start")
        total = 0
        while total < self.seconds * self.fs:
            l, data = inp.read()
            if l:
                total += l
                f.write(data)
                time.sleep(.001)
        print("recording stop")

    def audio_to_signal(self, convertofloat=True):

        # Open the device in nonblocking capture mode. The last argument could
        # just as well have been zero for blocking mode. Then we could have
        # left out the sleep call in the bottom of the loop
        inp = alsaaudio.PCM(type=alsaaudio.PCM_CAPTURE, mode=alsaaudio.PCM_NONBLOCK, rate=self.fs, channels=2,
                            format=alsaaudio.PCM_FORMAT_S32_LE, periodsize=32, device=self.card)

        # The period size controls the internal number of frames per period.
        # The significance of this parameter is documented in the ALSA api.
        # For our purposes, it is sufficient to know that reads from the device
        # will return this many frames. Each frame being 2 bytes long.
        # This means that the reads below will return either 320 bytes of data
        # or 0 bytes of data. The latter is possible because we are in nonblocking
        # mode.

        print("recording start")
        total = 0
        sig = []
        while total < self.seconds * self.fs:
            l, data = inp.read()
            if l:
                total += l
                data = struct.unpack("i"*(len(data)//4), data)
                for i in range(len(data) // 2):
                    sig_i = (data[2 * i] + data[2 * i + 1]) // 65536
                    sig_i = sig_i / 32768.0 if convertofloat is True else sig_i
                    sig.append(sig_i)
                time.sleep(.001)
        print("recording stop")
        return sig

    def split_signal(self, sig):

        # Split signal with overlap
        model_seconds = 3.0
        minlen = 1.5
        sig_splits = []
        for i in range(0, len(sig), int((model_seconds - self.overlap) * self.fs)):
            split = sig[i:i + int(model_seconds * self.fs)]

            # End of signal?
            if len(split) < int(minlen * self.fs):
                break

            # Signal chunk too short? Fill with zeros.
            if len(split) < int(self.fs * model_seconds):
                temp = np.zeros((int(self.fs * model_seconds)))
                temp[:len(split)] = split
                split = temp

            sig_splits.append(split)

        return sig_splits
