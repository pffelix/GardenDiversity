#!/usr/bin/env python

import recorder
import modeler
import streamer

if __name__ == '__main__':

    # variables


    lat = 49.19
    lon = 11.1
    week = 40
    sensitivity = 1.0

    # runtime services

    # run once
    print("script started")
    # Load model
    main_modeler = modeler.Modeler()
    main_recorder = recorder.Recorder()
    main_streamer = streamer.Streamer()
    main_modeler.load_model()
    # main_recorder.audio_to_file()

    # run periodically
    iteration = 1
    while True:
        print("\niteration: " + str(iteration))
        sig = main_recorder.audio_to_signal()
        chunks = main_recorder.split_signal(sig)
        detections = main_modeler.analyze_audio_data(chunks, lat, lon, week, sensitivity, main_recorder.overlap)
        main_streamer.mqtt_publish(detections)
        iteration += 1

