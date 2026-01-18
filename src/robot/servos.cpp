#include <Arduino.h>
#include "globals.h"
#include "robot/servos.h"

void moveServoSmooth(Servo &servo, int &currentPos, int targetPos, bool skip) {
    if (!skip) targetPos = constrain(targetPos, 0, 180);

    int step = (currentPos < targetPos) ? 1 : -1;

    while (currentPos != targetPos) {
        currentPos += step;
        servo.write(currentPos);
        delay(10);
    }
}

void moveall(int targetA, int targetB, int targetC, int targetD, bool interpolate, int duration, char* order) {
    targetA = constrain(targetA + offsetA, 0, 180);
    targetB = constrain(180 - (targetB + offsetB), 0, 180);
    targetC = constrain(targetC + 90 + offsetC, 0, 180);
    targetD = constrain(targetD + offsetD, 0, 180);

    int steps = 100;
    float stepDelay = (float)duration / steps;

    if (interpolate) {
        float startA = posA, startB = posB, startC = posC, startD = posD;
        float deltaA = targetA - startA;
        float deltaB = targetB - startB;
        float deltaC = targetC - startC;
        float deltaD = targetD - startD;

        for (int i = 0; i <= steps; i++) {
            float t = (float)i / steps;

            posA = startA + deltaA * t;
            posB = startB + deltaB * t;
            posC = startC + deltaC * t;
            posD = startD + deltaD * t;

            servoA.write(posA);
            servoB.write(posB);
            servoC.write(posC);
            servoD.write(posD);

            delay((int)stepDelay);
        }

        posA = (int)round(targetA);
        posB = (int)round(targetB);
        posC = (int)round(targetC);
        posD = (int)round(targetD);
    } else {
        for (size_t i = 0; i < strlen(order); i++) {
            char servoID = order[i];
            switch (servoID) {
                case '1':
                    moveServoSmooth(servoA, posA, targetA);
                    break;
                case '2':
                    moveServoSmooth(servoB, posB, targetB);
                        break;
                case '3':
                    moveServoSmooth(servoC, posC, targetC);
                    break;
                case '4':
                    moveServoSmooth(servoD, posD, targetD);                        break;
                    break;
                default:
                    break;
            }
        }
    }
}
