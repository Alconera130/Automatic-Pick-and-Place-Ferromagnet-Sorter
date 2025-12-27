#include <Arduino.h>
#include <WiFi.h>
#include "globals.h"
#include "wifi/wifi_manager.h"

IPAddress local_IP;

char wifi_ssid[64] = {};
char wifi_pass[64] = {};

void wifiSetup() {
    Serial.println("\n=== Wi-Fi Setup (STA) ===");

    Serial.print("Type Wi-Fi SSID and press Enter: ");
    while (!Serial.available()) delay(10);
    String s = Serial.readStringUntil('\n'); s.trim();
    s.toCharArray(wifi_ssid, sizeof(wifi_ssid));
    Serial.println(wifi_ssid);

    Serial.println("Type Wi-Fi password and press Enter:");
    while (!Serial.available()) delay(10);
    String p = Serial.readStringUntil('\n'); p.trim();
    p.toCharArray(wifi_pass, sizeof(wifi_pass));

    Serial.printf("Connecting to Wi-Fi '%s' ...\n", wifi_ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_pass);

    int t = 0;

    while (WiFi.status() != WL_CONNECTED && t < 40) {
        delay(500);
        Serial.print(".");
        t++;
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
        local_IP = WiFi.localIP();
        Serial.printf("Wi-Fi connected, IP: %s\n", local_IP.toString().c_str());

        delay(1000);
    } else {
        Serial.println("Wi-Fi connect failed. Proceeding without preview.");
    }
}