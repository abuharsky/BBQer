#!/usr/bin/python
import telepot, time, serial, threading, json, datetime, requests, configparser, os
from telepot.loop import MessageLoop

#read conf
config = configparser.ConfigParser()
config.read('/root/BBQer/cloudClient/configuration.txt')

usbDevice = config.get('arduino', 'port')
usbSpeed  = config.get('arduino', 'baud')

chatId    = config.get('telegram', 'chatId')
botToken  = config.get('telegram', 'botToken')

dbName    = config.get('corlysis', 'db')
dbToken   = config.get('corlysis', 'token')

#---------
ser = serial.Serial(usbDevice, usbSpeed)
connected = False
savedChatId = chatId

bot = telepot.Bot(botToken)


def plot_json(data):
    obj = json.loads(data)

    # currenttime = '{0:%H:%M:%S}'.format(datetime.datetime.now())

    airflow  = obj["airflow"]["level"]
    damper   = obj["airflow"]["damper"]
    motor    = obj["airflow"]["motor"]
    curtemp  = obj["temperature"]["current"]
    probe0   = obj["temperature"]["probe0"]
    target   = obj["temperature"]["target"]

    url = 'https://corlysis.com:8086/write'
    
    params = {"db": dbName, "u": "token", "p": dbToken}
    
    payload = "temperature_data current="+curtemp+",target="+target+",probe0="+probe0+"\nairflow_data level="+airflow+",damper="+damper+",motor="+motor+"\n"
    
    r = requests.post(url, params=params, data=payload)


def save_chat_id(chat_id):
	#print(chat_id)
	#    global savedChatId
	savedChatId = chat_id

def handle_data(data):
	print(data)
	
	# detect json data
	if (data.startswith( '{' )):
		plot_json(data)
	else:
		if ((savedChatId is 'none') == False):
			bot.sendMessage(savedChatId, 'serial: '+data)

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

def handle(msg):

	userName = msg['from']['first_name']+" "+msg['from']['last_name']

	content_type, chat_type, chat_id = telepot.glance(msg)

	save_chat_id(chat_id)

	if (content_type == 'text'):
		
		command = msg['text']
		print ('Got command: %s' % command)

		if  '/hello' in command:
			bot.sendMessage(chat_id, "Hello "+userName+"!")

		if '/serial' in command:
			ser.write(command[7:].encode())
			ser.write(b'\n')

		if '/cputemp' in command:
			file = open("/etc/armbianmonitor/datasources/soctemp", "r") 
			bot.sendMessage(chat_id, "CPU temp:{:.1f}C".format(float(file.read())/1000))  


# Add handle function to be called every received message.
MessageLoop(bot, handle).run_as_thread()

# Wait for new messages
while 1:
	time.sleep(20)

