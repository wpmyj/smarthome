# python /home/pi/smartHome/dnsget/dnsget.py

#import urllib,smtplib
#from email.mime.text import MIMEText

import urllib

from smtplib import SMTP_SSL as SMTP       # this invokes the secure SMTP protocol (port 465, uses SSL)
#from smtplib import SMTP                  # use this for standard SMTP protocol   (port 25, no encryption)
from email.MIMEText import MIMEText

def getMyIp():
    try:
        #return urllib.urlopen("http://www.instrument.com.cn/ip.aspx").read()
        return urllib.urlopen("http://myip.dnsdynamic.org/").read()
    except Exception, e:
        return "error"

def sendEmail(ipAddr, result):
    sender = 'haonotifier@yeah.net'
    receiver = 'hchanghao@163.com'
    subject = 'IP changed'
    smtpserver = 'smtp.yeah.net'
	smtpport = 465
    username = 'haonotifier@yeah.net'
    password = 'ocdNew2'
	
	result = True
	
    try:
        #msg = MIMEText("<html>The result is [" + result + "].<br><br>The new IP is <a href='" + ipAddr + ":9033'>" + ipAddr + ":9033</a></html>", "html", "utf-8")
		content = "The result is [" + result + "].\r\n\r\nThe new URL is http://" + ipAddr + ":9033"
		msg = MIMEText(content, 'plain')
        msg['Subject'] = subject
		msg['From'] = sender
		msg['To'] = receiver
        #smtp = smtplib.SMTP()
		#smtp.connect(smtpserver)
		smtp = SMTP_SSL(smtpserver, smtpport )
		smtp.set_debuglevel(False)
		smtp.ehlo
        smtp.login(username, password)
        smtp.sendmail(sender, receiver, msg.as_string())
		#print 'a-'+ msg.as_string()
        smtp.quit()
	finally:
		smtp.close()
		

    except Exception, e:
        print str(e)
        result=False
	
	return result



myIp=getMyIp()
if "error" in myIp:
    result = "Error happened when retrieving my IP."
else:
    try:
        result=urllib.urlopen("http://hchanghao:3303272@dynupdate.no-ip.com/nic/update?hostname=hao.myftp.org&myip=" + myIp).read()
	#result=urllib.urlopen("https://hchanghao@163.com:3303272q@www.dnsdynamic.org/api/?hostname=hao.dnsget.org&myip=" + myIp).read()
    except Exception, e:
        result="Error happened when updating IP address."

#print result
#print "good" in result
#print "nochg" in result

if "nochg" in result:
    print "no change"
else:
    print "changed or something wrong"
    sendEmail(myIp, result)

