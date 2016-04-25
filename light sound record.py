import numpy as test_video
import cv2
import time
import os
import subprocess
import RPi.GPIO as GPIO ## Import GPIO Library
import pygame
import threading
from picamera.array import PiRGBArray
from picamera import PiCamera



###CODE TO BLINK LIGHT BEFORE RECORD


## Define function named Blink()
def Blink():
    print "blink start"
    GPIO.setmode(GPIO.BOARD) ## Use BOARD pin numbering
    GPIO.setup(7, GPIO.OUT) ## Setup GPIO pin 7 to OUT
    #might have to debug this because haven't tested
    GPIO.output(7, True)
    time.sleep(1)
    GPIO.output(7, False)
    time.sleep(1)
    #0 second
    for i in range(0, 2):
        GPIO.output(7, True)
        time.sleep(0.5)
        GPIO.output(7, False)
        time.sleep(0.5)
    #4 second
    for i in range(0, 8):
        GPIO.output(7, True)
        time.sleep(0.125)
        GPIO.output(7, False)
        time.sleep(0.125)
        GPIO.output(7, True)
    #5 second
    print "blink end"


#PLAY AUDIO CLIP
def play_audio(clip_to_play):
    print 'play audio start'
    pygame.mixer.init()
    pygame.mixer.music.load(clip_to_play)
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy() == True:
      continue

    #put the one we just played in a folder
    old_dest = directory + '/' + clip_to_play
    new_dest = directory + '/played/' + clip_to_play
    os.rename(old_dest, new_dest) #do i need this to be the correct directory?
    print 'play audio end'

###CODE FROM ORIGINAL START.PY TO RECORD SET LENGTH OF VIDEO

def record_video(): 
    time.sleep(5)
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


#BEGIN CONCAT CODE

def concat_middle(videofiles):
    i = 0
    x = '' 
    while i < len(videofiles):
        
        x += videofiles[i] + '|'
        i += 1 
    return x[0:-1]
        
       # output,error  = subprocess.Popen(
        #           command, universal_newlines=True, shell=True,
         #              stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

def create_video():
    print "begin concat"
    directory = '/home/pi/Desktop/cp1'
    videofiles = [n for n in os.listdir(directory) if n[0]=='t' and n[-4:]=='.avi']

    command_start = 'ffmpeg -i "concat:'
    #command_end = '" -i hotline.m4a -c copy out' + str(timestamp) + '.avi'       
    command_end = '" -i hotline_short.m4a -c copy out' + '.avi'    

    command = command_start + concat_middle(videofiles) + command_end
    print command
#    subprocess.call(command, shell=True)
    output,error  = subprocess.Popen(
                   command, universal_newlines=True, shell=True,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    print "end concat"

###UPLOADING TO YOUTUBE FUNCTION
def upload_vid():
    #print 'sleeping'     
    #time.sleep(100)
    print "begin upload"
    youtube_up = "youtube-upload --title='Groove Cube' /home/pi/Desktop/cp1/out.avi"
    subprocess.call(youtube_up, shell=True)
    print "end upload"


###CODE TO RESPOND TO BUTTON PRESS
GPIO.setmode(GPIO.BOARD)
GPIO.setup(11, GPIO.IN, pull_up_down=GPIO.PUD_UP)

while True:
    input_state = GPIO.input(11)
    if input_state == False:
        #do some sort of subprocess.call(something)
        print('Button Pressed')
	GPIO.cleanup()
        break

###END BUTTON PRESS CODE

threads = []
directory = '/home/pi/Desktop/cp1'
audiofiles = [int(n[2]) for n in os.listdir(directory) if n[0]=='h' and n[-4:]=='.mp3']
audiofiles.sort()
#play lowest numbered first
if len(audiofiles) > 0:
    clip_to_play = 'hb' + str(audiofiles[0]) + '.mp3'

threads.append(threading.Thread(target=Blink))
threads.append(threading.Thread(target=play_audio, args=(clip_to_play,)))
threads.append(threading.Thread(target=record_video))
map(lambda x: x.start(), threads)
audiofiles.remove(audiofiles[0])
print audiofiles
#below should run after everything
if len(audiofiles) == 0:
    time.sleep(26)
    create_video()
    upload_vid()
    for vid in videofiles:
	os.remove(directory + '/' + vid)
	print 'vids moved back'
    os.remove(directory + '/' + 'out.avi')
    played_directory = directory + '/played/'
    move_back = [n for n in os.listdir(played_directory) if n[0]=='h' and n[-4:]=='.mp3']
    for m in move_back:
	os.rename(played_directory + m, directory + '/' + m)
    print move_back

