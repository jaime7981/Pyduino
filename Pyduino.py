import serial as sr
import threading
import time

import kivy
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput
from kivy.uix.gridlayout import GridLayout
from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle
from kivy.clock import Clock

serial_port = '/dev/ttyUSB0'
serial_bts = 9600
printdata = ''
arduino = sr.Serial(port = serial_port, baudrate = serial_bts, timeout = .1)

class MenuScreen(Widget):
    pass

class PyduinoApp(App):
    def build(self):
        return MenuScreen()

    def on_start(self):
        Clock.schedule_interval(self.ReadData, 0.5)

    def ReadData(self, *args):
        data = arduino.readline()
        if (data != b''):
            printdata = str(data)
            print("Recived data: " + printdata)
            self.root.ids.output.text = printdata

def WriteData(*args):
    flag = True

    while (flag):
        command = input("Command (Exit --> E): ")
        arduino.write(bytes(command, 'utf-8'))

def Connect():
    t2 = threading.Thread(target = WriteData, args = ("Write",), daemon = True)
    t2.start()

def StartGui():
    PyduinoApp().run()

if __name__ == "__main__":

    try:
        Connect()
    except Exception as e:
        print(e)

    StartGui()
