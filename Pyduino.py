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

class MenuScreen(Widget):
    pass

class PyduinoApp(App):
    def build(self):
        self.conection = False
        self.arduino = None
        self.terminal_output = []
        self.serial_port = '/dev/ttyUSB0'
        self.serial_bts = 9600
        return MenuScreen()

    def on_start(self):
        Clock.schedule_interval(self.ReadData, 0.5)

    def ReadData(self, *args):
        try:
            if (self.conection == True and self.arduino != None):
                data = self.arduino.readline()
                if (data != b''):
                    self.terminal_output.append(" " + str(data))
                    if (len(self.terminal_output) > 21):
                        self.terminal_output.pop(0)
                    
                    finalstring = ""
                    for newline in self.terminal_output:
                        finalstring = finalstring + newline + "\n"
                    self.root.ids.output.text = finalstring
        except Exception as e:
            self.root.ids.connectstatus.text = "Arduino disconected"
            self.root.ids.connect.text = "Conect"
            self.conection = False
            self.arduino = None
            print(e)
    
    def WriteData(self):
        try:
            if (self.conection == True and self.arduino != None):
                if (str(self.root.ids.inputcommand) != ''):
                    self.arduino.write(bytes(str(self.root.ids.inputcommand.text), 'utf-8'))
        except Exception as e:
            self.root.ids.connectstatus.text = "Arduino disconected"
            self.root.ids.connect.text = "Conect"
            self.conection = False
            self.arduino = None
            print(e)

    def WriteFromButton(self, buttoncommand):
        try:
            if (self.conection == True and self.arduino != None):
                self.arduino.write(bytes(str(buttoncommand), 'utf-8'))
        except Exception as e:
            self.root.ids.connectstatus.text = "Arduino disconected"
            self.root.ids.connect.text = "Conect"
            self.conection = False
            self.arduino = None
            print(e)

    def Conection(self):
        if (self.conection == False):
            try:
                self.serial_port = self.root.ids.port.text
                self.serial_bts = int(self.root.ids.bitrate.text)
                self.arduino = sr.Serial(port = self.serial_port, baudrate = self.serial_bts, timeout = .1)
                self.conection = True
                self.root.ids.connectstatus.text = "The conection was succesful"
                self.root.ids.connect.text = "Disconect"
            except Exception as e:
                self.root.ids.connectstatus.text = "Error on conection"
                self.conection = False
                self.arduino = None
                print(e)
        else:
            self.root.ids.connectstatus.text = "Arduino disconected"
            self.root.ids.connect.text = "Conect"
            self.arduino = None
            self.conection = False

def StartGui():
    PyduinoApp().run()

if __name__ == "__main__":
    StartGui()
