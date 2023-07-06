#include <SoftwareSerial.h>

// Set up the SoftwareSerial instance
SoftwareSerial gsmSerial(7, 8);  // RX, TX pins for the GSM shield

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  gsmSerial.begin(9600);

  // Read the phone number from the Raspberry Pi
  char phone_number[20];
  int index = 0;
  while (Serial.available()) {
    char c = Serial.read();
    phone_number[index] = c;
    index++;
  }
  phone_number[index] = '\0';  // Null-terminate the string

  // Send the phone number to the GSM module
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phone_number);
  gsmSerial.println("\"");
  delay(1000);

  // Send an example SMS
  gsmSerial.println("Hello from Arduino!");
  delay(1000);
  gsmSerial.write(26);  // End the SMS by sending Ctrl+Z
}

void loop() {
  // Do nothing in the loop
}

