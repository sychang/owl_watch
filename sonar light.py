import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
import pygame

#PLAY AUDIO CLIP
def play_audio(clip_to_play):
  print 'play audio start'
  pygame.mixer.init()
  pygame.mixer.music.load(clip_to_play)
  pygame.mixer.music.play()
  while pygame.mixer.music.get_busy() == True:
    continue

TRIG = 20 
ECHO = 26

red = 17
green = 27
blue = 18

# Buttons TODO: UPDATE
GPIO.setup(11, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(11, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(11, GPIO.IN, pull_up_down=GPIO.PUD_UP)

print "Distance Measurement In Progress"

GPIO.setup(TRIG,GPIO.OUT)
GPIO.setup(ECHO,GPIO.IN)

GPIO.setup(red, GPIO.OUT)
GPIO.setup(green, GPIO.OUT)
GPIO.setup(blue, GPIO.OUT)

GPIO.output(red, 0)
GPIO.output(green, 0)
GPIO.output(blue, 0)
GPIO.output(TRIG, False)
print "Waiting For Sensor To Settle"
time.sleep(2)


while(True):
	#start the live stream when turned on
	#LD_LIBRARY_PATH=/usr/local/lib mjpg_streamer -i "input_file.so -f /tmp/stream -n pic.jpg" -o "output_http.so -w /usr/local/www"

	GPIO.output(TRIG, True)
	time.sleep(0.00001)
	GPIO.output(TRIG, False)

	# UPDATE THESE PINS
	panic_button = GPIO.input(11)
	off_button = GPIO.input(11)
	report_button = GPIO.input(11)

	while GPIO.input(ECHO)==0:
		pulse_start = time.time()

	while GPIO.input(ECHO)==1:
		pulse_end = time.time()

	pulse_duration = pulse_end - pulse_start

	distance = pulse_duration * 17150

	distance = round(distance, 2)
	GPIO.output(red, 0)
	GPIO.output(green, 0)
	GPIO.output(blue, 0)

	if panic_button == False:
    print('Panic Button Pressed')
    play_audio('panic.mp3')
    time.sleep(0.2)
  elif off_button == False:
  	print('Off Button Pressed')
    time.sleep(0.2)
  elif report_button == False:
  	print('Report Button Pressed')
  	play_audio('report.mp3')
  	time.sleep(0.2)
	
	#if distance < 10:
		#not manually shut off within x time, send alert
		#speaker gets really loud
	if distance < 50:
		#activates timed warning
		GPIO.output(red, 1)
		
	elif distance < 300:
		GPIO.output(blue, 1)
		
	print "Distance:",distance,"cm"
	time.sleep(0.1)

GPIO.cleanup()
