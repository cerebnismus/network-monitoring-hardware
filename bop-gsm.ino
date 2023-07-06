import serial
import time

# Establish serial communication with the Arduino
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Replace with the correct serial port and baud rate

# Delay to ensure a stable connection
time.sleep(2)

# Phone number to send
phone_number = "+905304543426"  # Replace with the desired phone number

# Send the phone number to the Arduino
ser.write(phone_number.encode())

# Close the serial connection
ser.close()

