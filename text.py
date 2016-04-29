# Import smtplib for the actual sending function
import smtplib

# Import the email modules we'll need
# from email.mime.text import MIMEText
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
# Open a plain text file for reading.  For this example, assume that
# the text file contains only ASCII characters.
# fp = open(textfile, 'rb')
# Create a text/plain message
# msg = MIMEText(fp.read())
# fp.close()
msg = MIMEMultipart()

me = 'watch.owl911@gmail.com'
you = '4175761316@mms.att.net'
# you = '4087817156@vzwpix.com'
msg['Subject'] = 'SOS help Serena!'
msg['From'] = me 	
msg['To'] = you
body = "Serena needs your help she is here and just got attacked by the cookie monster. send sos pl0x"

msg.attach(MIMEText(body, 'plain'))
	 
server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
server.login(me, "sechalyeshkhprdh")
text = msg.as_string()
server.sendmail(me, you, text)
server.quit()

# Send the message via our own SMTP server, but don't include the
# envelope header.
# s = smtplib.SMTP('localhost')
# s.sendmail(me, [you], msg.as_string())
# s.quit()