# encoding: UTF-8
'''
    Copyright (c) 2020-8 Arducam <http://www.arducam.com>.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
    OR OTHER DEALINGS IN THE SOFTWARE.
'''

import time
import adafruit_servokit


class ServoKit(object):
    default_angle = 90

    def __init__(self, num_ports):
        print("Initializing the servo...")
        self.kit = adafruit_servokit.ServoKit(channels=16)
        self.num_ports = num_ports
        self.resetAll()
        print("Initializing complete.")

    def setAngle(self, port, angle):
        if angle < 0:
            self.kit.servo[port].angle = 0
        elif angle > 180:
            self.kit.servo[port].angle = 180
        else:
            self.kit.servo[port].angle = angle
    
    def getAngle(self, port):
        return self.kit.servo[port].angle

    def reset(self, port):
        self.kit.servo[port].angle = self.default_angle

    def resetAll(self):
        for i in range(self.num_ports):
            self.kit.servo[i].angle = self.default_angle


def test():
    servoKit = ServoKit(4)
    print("Start test")
    for i in range(0,180, 5):
        servoKit.setAngle(0, i)
        servoKit.setAngle(2, i)
        time.sleep(.05)
    for i in range(180,0,-5):
        servoKit.setAngle(0, i)
        servoKit.setAngle(2, i)
        time.sleep(.05)

    for i in range(15,145, 5):
        servoKit.setAngle(1, i)
        servoKit.setAngle(3, i)
        time.sleep(.05)
    for i in range(145,15,-5):
        servoKit.setAngle(1, i)
        servoKit.setAngle(3, i)
        time.sleep(.05)
    
    servoKit.resetAll()

if __name__ == "__main__":
    test()