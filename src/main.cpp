#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "globals.h"
#include "camera/camera_pins.h"
#include "camera/camera_runtime.h"
#include "wifi/wifi_manager.h"
#include "ei/ei_inference.h"
#include "setup/setup.h"

// ===== GLOBAL DEFINITIONS =====
WebServer server(80);

Servo servoA, servoB, servoC, servoD, servoE;

int posA=91, posB=91, posC=31, posD=91, posE=90;
int offsetA=-5, offsetB=5, offsetC=39, offsetD=0;

int servoDuration=1000;
bool enableInterpolate=true;
char* servoSequence="1234";

float DESTINATION_X=-50;
float DESTINATION_Y=10;
double LENGTH=80.0;
int PICKUP_OFFSET=-20;

bool hmirror_enabled=false;
bool vflip_enabled=false;

int hallEffectSensor1=1;
int hallEffectSensor2=2;

float mm_per_px=0.9;
int x_ref=0, y_ref=0;
float robot_offset_x=30, robot_offset_y=55;
float confidence=0.6;

uint8_t snapshot_buf[320*240*3];

camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_QVGA,
    .jpeg_quality = 12,
    .fb_count = 2,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

void setup() {
    Serial.begin(115200);
    wifiSetup();

    esp_err_t err = esp_camera_init(&camera_config); 
    if (err != ESP_OK) {
        Serial.printf("Failed to initialize Camera! Error: 0x%x\n", err);
    } else {
        Serial.println("Camera initialized\r\n");
    }

    variableSetup();
    delay(1000);
    startWebServer();
    delay(1000);
    
    Serial.println("\nStarting continious inference in 2 seconds...");
    delay(2000);
}

unsigned long lastInferenceTime = 0;

void loop() {
    //if (webActive) { delay(50); return; } // Pause inference when web server is active
    server.handleClient();

    if (millis() - lastInferenceTime > 1000) {
        runInference();
        lastInferenceTime = millis();
    }
}