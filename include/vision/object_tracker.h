#pragma once

#include <Arduino.h>
#include <vector>

extern float robotX;
extern float robotY;

struct DetectedObject {
    float x;
    float y;
    String label;

    DetectedObject(float x_ = -1, float y_ = -1, String lbl = "")
        : x(x_), y(y_), label(lbl) {}

    float distanceSquaredFromOrigin() const {
        return x*x + y*y;
    }
};

DetectedObject findClosestToOrigin(const std::vector<DetectedObject>& objects);
void pickAndPlace(float x, float y);
float pixelToMM_X(int x);
float pixelToMM_Y(int y);
void cameraToRobot(int cx, int cy, float &out_robotX_mm, float &out_robotY_mm);