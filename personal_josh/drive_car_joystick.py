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
    toggle = 0
    forward = 0
    leftright = 0
    leftinp = 0
    rightinp = 0
    while loop_exit:
        GPIO.output(M1, GPIO.LOW)
        GPIO.output(M2, GPIO.LOW)
        forward = ADC0832_tmp.getResult1()
        if (forward > 125):
            forward = 0
        else:
            forward = 125 - forward
        leftright = ADC0832_tmp.getResult() - 5
        if (leftright < 0):
            leftright = 0
        if (leftright >= 125):
            leftinp = 125
            rightinp = 125 - (leftright - 125)
        else: # if leftright < 125
            leftinp = leftright
            rightinp = 125
        print "leftright = " + str(leftright) + " " + str(forward) + " -> " + str((float(forward) / 125))
        leftinp = (int((float(forward) / 125) * leftinp * 2 + 0.5)) / 25
        rightinp = (int((float(forward) / 125) * rightinp * 2 + 0.5)) / 25
        if (GPIO.input(btn) == 0):
            loop_exit = 0;
        else:
            pwm(leftinp, rightinp)
            
def pwm(left, right):
    inputleft = left
    inputright = right
    print "check " + str(inputleft) + " " + str(inputright)
    for i in range (0,9):
        print "double check " + str(inputleft) + " " + str(inputright)
        if (inputleft > 0):
            GPIO.output(E1, GPIO.HIGH)
            inputleft = inputleft - 1
        else:
            GPIO.output(E1, GPIO.LOW)
        if (inputright > 0):
            GPIO.output(E2, GPIO.HIGH)
            inputright = inputright - 1
        else:
            GPIO.output(E2, GPIO.LOW)
        time.sleep(0.01)
            
    
if __name__ == '__main__':  # Program starts from here
    setup()
    loop()
    GPIO.cleanup()