#include <Arduino.h>
#include <math.h>
#include "globals.h"
#include "vision/object_tracker.h"
#include "robot/servos.h"
#include "robot/kinematics.h"
#include <vector>

float robotX = 0.0;
float robotY = 0.0;

DetectedObject findClosestToOrigin(const std::vector<DetectedObject>& objects) {
    if (objects.empty()) return DetectedObject();

    const DetectedObject* closest = &objects[0];
    float minDist = objects[0].distanceSquaredFromOrigin();

    for (size_t i = 1; i < objects.size(); i++) {
        float dist = objects[i].distanceSquaredFromOrigin();
        if (dist < minDist) {
            minDist = dist;
            closest = &objects[i];
        }
    }

    return *closest;
}


void pickAndPlace(float x, float y) {
    Serial.printf("🤖 Picking at (%.1f, %.1f) mm...\n", x, y);
    delay(300);

    moveto(x, y, 40);  // MOVE ABOVE OBJECT
    delay(300);
    moveServoSmooth(servoD, posD, 90);  // OPEN GRIPPER
    delay(300);
    moveto(x, y, -20);  // DESCEND TO OBJECT
    delay(300);
    moveServoSmooth(servoD, posD, 70);  // CLOSE GRIPPER
    delay(300);
    moveto(x, y, 40);  // ELEVATE OBJECT
    delay(300);
    moveto(DESTINATION_X, DESTINATION_Y, 40);  // MOVE TO DESTINATION
    delay(300);
    moveto(DESTINATION_X, DESTINATION_Y, -20);  // DESCEND OBJECT
    delay(300);
    moveServoSmooth(servoD, posD, 90);  // OPEN GRIPPER
    delay(300);
    moveto(DESTINATION_X, DESTINATION_Y, 40);  // ASCEND ARM
    delay(300);
    moveall(90, 90, -60, 90, true);  // MOVE TO ORIGINAL POSITION
    delay(1000);

    bool ferrous = false;

    for (int i = 0; i < 100; i++) {
        // READ HALL EFFECT SENSORS
        int read1 = !digitalRead(hallEffectSensor1);
        int read2 = !digitalRead(hallEffectSensor2);

        if (read1 || read2) { ferrous = !ferrous; Serial.println("Ferrous object detected!"); break; }

        delay(10);
    }

    if (ferrous) { servoE.write(0); } else { servoE.write(180); }

    delay(1000);
    servoE.write(90);

    Serial.println("✅ Done.");
}

float pixelToMM_X(int x) { return (x - x_ref) * mm_per_px; }
float pixelToMM_Y(int y) { return (y - y_ref) * mm_per_px; }

void cameraToRobot(int cx, int cy, float &out_robotX_mm, float &out_robotY_mm) {
    float camToRobotX = pixelToMM_Y(cy);  // camera Y -> robot X
    float camToRobotY = pixelToMM_X(cx);  // camera X -> robot Y

    out_robotX_mm = camToRobotX + robot_offset_x;
    out_robotY_mm = camToRobotY + robot_offset_y;
}