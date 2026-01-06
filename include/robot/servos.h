#pragma once
#include <ESP32Servo.h>
#include "globals.h"

void moveServoSmooth(Servo&, int&, int, int duration=servoDuration, bool skip=false);
void moveall(int, int, int, int, bool interpolate=enableInterpolate, int duration=servoDuration, char* order=servoSequence);