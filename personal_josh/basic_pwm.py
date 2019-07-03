# Code for remote control car
# Joshua Beverley ELEC3850

import ADC0832_tmp
import RPi.GPIO as GPIO
import time

btn = 15  # define button pin
E1 = 29  # control pins for the motor controller
M1 = 31  # set to GPIO outputs
E2 = 33
M2 = 35

def setup():
    ADC0832_tmp.setup()  # setup ADC0832
    GPIO.setmode(GPIO.BOARD)  # numbers GPIOs by physical location
    GPIO.setup(btn, GPIO.IN, pull_up_down=GPIO.PUD_UP)  # setup button pin as input an pull it up
    GPIO.setup(E1, GPIO.OUT)  # set up control pins as outputs
    GPIO.setup(M1, GPIO.OUT)
    GPIO.setup(E2, GPIO.OUT)
    GPIO.setup(M2, GPIO.OUT)

def loop():
    loop_exit = 1
    check = 1
    while loop_exit:
        joystick = result()
        if joystick == 1:
            # forwards
            GPIO.output(E1, GPIO.HIGH)
            GPIO.output(M1, GPIO.LOW)
            GPIO.output(E2, GPIO.HIGH)
            GPIO.output(M2, GPIO.LOW)
            print "Forwards/n"
        elif joystick == 2:
            # backwards
            GPIO.output(M1, GPIO.LOW)
            GPIO.output(M2, GPIO.LOW)
            if (check == 1):
                GPIO.output(E1, GPIO.HIGH)
                GPIO.output(E2, GPIO.HIGH)
            else # check == 0
                GPIO.output(E1, GPIO.LOW)
                GPIO.output(E2, GPIO.LOW)
            print "Half Speed/n"
        elif joystick == 3: 
            # left
            GPIO.output(E1, GPIO.LOW)
            GPIO.output(E2, GPIO.HIGH)
            GPIO.output(M2, GPIO.LOW)
            print "Left/n"
        elif joystick == 4:
            # right
            GPIO.output(E1, GPIO.HIGH)
            GPIO.output(M1, GPIO.LOW)
            GPIO.output(E2, GPIO.LOW)
            print "Right/n"
        elif joystick == 5:
            # joystick button pressed, exit
            print "Exit/n"
            loop_exit = 0
        else: 
            # joystick not pressed, stop
            GPIO.output(E1, GPIO.LOW)
            GPIO.output(E2, GPIO.LOW)
            
def result():
    if ADC0832_tmp.getResult1() == 0:
        return 1  # up
    if ADC0832_tmp.getResult1() == 255:
        return 2  # down
    if ADC0832_tmp.getResult() == 0:
        return 3  # left
    if ADC0832_tmp.getResult() == 255:
        return 4  # right
    if GPIO.input(btn) == 0:
        return 5  # button
    return 0
    
if __name__ == '__main__':  # Program starts from here
    setup()
    loop()
    GPIO.cleanup()