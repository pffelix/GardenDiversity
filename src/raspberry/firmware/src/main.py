#!/usr/bin/env python

import recorder
import model

if __name__ == '__main__':

    # variables


    lat = 49.19
    lon = 11.1
    week = 40
    sensitivity = 0.9

    # runtime services

    # run once
    # Load model
    main_model = model.Model()
    main_recorder = recorder.Recorder()
    main_model.load_model()

    # run periodically

    main_recorder.audio_to_file()
    sig = main_recorder.audio_to_signal()
    chunks = main_recorder.split_signal(sig)
    detections = main_model.analyze_audio_data(chunks, lat, lon, week, sensitivity, main_recorder.overlap)

