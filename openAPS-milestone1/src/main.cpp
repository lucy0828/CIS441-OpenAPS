#include <Arduino.h>
#include <vector>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "arduino_secrets.h"

// WiFi and MQTT configuration

// Task initialization

// Global variables
volatile float current_BG = 0.0;
volatile long current_time = 0;
volatile bool newBGData = false;
volatile bool newInsulinTreatment = false;
volatile bool attributeReceived = false;

void onMqttMessage(int messageSize) {
    // TODO: Implement MQTT message callback
    // Handle attribute updates and CGM data
    // Update openAPS, current_BG, current_time, and flags as needed
}

void TaskMQTT(void *pvParameters) {
    // TODO: Implement MQTT task
    // Continuously poll for MQTT messages
}

void TaskOpenAPS(void *pvParameters) {
    // TODO: Implement OpenAPS task
    // Process new data, calculate basal rate, and publish to MQTT

    // *** MILESTONE 1 ONLY ***
    // ------------------------------
    // dummy calculation for insulin rate:
    // if new incoming BG > 120 then
    //   insulin_rate = 0.5
    // else
    //   insuln_rate = 0.0
    // ==============================
}

void setup() {
    // TODO: Implement setup function
    // Initialize Serial, WiFi, MQTT, OpenAPS, mutex, and tasks
    // Subscribe to necessary MQTT topics
    // Request virtual patient profile
}

void loop() {
    // Empty. Tasks are handled by FreeRTOS
}
