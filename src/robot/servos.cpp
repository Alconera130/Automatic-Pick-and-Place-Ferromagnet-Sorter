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
    if (interpolate) {
        targetA = constrain(targetA + offsetA, 0, 180);
        targetB = constrain(180 - (targetB + offsetB), 0, 180);
        targetC = constrain(targetC + 90 + offsetC, 0, 180);
        targetD = constrain(targetD + offsetD, 0, 180);

        float startA = posA, startB = posB, startC = posC, startD = posD;
        float deltaA = targetA - startA;
        float deltaB = targetB - startB;
        float deltaC = targetC - startC;
        float deltaD = targetD - startD;

        int steps = 100;
        float stepDelay = (float)duration / steps;

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
        int tC = constrain(targetC + 90 + offsetC, 0, 180);
        int tA = constrain(targetA + offsetA, 0, 180);
        int tB = constrain(180 - (targetB + offsetB), 0, 180);
        int tD = constrain(targetD + offsetD, 0, 180);

        for (size_t i = 0; i < strlen(order); i++) {
            char servoID = order[i];
            switch (servoID) {
                case '1':
                    moveServoSmooth(servoC, posC, tC);
                    break;
                case '2':
                    moveServoSmooth(servoA, posA, tA);
                    break;
                case '3':
                    moveServoSmooth(servoB, posB, tB);
                    break;
                case '4':
                    moveServoSmooth(servoD, posD, tD);
                    break;
                default:
                    break;
            }
        }
    }
}
