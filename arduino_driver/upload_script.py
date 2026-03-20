import subprocess
import serial.tools.list_ports
import os

_path_py_file = os.path.dirname(os.path.realpath(__file__))
_serial_number = "692CE576A1A2CCF8C364"
_file_name = "arduino_driver.ino"
_port = ""

ports = serial.tools.list_ports.comports()

for port in ports:
    print(port.serial_number)

for port in ports:
   
    ser_num = port.serial_number
    if _serial_number == ser_num:
        _port = port
        print(f"seial port {port} found for serial n. {ser_num}")
        break

if _port != "":
    print("path = ", f"{_path_py_file}/{_file_name}")
    subprocess.run([f"arduino-cli compile --fqbn arduino:megaavr:uno2018 ./{_file_name}"], shell = True)
    subprocess.run([f"arduino-cli upload ./{_file_name }-p {_port} -b arduino:megaavr:uno2018"], shell = True)
else:
    print(f"no serial port found for serial n. {_serial_number}")


