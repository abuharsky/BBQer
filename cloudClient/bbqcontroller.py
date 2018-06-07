#!/usr/bin/python
import serial, time, threading, json, datetime, requests, configparser, os, queue, logging
#-------------------------
# config
#-------------------------

config = configparser.ConfigParser()
config.read('/root/BBQer/cloudClient/configuration.txt')

usbDevice = config.get('arduino', 'port')
usbSpeed  = config.get('arduino', 'baud')

chatId       = config.get('telegram', 'chatId')
botToken     = config.get('telegram', 'botToken')
proxyType    = config.get('telegram', 'proxyType')
proxyAddress = config.get('telegram', 'proxyAddress')

proxy = {}
if proxyType and proxyAddress:
	proxy = {proxyType:proxyAddress}

dbName    = config.get('corlysis', 'db')
dbToken   = config.get('corlysis', 'token')

networkLostCommand = config.get('system', 'runCommandOnNoInternet')
networkRestartRetriesBeforeRestart = 0
networkRestartMaxRetries = 5
networkErrorsCounter = 0 #count every send operation error
networkMaxErrorCount = 5

#---------
serial_port = serial.Serial(usbDevice, usbSpeed)

main_thread_queue = queue.Queue()
network_thread_queue = queue.Queue()

logging.basicConfig(filename="/root/BBQer/cloudClient/logs/bbqcontroller"+str(time.time())+".log", level=logging.INFO)


def run_on_main_thread(function):
	main_thread_queue.put(function)

def run_on_network_thread(function):
	network_thread_queue.put(function)

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
	
	global networkErrorsCounter
	try:
		resp = requests.post(url, params=params, data=payload, timeout=10)
		resp.raise_for_status()
	except Exception as err:
		networkErrorsCounter += 1
		logging.info('Error sending plot data: ' + str(err))
		print('Error sending plot data: ' + str(err) + '\n' + payload)


#-------------------------
# serial
#-------------------------

def write_line_to_serial_port(line):
	logging.info('[serial] <- ' + line)
	print('[serial] <- ' + line)
	serial_port.write(line.encode())
	serial_port.write(b'\n')

def handle_serial_port_data(data):
	logging.info('[serial] -> ' + data)
	print('[serial] -> ' + data)
	
	# detect json data
	if (data.startswith( '{' )):
		run_on_network_thread(lambda: plot_json(data))
	else:
		run_on_network_thread(lambda: telegram_send_message(chatId, 'serial: '+data))

def read_from_serial_port(serial_port):
	while True:
		line = serial_port.readline().decode()
		run_on_main_thread(lambda: handle_serial_port_data(line))
		time.sleep(1)

thread_serial_port = threading.Thread(target=read_from_serial_port, args=(serial_port,))
thread_serial_port.start()


#-------------------------
# telegram bot
#-------------------------

def telegram_send_message(to, text):
	global networkErrorsCounter
	try:
		resp = requests.get('https://api.telegram.org/bot' + botToken + '/sendMessage?chat_id=' + to + '&text=' + text, proxies=proxy, timeout=10)
		resp.raise_for_status()
	except Exception as err:
		networkErrorsCounter += 1
		logging.info('Error sending message to telegram ' + str(err))
		print('Error sending message to telegram ' + str(err))

def telegram_get_updates():
	offset = 0
	while True:
		try:
			resp = requests.get('https://api.telegram.org/bot' + botToken + '/getUpdates?timeout=30&allowed_updates=["message"]&offset=' + str(offset), proxies=proxy, timeout=30)
			if resp.status_code == 200:
				offset = process_telegram_updates_response(resp.json())
			else:
				resp.raise_for_status()
		except requests.exceptions.Timeout:
			time.sleep(3) #no updates, run next iteration
			continue
		except Exception as err:
			logging.info('telegram "getUpdates" error: ' + str(err))
			print('telegram "getUpdates" error: ' + str(err))
			time.sleep(15)
			continue
		time.sleep(1)

def process_telegram_updates_response(json):
	offset = 0
	updates = json["result"]
	for index, update in enumerate(updates):
		run_on_main_thread(lambda: process_telegram_message(update["message"]))
		offset = int(update["update_id"])
	return offset + 1

def process_telegram_message(message_json):
	if chatId == str(message_json["from"]["id"]): 
		text = message_json["text"]
		logging.info('[telegram] ' + text)
		print('[telegram] ' + text)
		if text.startswith('/cputemp'):
			file = open("/etc/armbianmonitor/datasources/soctemp", "r")
			run_on_network_thread(lambda: telegram_send_message(chatId, "CPU temp:{:.1f}C".format(float(file.read())/1000)))  
		elif text.startswith('/serial'):
			run_on_main_thread(lambda: write_line_to_serial_port(text[8:]))

thread_telegram_bot = threading.Thread(target=telegram_get_updates)
thread_telegram_bot.start()


#-------------------------
# internet connection 
# checker
#-------------------------

def connected_to_internet():
	global networkErrorsCounter
	if networkErrorsCounter > networkMaxErrorCount:
		print("Too many network errors")
		networkErrorsCounter = 0
		return False
	
	try:
		response = requests.get('https://api.telegram.org/bot' + botToken + '/getMe', proxies=proxy, timeout=10)
		response.raise_for_status()
		return True
	except Exception as e:
		logging.info("No internet connection available. " + str(e))
		print("No internet connection available. " + str(e))
	return False

def check_internet_connection_and_reconnect():
	while True:
		print('checking internet')
		if not connected_to_internet():
			global networkRestartRetriesBeforeRestart
			if (networkRestartRetriesBeforeRestart < networkRestartMaxRetries):
				logging.info('no internet, running command')
				print('no internet, running command')
				networkRestartRetriesBeforeRestart += 1
				os.system(networkLostCommand)
				run_on_network_thread(lambda: telegram_send_message(chatId, 'reconnected to the internet.'))
			else:
				logging.info('no internet, rebooting')
				print('no internet, rebooting')
				os.system('reboot')
		else:
			networkRestartRetriesBeforeRestart = 0
			print('internet OK')
		time.sleep(60)


thread_check_internet = threading.Thread(target=check_internet_connection_and_reconnect)
thread_check_internet.start()

#-------------------------
# main loop and  
# threaded task processing
#-------------------------

def queue_loop(queue):
	while True:
		print('get next task')
		nextTask = queue.get() #blocking
		try:
			nextTask()
		except Exception as e:
			logging.info("task error:" + str(e))
			print("task error:" + str(e))
			print(nextTask)

		queue.task_done()

		time.sleep(1)

thread_network = threading.Thread(target=queue_loop, args=(network_thread_queue,))
thread_network.start()


if __name__ == '__main__':
	try:
		queue_loop(main_thread_queue)
	except KeyboardInterrupt:
		print('Exiting by user request.')
		pass

