
//#include <Arduino.h>
//
//#include "BluetoothSerial.h"
//
//#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
//#endif
//
//#define LED_BUILTIN 2
//
//BluetoothSerial SerialBT;
//
//void setup() {
//    Serial.begin(115200);
//    SerialBT.begin("Target Practice"); //Bluetooth device name
//    pinMode(LED_BUILTIN, OUTPUT);
//    digitalWrite(LED_BUILTIN, LOW);
//    Serial.println("Ready.");
//}
//
//void loop() {
//    static char receiveBuffer[8] = "";
//    static uint8_t receiveBufferIndex = 0;
//
//    if (Serial.available()) {
//        SerialBT.write(Serial.read());
//    }
//
//    if (SerialBT.available()) {
//        char receivedChar = SerialBT.read();
//        if (receivedChar == 10) {
//            return;
//        }
//
//        if (receivedChar == 13) {
//            Serial.write(receiveBuffer);
//            Serial.print('\n');
//
//            if (strcmp(receiveBuffer, "on") == 0) {
//                digitalWrite(LED_BUILTIN, HIGH);
//            } else if (strcmp(receiveBuffer, "off") == 0) {
//                digitalWrite(LED_BUILTIN, LOW);
//            }
//
//            // Reset buffer
//            for (char &i: receiveBuffer) {
//                i = 0;
//            }
//            receiveBufferIndex = 0;
//        } else {
//            receiveBuffer[receiveBufferIndex++] = receivedChar;
//            if (receiveBufferIndex >= 8) {
//                receiveBufferIndex = 0;
//            }
//        }
//    }
//    delay(20);
//}