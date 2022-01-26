/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SENSOR_THRESHOLD 800     // Out of 4095
#define SENSOR_PIN 15
#define LED_BUILTIN 2

bool isOn = true;
bool isHit = false;
int hitForce = 0;
unsigned long resetTime = millis();
unsigned long hitDuration = 0;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            if (value == "on") {
                isOn = true;

                // Reset hit parameters
                isHit = false;
                resetTime = millis();
                hitDuration = 0;
            } else if (value == "off") {
                isHit = false;
                isOn = false;
            }
        }
    }

    void onRead(BLECharacteristic *pCharacteristic) override {
        if (isHit) {
            char *buffer;
            asprintf(&buffer, "hit;%d;%lu", hitForce, hitDuration);
            pCharacteristic->setValue(buffer);
        } else if (isOn){
            pCharacteristic->setValue("on");
        } else {
            pCharacteristic->setValue("off");
        }
    }
};

void checkIfIsHit() {
    int value = analogRead(SENSOR_PIN);
    if (value < SENSOR_THRESHOLD || value == 4095) {
        return;
    }

    isOn = false;
    isHit = true;

    if (hitDuration == 0) {
        hitDuration = millis() - resetTime;
    }

    if (value > hitForce) {
        hitForce = value;
    }
}

void outputIsHit() {
    if (isOn) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void setup() {
    Serial.begin(115200);
//    Serial.println("1- Download and install an BLE scanner app in your phone");
//    Serial.println("2- Scan for BLE devices in the app");
//    Serial.println("3- Connect to ESP32-BLE_Server");
//    Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
//    Serial.println("5- See the magic =)");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);

    BLEDevice::init("Target Practice");
    BLEServer *pServer = BLEDevice::createServer();

    // Create custom service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("");
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void loop() {
    checkIfIsHit();
    outputIsHit();
    delay(1);
}
