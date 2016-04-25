from flask import Flask, request, session, g, redirect, url_for, \
     abort, render_template, flash
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
 
import requests

app = Flask(__name__)
app.config.from_object(__name__)

@app.route('/')
def map_page():
    return render_template('index.html')

@app.route('/add_marker')
def add_marker():
	lat = request.args.get('lat')
	lon = request.args.get('lon')
	data = {'lat':lat, 'lng': lon}
	r = requests.post('https://blistering-inferno-357.firebaseIO.com/coords.json', json=data)
	# r = requests.post('https://blistering-inferno-357.firebaseIO.com/coords.json', data = {'lat':lat, 'lng': lon}, verify=True)
	fromaddr = "radar.alert.cm@gmail.com"
	toaddr = "berkeley.asuc.ada@gmail.com"
	msg = MIMEMultipart()
	msg['From'] = fromaddr
	msg['To'] = toaddr
	msg['Subject'] = "RADAR Alert at (" + lat + "," + lon + ")"
	 
	body = "There was been a non-ADA accessible incident reported at (" + lat + "," + lon + "). To view the incident, please go to: ______"
	msg.attach(MIMEText(body, 'plain'))
	 
	server = smtplib.SMTP('smtp.gmail.com', 587)
	server.starttls()
	server.login(fromaddr, "sechromojuluming")
	text = msg.as_string()
	server.sendmail(fromaddr, toaddr, text)
	server.quit()
   	return "empty"

if __name__ == '__main__':
    app.run(debug=True)
