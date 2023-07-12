### [bop-gsm](bop-gsm/)
This project enables you to send ICMP requests to remote network nodes using Arduino SoftwareSerial library and the Sixfab Tracker library.

**The code includes:**
- Initialization of Sixfab Tracker
- Indicating the readiness of the system with the onboard User LED
- Sending an SMS message for notification
- Configuring the GSM module for text mode SMS
- Receiving and processing user input from the serial monitor
- Sending an ICMP Echo Request to a predefined IP address when 'p' is received from the serial monitor.

The program depends on two libraries: SoftwareSerial and Sixfab Tracker. The former allows for serial communication on other digital pins of the Arduino, while the latter is a specialized library for the Sixfab Tracker.

Follow these steps to install the libraries:

Open your terminal and run ```arduino-cli board list``` to make sure your Arduino board is recognized.
Run ```arduino-cli lib install "Sixfab_Tracker"``` to install the Sixfab Tracker library.
Run ```arduino-cli lib install "SoftwareSerial@1.0.0"``` to install the SoftwareSerial library.

Use the following command to compile the project on pi:
```
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/ttyUSB0  \
  --libraries /home/pi/bowl-of-petunias/bop-gsm/libs/  \
  --build-cache-path /home/pi/bowl-of-petunias/bop-gsm/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /home/pi/bowl-of-petunias/bop-gsm/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /home/pi/bowl-of-petunias/bop-gsm/bop-gsm.ino
```

This project is based on the Sixfab Arduino Tracker Shield library originally created by Yasin Kaya (selengalp), and later updated by Oguzhan Ince (cerebnismus). The original library can be found [here](https://github.com/sixfab/Sixfab_Arduino_Tracker_Shield/).