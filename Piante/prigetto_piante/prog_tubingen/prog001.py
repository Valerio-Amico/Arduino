from arduino_python import ArduinoUno
import time
import datetime

ard = ArduinoUno(serial_number="692CE576A1A2CCF8C364")
print(dir(ard))

while True:
    ard.digital_write(13, True)
    val = ard.analog_read(0)
    print(val)
    time.sleep(1)
    ard.digital_write(13, False) 
    val = ard.analog_read(0)
    print(val)
    time.sleep(1)

