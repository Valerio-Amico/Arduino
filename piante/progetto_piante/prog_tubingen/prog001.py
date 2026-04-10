from arduino_python import ArduinoUno
import time
import datetime

ON_TIME  = datetime.time(7, 0)   # 07:00
OFF_TIME = datetime.time(20, 0) 

class Program():
    def __init__(self):
        self.ard: ArduinoUno|None=None
        self.connected = False
        
    def connect(self):
        _ard = ArduinoUno(serial_number="692CE576A1A2CCF8C364")
        print("connected to arduino")
        return _ard

    def start(self):
        while True:
            try:
                # check connection
                if not self.connected:
                    self.ard = self.connect()
                    self.connected = True

                # check time
                now = datetime.datetime.now().time()
                status:bool = not ( True if ON_TIME <= now < OFF_TIME else False)
                print(status, ON_TIME, now, OFF_TIME)
                self.ard.digital_write(13, status)
            
                # distance=self.ard.read_distance(9, 10)
                # print(distance)
            except:
                self.connected = False
                print("connection error")
            
            time.sleep(10)

if __name__=="__main__":
    prog = Program()
    prog.start()
