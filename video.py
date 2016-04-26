import subprocess
import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)

# def record_video(): 
timestamp = time.time()
stop_time = timestamp + 20
record = "raspivid -o test" + str(timestamp) + ".avi -t 20000"
subprocess.call(record, shell=True)
print "recording started"
while True:
    GPIO.output(7, True) ## Turn on GPIO pin 7
    if time.time() >= stop_time:
        GPIO.output(7, False) ## Turn on GPIO pin 7
        GPIO.cleanup()
        print "stopped recording"
        break
