#include <Arduino.h>
#include <math.h>
#include "globals.h"
#include "robot/kinematics.h"
#include "robot/servos.h"

const double PI_D = 3.14159265358979323846;

void moveto(double x, double y, double z, const char* order) {
    double angle_1 = atan2(y, x) * 180.0 / PI_D;

    double L = sqrt(x * x + y * y);
    double h = sqrt(z * z + L * L);

    double half_h_over_L = (L > 0.0001) ? (h / 2.0) / LENGTH : 0.0;
    half_h_over_L = min(1.0, max(-1.0, half_h_over_L));  // clamp

    double theta_1 = acos(half_h_over_L) * 180.0 / PI_D;
    double phi_1 = atan2(z, L) * 180.0 / PI_D;

    double angle_2 = phi_1 + theta_1;  // elbow
    double angle_3 = phi_1 - theta_1;  // shoulder

    moveall((int)round(angle_1), (int)round(angle_2), (int)round(angle_3), (int)posD, enableInterpolate, servoDuration, (char*)order);
}