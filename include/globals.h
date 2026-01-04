#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>
#include <WebServer.h>

#include "esp_camera.h"

// ===== SERVER =====
extern WebServer server;

// ===== SERVOS =====
extern Servo servoA, servoB, servoC, servoD, servoE;
extern int posA, posB, posC, posD, posE;
extern int offsetA, offsetB, offsetC, offsetD;
extern int gripperAngle;

extern int servoDuration;
extern bool enableInterpolate;
extern char* servoSequence;

// ===== ROBOT =====
extern float DESTINATION_X, DESTINATION_Y;
extern double LENGTH;
extern int PICKUP_OFFSET;

// ===== CAMERA FLAGS =====
extern bool hmirror_enabled;
extern bool vflip_enabled;

// ===== HALL SENSORS =====
extern int hallEffectSensor1;
extern int hallEffectSensor2;

// ===== CALIBRATION =====
extern float mm_per_px;
extern int x_ref, y_ref;
extern float robot_offset_x, robot_offset_y;
extern float confidence;

// ===== OBJECTS =====
struct Object {
    String label;
    float x_mm;
    float y_mm;
    float distance;
    bool handled;
};

// ===== CAMERA =====
extern camera_config_t camera_config;
extern uint8_t snapshot_buf[];

extern unsigned long lastInferenceTime;

#define EI_CLASSIFIER_TFLITE_ENABLE_CUDA 0
#define EI_CLASSIFIER_DEBUG 0