> "Oh no, not again." - The Bowl of Petunias

## Project Overview
The Bowl of Petunias project aims to create a comprehensive network monitoring system capable of analyzing the intricate signals traversing the network cosmos. Our mission is to dive into the depths of network data and uncover insights that will leave you contemplating the meaning of life, the universe, and everything.

## Features
- **Universal Network Monitoring**: Monitor network devices across all layers, from the fundamental physical layer to the intricate application layer.
- **Traversal Network Discovery**: Discover network devices across all layers, from the fundamental physical layer to the intricate application layer.
- **Signal Spectrum Analysis**: Delve into the complete signal spectrum, unveiling hidden patterns and anomalies in the vastness of network traffic.
- **Big Data Services**: Harness the power of big data to capture, store, and analyze network data, opening up possibilities for deep insights and intelligent decision-making.
- **Cosmic Packet Analysis**: Analyze ICMP and SNMP packets with precision, extracting valuable information and unraveling the mysteries of network behavior.
- **Real-Time Galactic Insights**: Process network data in real-time, providing you with up-to-the-minute insights and the ability to act swiftly in this ever-expanding universe of interconnected devices.


## Getting Started
Follow these steps to embark on your journey with the Bowl of Petunias
1. Clone this repository to your local machine.
- ```git clone https://github.com/cerebnismus/bowl-of-petunias.git```
3. Explore the source code and unleash your inner intergalactic network explorer.
4. Customize and enhance the project to suit your cosmic monitoring needs.


### Description
- [bop-eth](bop-eth/)
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
  --libraries /home/pi/bowl-of-petunias/bop-eth/libs/  \
  --build-cache-path /home/pi/bowl-of-petunias/bop-eth/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /home/pi/bowl-of-petunias/bop-eth/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /home/pi/bowl-of-petunias/bop-eth/bop-eth.ino
```

For macOS:
```
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/cu.usbserial-14120  \
  --libraries /Users/macbook/Documents/bowl-of-petunias/bop-eth/libs/  \
  --build-cache-path /Users/macbook/Documents/bowl-of-petunias/bop-eth/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /Users/macbook/Documents/bowl-of-petunias/bop-eth/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /Users/macbook/Documents/bowl-of-petunias/bop-eth/bop-eth.ino
```

The software will send ICMP requests to the IP address specified in the destinationIP variable in the code. You should replace the IP address with the one for your target network node.

MAC addresses must also be set. Replace destinationMAC with your Router's MAC address and sourceMAC with your Arduino's MAC address.

This project is powered by Arduino Ethernet library and W5100Interface which implements the EthernetInterface for the Wiznet W5100 based Ethernet shield.


## Contributing
Contributions from fellow cosmic explorers are most welcome! If you have ideas, improvements, or bug fixes, feel free to submit a pull request. Together, we can push the boundaries of network monitoring and bring forth new discoveries.

## License
This project is licensed under the [MIT License](LICENSE), granting you the freedom to explore, experiment, and create without boundaries.
