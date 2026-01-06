#include <Arduino.h>
#include <WebServer.h>

#include "globals.h"
#include "camera/camera_runtime.h"
#include "esp_camera.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
    <meta charset="utf-8">
    <title>ESP32 Camera</title>
    <style>
        body { font-family: Arial, sans-serif; text-align:center; margin:20px; }
        img { border:1px solid #444; width:640px; max-width:100%; height:auto; }
        button { font-size:16px; padding:10px 16px; margin-top:12px; }
        #status { margin-top:8px; color:#333; }
    </style>
    </head>
    <body>
        <h2>ESP32S3 Camera Stream</h2>
        <img id="stream" src="/stream" alt="stream">
        <div>
            <button id="captureBtn">Download Current Frame</button>
        </div>
        <div id="status"></div>

        <script>
            const btn = document.getElementById('captureBtn');
            const status = document.getElementById('status');

            btn.addEventListener('click', async () => {
                try {
                    btn.disabled = true;
                    status.textContent = 'Capturing...';

                    // Fetch /capture (server returns image/jpeg)
                    const resp = await fetch('/capture');
                    if (!resp.ok) throw new Error('Capture failed: ' + resp.status);

                    const blob = await resp.blob();
                    // Create download filename with timestamp
                    const ts = new Date().toISOString().replace(/[:.]/g,'-');
                    const filename = `capture-${ts}.jpg`;

                    // Create temporary link and click it
                    const url = window.URL.createObjectURL(blob);
                    const a = document.createElement('a');
                    a.href = url;
                    a.download = filename;
                    document.body.appendChild(a);
                    a.click();
                    a.remove();
                    window.URL.revokeObjectURL(url);

                    status.textContent = 'Downloaded: ' + filename;
                } catch (err) {
                    console.error(err);
                    status.textContent = 'Error: ' + err.message;
                } finally {
                    // re-enable after a short delay to avoid spamming
                    setTimeout(()=> { btn.disabled = false; }, 500);
                }
            });
        </script>
    </body>
</html>
)rawliteral";

void streamHandler() {
    WiFiClient client = server.client();
    Serial.println("Client connected -> blocking stream");
    // REQUIRED MJPEG HEADERS 
    client.println("HTTP/1.1 200 OK");

    client.println("Content-Type: multipart/x-mixed-replace; boundary=frame"); 
    client.println("Cache-Control: no-cache"); 
    client.println(); // BLOCKING LOOP: RUN UNTIL BROWSER DISCONNECTS

    unsigned long lastMillis = millis();

    while (client.connected()) {
        if (millis() - lastMillis >= 100) { // Stream at ~10 fps
            lastMillis = millis();

            camera_fb_t *fb = esp_camera_fb_get();
            if (!fb) {
                Serial.println("Camera capture failed");
                break;
            }

            // Send MJPEG frame headers
            client.println("--frame");
            client.println("Content-Type: image/jpeg");
            client.printf("Content-Length: %u\r\n", fb->len);
            client.println(); // blank line between headers and image data

            // Send the JPEG image data
            client.write(fb->buf, fb->len);
            client.println(); // CRLF after image data

            esp_camera_fb_return(fb);
        }

        server.handleClient(); // Keep server responsive
    }
    
    Serial.println("Client disconnected (blocking stream ended)");
    
    lastInferenceTime = millis(); // reset inference timer
}

void captureHandler() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        server.send(500, "text/plain", "Camera capture failed");
        return;
    }

    // Tell server how long the body will be
    server.setContentLength(fb->len);
    server.sendHeader("Content-Type", "image/jpeg");
    server.sendHeader("Content-Disposition", "attachment; filename=capture.jpg");
    server.send(200); // send headers

    // Write the JPEG bytes to the client
    WiFiClient client = server.client();
    client.write(fb->buf, fb->len);

    esp_camera_fb_return(fb);

    server.handleClient(); // Keep server responsive
}

void startWebServer() {
    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", index_html);
    });

    server.on("/stream", HTTP_GET, streamHandler);
    server.on("/capture", HTTP_GET, captureHandler);

    server.begin();
    Serial.println("Web server started at http://" + WiFi.localIP().toString());
}