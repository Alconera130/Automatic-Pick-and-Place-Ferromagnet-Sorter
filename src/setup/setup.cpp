#include <Arduino.h>
#include <ESP32Servo.h>
#include "setup/setup.h"
#include "globals.h"

float askFloat(String prompt) { 
    Serial.print(prompt); 
    while (!Serial.available()) 
    delay(10); 
    String input = Serial.readStringUntil('\n'); 
    input.trim(); Serial.println(); return input.toFloat(); 
} 

int askInt(String prompt) { 
    Serial.print(prompt); 
    while (!Serial.available()) delay(10);
    String input = Serial.readStringUntil('\n');
    input.trim(); Serial.println();
    return input.toInt();
}

bool askBool(String prompt) {
    Serial.print(prompt);
    while (!Serial.available()) delay(10);
    String input = Serial.readStringUntil('\n');
    input.trim(); input.toLowerCase();
    bool result = input == "true" || input == "1" ? true : false;
    Serial.println();
    return result;
}

void variableSetup() { 
    Serial.println("Do you want to continue with the calibration? [Y/N]");
    while(!Serial.available()) delay(10);
    String input = Serial.readStringUntil('\n');
    input.trim(); input.toUpperCase();
    
    if (input == "Y") { 
        Serial.println("\n--- Camera Calibration Setup ---");
        Serial.println("Enter the following values (press Enter after each):");

        mm_per_px = askFloat("mm_per_px: defaut -> 0.9");
        x_ref = askInt("x_ref (origin X in pixels): default -> 0");
        y_ref = askInt("y_ref (origin Y in pixels): default -> 0");
        robot_offset_x = askFloat("robot_offset_x (mm): default -> 30");
        robot_offset_y = askFloat("robot_offset_y (mm): default -> 55");
        DESTINATION_X = askFloat("DESTINATION_X (drop off X mm): default -> -50");
        DESTINATION_Y = askFloat("DESTINATION_Y (drop off Y mm): default -> 10");
        servoDuration = askInt("servoDuration (servo movement speed): default -> 1000");
        enableInterpolate = askBool("enableInterpolate (arm servos move at the same time): default -> true");
        confidence = askFloat("confidence (detection threshold 0.0 - 1.0): default -> 0.6");
    } 
    
    Serial.println("\n✅ Settings confirmed:");
    Serial.printf("mm_per_px = %.3f\n", mm_per_px);
    Serial.printf("x_ref = %d\n", x_ref);
    Serial.printf("y_ref = %d\n", y_ref);
    Serial.printf("robot_offset_x = %.1f mm\n", robot_offset_x);
    Serial.printf("robot_offset_y = %.1f mm\n", robot_offset_y);
    Serial.printf("DESTINATION_X = %.1f mm\n", DESTINATION_X);
    Serial.printf("DESTINATION_Y = %.1f mm\n", DESTINATION_Y);
    Serial.printf("servoDuration = %d\n", servoDuration);
    Serial.printf("enableInterpolate = %s\n", enableInterpolate ? "true" : "false");
    Serial.printf("confidence = %.2f\n", confidence);
    Serial.println("-------------------------------");

    delay(500);
    Serial.println("Attaching servos...");
    delay(1000);

    servoA.attach(19);
    servoB.attach(20);
    servoC.attach(21);
    servoD.attach(47);
    servoE.attach(48);

    delay(100);

    servoA.write(posA);
    servoB.write(posB);
    servoC.write(posC);
    servoD.write(posD);
    servoE.write(posE);
}
