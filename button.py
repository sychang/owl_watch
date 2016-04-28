import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)

GPIO.setup(16, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_UP)

while True:
    panic = GPIO.input(7)
    if panic == False:
        print('Panic Button Pressed')
        #do something here
        time.sleep(0.2)
    off = GPIO.input(16)
    if off == False:
        print('Off Button Pressed')
        time.sleep(0.2)      