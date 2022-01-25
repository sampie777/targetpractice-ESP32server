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
#define LED_BUILTIN 2

bool isHit = false;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            if (value == "on") {
                isHit = false;
            } else if (value == "off") {
                isHit = true;
            }

            for (int i = 0; i < value.length(); i++) {
                Serial.println(value[i]);
            }
        }
    }

    void onRead(BLECharacteristic *pCharacteristic) override {
        if (isHit) {
            pCharacteristic->setValue("hit");
        } else {
            pCharacteristic->setValue("");
        }
    }
};

void setup() {
    Serial.begin(115200);
//    Serial.println("1- Download and install an BLE scanner app in your phone");
//    Serial.println("2- Scan for BLE devices in the app");
//    Serial.println("3- Connect to ESP32-BLE_Server");
//    Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
//    Serial.println("5- See the magic =)");

    pinMode(LED_BUILTIN, OUTPUT);

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
    if (isHit) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    delay(20);
}
