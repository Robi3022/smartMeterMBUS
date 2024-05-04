
import os
import subprocess
import time
import sys
import string


evn_schluessel = "Place Her Your EVN Key in Hex   "
evn_schluessel = evn_schluessel.upper()



os.system("pkill -x stty")
os.system("pkill -x cat")
os.system("stty -F /dev/ttyUSB0 2400 raw parenb -parodd cs8 -cstopb -parmrk")
proc = subprocess.Popen("cat /dev/ttyUSB0", stdout=subprocess.PIPE, shell=True)


readCnt = 376

while 1:
	out = proc.stdout.read(readCnt)

	ofsCnt = 0
	for i in range(len(out)):
		if (len(out) > i+3 and out[i] == out[i+3] and out[i+1] == out[i+2] and out[i+1] == 0xFA):
			ofsCnt = i
			break

	if readCnt == 376: readCnt = 376 + ofsCnt
	else: readCnt = 376

	i= i+1
	daten = ''.join(format(x, '02X') for x in out)

	out = subprocess.Popen("../Converter " + daten + " " + evn_schluessel, stdout=subprocess.PIPE, shell=True)
	while out.poll() is None:
    		time.sleep(0.1)

	meterData = out.stdout.read()
	if (out.returncode != 0):
		print("Error-Code: " + str(out.returncode) + " with data:" + daten + "Msg: " + str(meterData))
		if readCnt == 376: readCnt = 400
	else:
		text_file = open("data.txt", "w")
		n = text_file.write("{" + meterData.decode('utf-8')+ "}")
		text_file.close()
