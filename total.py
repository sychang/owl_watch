from flask import Flask, render_template
import smtplib
import RPi.GPIO as GPIO
import time
import pygame
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
import os
from email import Encoders

GPIO.setmode(GPIO.BCM)
app = Flask(__name__)

@app.route("/")
def hello():
    return render_template("index.html")

@app.route("/lookout")
def alert():
    play_audio('owl.mp3')

@app.route("/led/")
def toggle_led():
    global looping
    looping = not looping
    threading.Thread(target=loop_till_false).start()
    return str(looping)

def loop_till_false():
    while looping:
        GPIO.output(17, True)
        time.sleep(0.1)
    GPIO.output(17, 0)

#PLAY AUDIO CLIP
def play_audio(clip_to_play):
  print 'play audio start'
  pygame.mixer.init()
  pygame.mixer.music.load(clip_to_play)
  pygame.mixer.music.play()
  while pygame.mixer.music.get_busy() == True:
    continue

def send_text(me, you, attachment=None):
    msg = MIMEMultipart()
    me = 'watch.owl911@gmail.com'
    you = '4175761316@mms.att.net'
    # you = '4087817156@vzwpix.com'
    msg['Subject'] = 'Allison is in danger!'
    msg['From'] = me    
    msg['To'] = you
    body = "Allison is at (37.8761032, -122.25969880000002)"

    msg.attach(MIMEText(body, 'plain'))
    if attachment is not None:
        part = MIMIBase('application', "octet-stream")
        part.set_payload(open(attachment, "rb").read())
        Encoders.encode_base64(part)
        part.add_header('Content-Disposition', 'attachment; filename="%s"' % os.path.basename(attachment))
        msg.attach(part)
         
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    server.login(me, "sechalyeshkhprdh")
    text = msg.as_string()
    server.sendmail(me, you, text)
    server.quit()


if __name__ == "__main__":
    #run web browser toggle button
    #to blink the light red
    # GPIO.setwarnings(False)
    # GPIO.setmode(GPIO.BCM)
    # GPIO.setup(17, GPIO.OUT)
    # GPIO.output(17, 0)
    #to play a sound clip

    looping = False
    app.run(debug=True, host="0.0.0.0") 


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
        timestamp = 0
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
            #sends email with address for viewing 
            send_text('watch.owl911@gmail.com', '4175761316@mms.att.net')
            #button press activates something to start recording the video 
            #ideally with GPS and message !!!!!!!!
            time.sleep(0.2)
        elif report_button == False:
            print('Report Button Pressed')
            play_audio('report.mp3')
            time.sleep(0.2)
        
        #if distance < 10:
            #not manually shut off within x time, send alert
            #speaker gets really loud
        if distance < 100:
            GPIO.output(red, 1)
            play_audio('owl.mp3') #play warning sound
            #activates timed warning            
        elif distance < 200:
            GPIO.output(green, 1)
        elif distance < 300:
            GPIO.output(blue, 1)
            
        print "Distance:",distance,"cm"
        time.sleep(0.1)

    GPIO.cleanup()



