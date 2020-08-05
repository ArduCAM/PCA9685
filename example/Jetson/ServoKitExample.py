# encoding: UTF-8
'''
    Arducam ptz controller.

    Copyright (c) 2020-08 Arducam <http://www.arducam.com>.

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

import os
import sys
import time
from ServoKit import *
import curses

global image_count
image_count = 0

# Rendering status bar
def RenderStatusBar(stdscr):
    height, width = stdscr.getmaxyx()
    statusbarstr = "Press 'q' to exit"
    stdscr.attron(curses.color_pair(3))
    stdscr.addstr(height-1, 0, statusbarstr)
    stdscr.addstr(height-1, len(statusbarstr), " " *
                  (width - len(statusbarstr) - 1))
    stdscr.attroff(curses.color_pair(3))

# Rendering description
def RenderDescription(stdscr):
    servo0_val = "Servo0   : 'a'-'d' Key"
    servo1_val = "Servo1   : 'w'-'s' Key"
    servo2_val = "Servo2   : Left-Right Arrow"
    servo3_val = "Servo3   : Up-Down Arrow"

    desc_y = 1

    stdscr.addstr(desc_y + 1, 0, servo0_val, curses.color_pair(1))
    stdscr.addstr(desc_y + 2, 0, servo1_val, curses.color_pair(1))
    stdscr.addstr(desc_y + 3, 0, servo2_val, curses.color_pair(1))
    stdscr.addstr(desc_y + 4, 0, servo3_val, curses.color_pair(1))

# Rendering  middle text
def RenderMiddleText(stdscr, k, servoKit):
    # get height and width of the window.
    height, width = stdscr.getmaxyx()
    # Declaration of strings
    title = "Arducam Controller"[:width-1]
    subtitle = ""[:width-1]
    keystr = "Last key pressed: {}".format(k)[:width-1]

    # Obtain device infomation
    servo0_val = "Servo0   : {:.2f}".format(servoKit.getAngle(0))[:width-1]
    servo1_val = "Servo1   : {:.2f}".format(servoKit.getAngle(1))[:width-1]
    servo2_val = "Servo2   : {:.2f}".format(servoKit.getAngle(2))[:width-1]
    servo3_val = "Servo3   : {:.2f}".format(servoKit.getAngle(3))[:width-1]

    if k == 0:
        keystr = "No key press detected..."[:width-1]

    # Centering calculations
    start_x_title = int((width // 2) - (len(title) // 2) - len(title) % 2)
    start_x_subtitle = int(
        (width // 2) - (len(subtitle) // 2) - len(subtitle) % 2)
    start_x_keystr = int((width // 2) - (len(keystr) // 2) - len(keystr) % 2)
    start_x_device_info = int(
        (width // 2) - (len("MotorX    : 00000") // 2) - len("MotorX    : 00000") % 2)
    start_y = int((height // 2) - 6)

    # Turning on attributes for title
    stdscr.attron(curses.color_pair(2))
    stdscr.attron(curses.A_BOLD)

    # Rendering title
    stdscr.addstr(start_y, start_x_title, title)

    # Turning off attributes for title
    stdscr.attroff(curses.color_pair(2))
    stdscr.attroff(curses.A_BOLD)

    # Print rest of text
    stdscr.addstr(start_y + 1, start_x_subtitle, subtitle)
    stdscr.addstr(start_y + 3, (width // 2) - 2, '-' * 4)
    stdscr.addstr(start_y + 5, start_x_keystr, keystr)
    # Print device info
    stdscr.addstr(start_y + 6, start_x_device_info, servo0_val)
    stdscr.addstr(start_y + 7, start_x_device_info, servo1_val)
    stdscr.addstr(start_y + 8, start_x_device_info, servo2_val)
    stdscr.addstr(start_y + 9, start_x_device_info, servo3_val)

# parse input key
def parseKey(k, servoKit, camera):
    global image_count
    motor_step = 5
    if k == ord('s'):
        servoKit.setAngle(1, servoKit.getAngle(1) - motor_step)
    elif k == ord('w'):
        servoKit.setAngle(1, servoKit.getAngle(1) + motor_step)
    elif k == ord('d'):
        servoKit.setAngle(0, servoKit.getAngle(0) + motor_step)
    elif k == ord('a'):
        servoKit.setAngle(0, servoKit.getAngle(0) - motor_step)
    elif k == ord('r'):
        servoKit.resetAll()
    elif k == curses.KEY_DOWN:
        servoKit.setAngle(3, servoKit.getAngle(3) - motor_step)
    elif k == curses.KEY_UP:
        servoKit.setAngle(3, servoKit.getAngle(3) + motor_step)
    elif k == curses.KEY_RIGHT:
        servoKit.setAngle(2, servoKit.getAngle(2) + motor_step)
    elif k == curses.KEY_LEFT:
        servoKit.setAngle(2, servoKit.getAngle(2) - motor_step)


# Python curses example Written by Clay McLeod
# https://gist.github.com/claymcleod/b670285f334acd56ad1c
def draw_menu(stdscr, camera):
    servoKit = ServoKit(4)

    k = 0
    cursor_x = 0
    cursor_y = 0

    # Clear and refresh the screen for a blank canvas
    stdscr.clear()
    stdscr.refresh()

    # Start colors in curses
    curses.start_color()
    curses.init_pair(1, curses.COLOR_CYAN, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_RED, curses.COLOR_BLACK)
    curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)

    # Loop where k is the last character pressed
    while (k != ord('q')):
        # Initialization
        stdscr.clear()
        # Flush all input buffers.
        curses.flushinp()
        # get height and width of the window.
        height, width = stdscr.getmaxyx()

        # parser input key
        parseKey(k, servoKit, camera)

        # Rendering some text
        whstr = "Width: {}, Height: {}".format(width, height)
        stdscr.addstr(0, 0, whstr, curses.color_pair(1))

        # render key description
        RenderDescription(stdscr)
        # render status bar
        RenderStatusBar(stdscr)
        # render middle text
        RenderMiddleText(stdscr, k, servoKit)
        # Refresh the screen
        stdscr.refresh()

        # Wait for next input
        k = stdscr.getch()


def main():
    curses.wrapper(draw_menu, None)


if __name__ == "__main__":
    main()
