import subprocess
import serial.tools.list_ports
import os

_path_py_file = os.path.dirname(os.path.realpath(__file__))
_serial_number = "692CE576A1A2CCF8C364"
_file_name = "prova_sens_dist.ino"
_port = ""

ports = serial.tools.list_ports.comports()

# for port in ports:
    # print(port.serial_number)

for port in ports:
   
    ser_num = port.serial_number
    if _serial_number == ser_num:
        # for el in dir(port):
            # if "_" not in el:
                # print(el, getattr(port, el))
        _port = port.device
        print(f"seial port {_port} found for serial n. {ser_num}")
        break

if _port != "":
    _path = f"{_path_py_file}/{_file_name}"
    print("path = ", _path)
    subprocess.run([f"arduino-cli compile --fqbn arduino:megaavr:uno2018 ./{_file_name}"], shell = True)
    
    command_upload = f"arduino-cli upload {_path} -p {_port} -b arduino:megaavr:uno2018"
    print("command = ", command_upload)
    subprocess.run([command_upload], shell = True)
else:
    print(f"no serial port found for serial n. {_serial_number}")


