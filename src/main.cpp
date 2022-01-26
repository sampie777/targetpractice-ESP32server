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
#define SENSOR_PIN 15
#define LED_BUILTIN 2

bool isOn = true;
bool isHit = false;
int hitThreshold = 800;     // Out of 4095
int hitForce = 0;
unsigned long resetTime = millis();
unsigned long hitDuration = 0;

BLEServer *pServer;
bool scanStarted = false;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        scanStarted = false;
        std::string value = pCharacteristic->getValue();

        if (value.length() <= 0) {
            return;
        }

        if (value == "on") {
            isOn = true;

            // Reset hit parameters
            isHit = false;
            resetTime = millis();
            hitDuration = 0;
            return;
        }

        if (value == "off") {
            isHit = false;
            isOn = false;
            return;
        }

        if (value.rfind("force;", 0) == 0){
            sscanf(value.c_str(), "force;%d", &hitForce);
            return;
        }
    }

    void onRead(BLECharacteristic *pCharacteristic) override {
        scanStarted = false;

        if (isHit) {
            char *buffer;
            asprintf(&buffer, "hit;%d;%lu", hitForce, hitDuration);
            pCharacteristic->setValue(buffer);
        } else if (isOn) {
            pCharacteristic->setValue("on");
        } else {
            pCharacteristic->setValue("off");
        }
    }
};

void checkIfIsHit() {
    int value = analogRead(SENSOR_PIN);
    if (value < hitThreshold || value == 4095) {
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

/*
 * Run in loop:
 *   void loop() {
 *     testPiezoReadings();
 *   }
 */
void testPiezoReadings() {
    static int values[64] = {0};
    static int valuesIndex = 0;
    static int valuesPrintedIndex = valuesIndex;
    static int prevValue = -1;
    static int printTimeout = 0;

    int value = analogRead(SENSOR_PIN);
    if (value != prevValue) {
        prevValue = value;
        values[valuesIndex++] = value;

        if (valuesIndex >= 64) {
            valuesIndex = 0;
        }
    }

    if (printTimeout == 0 && valuesPrintedIndex != valuesIndex) {
        Serial.println(values[valuesPrintedIndex++]);

        if (valuesPrintedIndex >= 64) {
            valuesPrintedIndex = 0;
        }

        printTimeout = 50;
    }

    if (printTimeout > 0) {
        printTimeout--;
    }
    delay(1);
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);

    BLEDevice::init("Target Practice");
    pServer = BLEDevice::createServer();

    // Create custom service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("");
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    scanStarted = true;

    Serial.println("Ready.");
}

void loop() {
    checkIfIsHit();
    outputIsHit();
    if (!scanStarted && pServer->getConnectedCount() == 0) {
        pServer->startAdvertising();
        scanStarted = true;
    }
    delay(1);
}
