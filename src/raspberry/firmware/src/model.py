#!/usr/bin/env python

import numpy as np
import time
import math
import operator
import tflite_runtime.interpreter as tflite
#from tensorflow import lite as tflite

class Model:

    def __init__(self):
        self.INTERPRETER = None
        self.INPUT_LAYER_INDEX = None
        self.OUTPUT_LAYER_INDEX = None
        self.MDATA_INPUT_INDEX = None
        self.CLASSES = None
        self.HUMAN_DETECTED = None

    def load_model(self):

        print('LOADING TF LITE MODEL...', end=' ')

        # Load TFLite model and allocate tensors.
        modelpath = './model/BirdNET_6K_GLOBAL_MODEL.tflite'
        self.INTERPRETER = tflite.Interpreter(model_path=modelpath, num_threads=4)
        self.INTERPRETER.allocate_tensors()

        # Get input and output tensors.
        input_details = self.INTERPRETER.get_input_details()
        output_details = self.INTERPRETER.get_output_details()

        # Get input tensor index
        self.INPUT_LAYER_INDEX = input_details[0]['index']
        self.MDATA_INPUT_INDEX = input_details[1]['index']
        self.OUTPUT_LAYER_INDEX = output_details[0]['index']

        # Load labels
        self.CLASSES = []
        labelspath = './model/labels.txt'
        with open(labelspath, 'r') as lfile:
            for line in lfile.readlines():
                self.CLASSES.append(line.replace('\n', ''))

        print('DONE!')

    def analyze_audio_data(self, chunks, lat, lon, week, sensitivity, overlap):

        detections = {}
        start = time.time()
        print('ANALYZING AUDIO...', end=' ', flush=True)

        # Convert and prepare metadata
        mdata = self.convert_metadata(np.array([lat, lon, week]))
        mdata = np.expand_dims(mdata, 0)

        # Parse every chunk
        pred_start = 0.0
        for c in chunks:

            # Prepare as input signal
            sig = np.expand_dims(c, 0)

            # Make prediction
            p = self.predict([sig, mdata], sensitivity)

            # print("PPPPP",p)

            # Save result and timestamp
            pred_end = pred_start + 3.0

            detections[str(pred_start) + ';' + str(pred_end)] = p

            pred_start = pred_end - overlap

        print('DONE! Time', int((time.time() - start) * 10) / 10.0, 'SECONDS')
    #    print('DETECTIONS:::::',detections)
        return detections

    def convert_metadata(self, m):

        # Convert week to cosine
        if m[2] >= 1 and m[2] <= 48:
            m[2] = math.cos(math.radians(m[2] * 7.5)) + 1
        else:
            m[2] = -1

        # Add binary mask
        mask = np.ones((3,))
        if m[0] == -1 or m[1] == -1:
            mask = np.zeros((3,))
        if m[2] == -1:
            mask[2] = 0.0

        return np.concatenate([m, mask])

    def custom_sigmoid(self, x, sensitivity=1.0):
        return 1 / (1.0 + np.exp(-sensitivity * x))

    def predict(self, sample, sensitivity):

        # Make a prediction
        self.INTERPRETER.set_tensor(self.INPUT_LAYER_INDEX, np.array(sample[0], dtype='float32'))
        self.INTERPRETER.set_tensor(self.MDATA_INPUT_INDEX, np.array(sample[1], dtype='float32'))
        self.INTERPRETER.invoke()
        prediction = self.INTERPRETER.get_tensor(self.OUTPUT_LAYER_INDEX)[0]

        # Apply custom sigmoid
        p_sigmoid = self.custom_sigmoid(prediction, sensitivity)

        # Get label and scores for pooled predictions
        p_labels = dict(zip(self.CLASSES, p_sigmoid))

        # Sort by score
        p_sorted = sorted(p_labels.items(), key=operator.itemgetter(1), reverse=True)

        return p_sorted



