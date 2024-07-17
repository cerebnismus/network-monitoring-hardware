### [bop-eth](bop-eth/)
Allows you to send ICMP requests to remote network nodes. It's built using the **customized Arduino Ethernet library** and the **W5100 library**, based on mbed that implements the EthernetInterface for the Wiznet W5100 based Ethernet shield.

You need to have the Arduino CLI installed on your system to use this project.

This project requires the following Arduino libraries:
```
Ethernet@2.0.0
SoftwareSerial@1.0.0
```

To install the **required** library:
```
arduino-cli lib install "SoftwareSerial@1.0.0"
```

After installing the required libraries, compile the project using the Arduino CLI.

For Linux (Raspberry Pi):
```
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/ttyUSB1  \
  --libraries /home/pi/network-monitoring-hardware/bop-eth/libs/  \
  --build-cache-path /home/pi/network-monitoring-hardware/bop-eth/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /home/pi/network-monitoring-hardware/bop-eth/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /home/pi/network-monitoring-hardware/bop-eth/bop-eth.ino
```

For macOS:
```
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/cu.usbserial-14120  \
  --libraries /Users/macbook/Documents/network-monitoring-hardware/bop-eth/libs/  \
  --build-cache-path /Users/macbook/Documents/network-monitoring-hardware/bop-eth/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /Users/macbook/Documents/network-monitoring-hardware/bop-eth/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /Users/macbook/Documents/network-monitoring-hardware/bop-eth/bop-eth.ino
```

The software will send ICMP requests to the IP address specified in the destinationIP variable in the code. You should replace the IP address with the one for your target network node.

MAC addresses must also be set. Replace destinationMAC with your Router's MAC address and sourceMAC with your Arduino's MAC address.

This project is powered by Arduino Ethernet library and W5100Interface which implements the EthernetInterface for the Wiznet W5100 based Ethernet shield.
