# BB gun target practice

To be used with the mobile app: https://github.com/sampie777/targetpractice-app. Or standalone/offline with target auto reset after 1 second. 

## Hardware

Made for ESP32 with Bluetooth LE.

Connect LED's to GPIO 2 and ground. These LED's will indicate whether the target is 'on' or disabled. Make sure max current draw is below max ratings (40 mA).

Connect piezo sensor between ground and GPIO 15 with a 1M ohm resistor in parallel. This sensor will detect a shot on target by vibration. 

