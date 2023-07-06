import serial

# Set the correct serial port and baud rate
ser = serial.Serial('/dev/ttyUSB0', 9600)

while True:
    if ser.in_waiting > 0:
        data = ser.readline().decode().strip()
        print("Received data from Arduino: ", data)
