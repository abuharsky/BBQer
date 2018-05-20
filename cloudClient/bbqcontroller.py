#!/usr/bin/python
import telebot, time, serial, threading, json, datetime, requests, configparser, os
from telebot import apihelper

#-------------------------
# config
#-------------------------

config = configparser.ConfigParser()
config.read('/root/BBQer/cloudClient/configuration.txt')

usbDevice = config.get('arduino', 'port')
usbSpeed  = config.get('arduino', 'baud')

chatId       = int(config.get('telegram', 'chatId'))
botToken     = config.get('telegram', 'botToken')
proxyType    = config.get('telegram', 'proxyType')
proxyAddress = config.get('telegram', 'proxyAddress')

if proxyType and proxyAddress:
	apihelper.proxy = {proxyType:proxyAddress}

dbName    = config.get('corlysis', 'db')
dbToken   = config.get('corlysis', 'token')

#---------
ser = serial.Serial(usbDevice, usbSpeed)
connected = False

bot = telebot.TeleBot(botToken)

#-------------------------
# corlysis
#-------------------------

def plot_json(data):
	obj = json.loads(data)

	# currenttime = '{0:%H:%M:%S}'.format(datetime.datetime.now())

	airflow  = obj["airflow"]["level"]
	damper   = obj["airflow"]["damper"]
	motor    = obj["airflow"]["motor"]
	curtemp  = obj["temperature"]["current"]
	probe0   = obj["temperature"]["probe0"]
	probe1   = obj["temperature"]["probe1"]
	target   = obj["temperature"]["target"]

	url = 'https://corlysis.com:8086/write'
	
	params = {"db": dbName, "u": "token", "p": dbToken}
	
	payload = "temperature_data current="+curtemp+",target="+target+",probe0="+probe0+",probe1="+probe1+"\nairflow_data level="+airflow+",damper="+damper+",motor="+motor+"\n"
	
	r = requests.post(url, params=params, data=payload)


#-------------------------
# serial
#-------------------------

def handle_data(data):
	print(data)
	
	# detect json data
	if (data.startswith( '{' )):
		plot_json(data)
	else:
		if ((chatId is 'none') == False):
			bot.send_message(chatId, 'serial: '+data)

def read_from_port(ser):
	global connected
	while not connected:
		#serin = ser.read()
		connected = True

		while True:
			reading = ser.readline().decode()
			handle_data(reading)

thread = threading.Thread(target=read_from_port, args=(ser,))
thread.start()


#-------------------------
# telegram bot
#-------------------------

@bot.message_handler(commands=['cputemp'])
def command_cputemp(m):
	if (chatId == m.chat.id):
		file = open("/etc/armbianmonitor/datasources/soctemp", "r")
		bot.send_message(chatId, "CPU temp:{:.1f}C".format(float(file.read())/1000))  

@bot.message_handler(commands=['serial'])
def command_serial(m):
	if (chatId == m.chat.id):
		ser.write(m.text[7:].encode())
		ser.write(b'\n')

bot.polling()

def main_loop():
	bot.polling(True)
	while 1:
		time.sleep(3)


if __name__ == '__main__':
	try:
		main_loop()
	except KeyboardInterrupt:
		print >> sys.stderr, '\nExiting by user request.\n'
		sys.exit(0)

