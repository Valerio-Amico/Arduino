# from logging import Logger
from serial.tools.list_ports_common import ListPortInfo
import serial
import serial.tools.list_ports
import struct
import time

# from qcontrol3.tools import logsetup

# log: Logger = logsetup.getlogger("driverlogger")


class ArduinoUno:
    serial_com: serial.Serial

    def __init__(self, serial_number, dummy=False, baudrate=9600, timeout=0.1):
        self._serial_number = serial_number
        self._dummy = dummy
        self._baudrate = baudrate
        self._timeout = timeout
        self._port = self._find_port()
        if not self._port:
            raise ValueError(
                f"No Arduino with serial number '{self._serial_number}' found."
            )

        self.serial_com = serial.Serial(
            port=self._port, baudrate=self._baudrate, timeout=self._timeout
        )

    def _find_port(self) -> str | None:
        ports: list[ListPortInfo] = serial.tools.list_ports.comports()
        port: ListPortInfo
        for port in ports:
            if port.serial_number == self._serial_number:
                print(
                    f"Port '{port}' found for serial number '{self._serial_number}'"
                )
                return port.device
        return None

    def close(self):
        """Close the serial port"""
        print("Arduino: Closing connection to Arduino")
        self.serial_com.close()

    def clear(self):
        """Clear the input/output buffers. Note that this will flush
        (i.e. write) all data that is still in the output buffer.
        """
        print("Arduino: Clearing input/output buffers")
        self.serial_com.reset_input_buffer()
        self.serial_com.reset_output_buffer()

    def _reconnect(self) -> bool:
        print("Arduino: Reconnecting...")
        self._port = self._find_port()
        if self._port == None:
            print("Arduino not found.")
            return False

        try:
            self.serial_com = serial.Serial(
                port=self._port, baudrate=self._baudrate, timeout=self._timeout
            )
            return True
        except serial.SerialException as e:
            print(f"Arduino: Unable to connect: '{e!s}'")
            return False

    def read(self) -> str | None:
        data: str | None = None
        try:
            data = self.serial_com.readline().decode("utf-8").strip()
        except serial.SerialException as e:
            print(f"Error while reading from Arduino: '{e!s}'")
        finally:
            return data

    def write_string(self, message: str):
        try:
            self.serial_com.write(f"{message}\n".encode("utf-8"))
        except serial.SerialException as e:
            print(f"Arduino: Unable to send command: '{e!s}'")
            self._reconnect()

    def digital_write(self, pin: int, value: bool):
        """
        digital write format:
            "D23:1" this will enable the pin 23 of arduino.
            "D12:0" this turns off the pin 12
        """
        self.write_string(f"D{pin:02}:{value:d}")
        self.serial_com.flush()

    def analog_write(self, pin: int, value: int):
        """
        there are two pins that allows analog writing on arduinoDue: DAC0 and DAC1.
        The output value is between 0 and 3.3V, with a resolution of 1/255.
        you can send a value between 000 and 254.
        analog write format example:
            "O0:251" -> arduino outputs 251/255*3.3V on pin DAC0.
            "O1:054" -> arduino outputs 54/255*3.3V on pin DAC1.
        """
        self.write_string(f"O{pin:01}:{value:03}")
        self.serial_com.flush()

    def analog_read(self, pin: int) -> str | None:
        """
        there are 8 pins that allows analog reading on arduinoDue: A0 to A7.
        analog write format example:
            "A3:?" -> arduino reads and returns the value from pin A3
        """
        self.write_string(f"A{pin:01}:?")
        self.serial_com.flush()
        return self.read()


class ArduinoDue:
    serial_com: serial.Serial

    def __init__(self, serial_number, dummy=False, baudrate=9600, timeout=0.1):
        self._serial_number = serial_number
        self._dummy = dummy
        self._baudrate = baudrate
        self._timeout = timeout
        self._port = self._find_port()
        if not self._port:
            raise ValueError(
                f"No Arduino with serial number '{self._serial_number}' found."
            )

        self.serial_com = serial.Serial(
            port=self._port, baudrate=self._baudrate, timeout=self._timeout
        )

    def _find_port(self) -> str | None:
        ports: list[ListPortInfo] = serial.tools.list_ports.comports()
        port: ListPortInfo
        for port in ports:
            if port.serial_number == self._serial_number:
                print(
                    f"Port '{port}' found for serial number '{self._serial_number}'"
                )
                return port.device
        return None

    def close(self):
        """Close the serial port"""
        print("Arduino: Closing connection to Arduino")
        self.serial_com.close()

    def clear(self):
        """Clear the input/output buffers. Note that this will flush
        (i.e. write) all data that is still in the output buffer.
        """
        print("Arduino: Clearing input/output buffers")
        self.serial_com.reset_input_buffer()
        self.serial_com.reset_output_buffer()

    def _reconnect(self) -> bool:
        print("Arduino: Reconnecting...")
        self._port = self._find_port()
        if self._port == None:
            print("Arduino not found.")
            return False

        try:
            self.serial_com = serial.Serial(
                port=self._port, baudrate=self._baudrate, timeout=self._timeout
            )
            return True
        except serial.SerialException as e:
            print(f"Arduino: Unable to connect: '{e!s}'")
            return False

    def read(self) -> str | None:
        data: str | None = None
        try:
            data = self.serial_com.readline().decode("utf-8").strip()
        except serial.SerialException as e:
            print(f"Error while reading from Arduino: '{e!s}'")
        finally:
            return data

    def write_string(self, message: str):
        try:
            self.serial_com.write(f"{message}\n".encode("utf-8"))
        except serial.SerialException as e:
            print(f"Arduino: Unable to send command: '{e!s}'")
            self._reconnect()

    def digital_write(self, pin: int, value: bool):
        """
        digital write format:
            "D23:1" this will enable the pin 23 of arduino.
            "D12:0" this turns off the pin 12
        """
        self.write_string(f"D{pin:02}:{value:d}")
        self.serial_com.flush()

    def analog_write(self, pin: int, value: int):
        """
        there are two pins that allows analog writing on arduinoDue: DAC0 and DAC1.
        The output value is between 0 and 3.3V, with a resolution of 1/255.
        you can send a value between 000 and 254.
        analog write format example:
            "O0:251" -> arduino outputs 251/255*3.3V on pin DAC0.
            "O1:054" -> arduino outputs 54/255*3.3V on pin DAC1.
        """
        self.write_string(f"O{pin:01}:{value:03}")
        self.serial_com.flush()

    def analog_read(self, pin: int) -> str | None:
        """
        there are 8 pins that allows analog reading on arduinoDue: A0 to A7.
        analog write format example:
            "A3:?" -> arduino reads and returns the value from pin A3
        """
        self.write_string(f"A{pin:01}:?")
        self.serial_com.flush()
        return self.read()


def main():
    arduino = ArduinoDue(serial_number="43439353536351915121")
    # arduino = ArduinoDue('/dev/ttyACM0')
    time.sleep(3)

    while True:
        arduino.digital_write(13, True)
        time.sleep(1)
        arduino.digital_write(13, False)
        time.sleep(1)

        arduino.analog_write(0, 130)
        time.sleep(1)
        arduino.analog_write(0, 30)
        time.sleep(1)

        value = arduino.analog_read(0)
        print(value)
        time.sleep(1)
        value = arduino.analog_read(0)
        print(value)
        time.sleep(1)


if __name__ == "__main__":
    main()
