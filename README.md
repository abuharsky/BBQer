
> This is the first, alpha version of documentation. It will be improved. I'll waiting for your questions

# BBQer
Hello, my name is Alex, I'm from Russia and I love BBQ!
So, BBQ Smokers are not so popular in Russia, thats why I'am the best in my small town ( not serious :) )!

There are a lot of different handmade of factorymade BBQ Smoker temperature controllers, but for me all of them have too many limitations.
For example, for assembling very popular "HeaterMeter" you need to be an good engineer with soldering skills.
Or, you need to spent a lot of time to assemble, debug and configure some other DIY projects.
Or it cost too much!

As for me, this is not an easy way.

I hope, that I can explain to you, how to get very easy, cheap, really, extra-cheap controller with an optional cloud monitoring, that perfectly solve every BBQSmoking needs!

### Let's begin!


What's is the main problem for Low&Slow smoking?! Right, it takes too many hours! You need to constantly maintain temperature level and smoke density.

For maintaining constant temperature you need to always change an airflow using damper.

Airflow provide the main heat and smoke transfer from burning wood and charcoals to the meat.

![standart offset smoker](https://github.com/abuharsky/BBQer/raw/master/info/bbq_diagram1.png)

So, we need only to add a simple temperature sensor and electronically controlled air blower/damper.
Controller periodically reads temperature and adjust the airflow.

![standart offset smoker with temperature controller](https://github.com/abuharsky/BBQer/raw/master/info/bbq_diagram2.png)

That's how does it must work.

### Part 1. Arduino based controller

### Hardware

As I say before, I don't think that we need to be an engineer with soldering skills! You don't need to solder anything (maybe exept few connectors).

You need to buy:

1x [arduino nano (~3$)](https://ru.aliexpress.com/store/product/Arduino-Nano-3-0-Arduino-Nano-CH340-usb/1602024_32841140539.html?spm=a2g0v.12010615.0.0.7c3c4263StAGe8)

1x [arduino nano I/O extension board(~1$)](https://ru.aliexpress.com/item/1Pcs-I2C-NANO-I-O-Expansion-Sensor-Shield-Module-For-Arduino-UNO-R3-Nano-V3-0/32829846130.html)

1x [temperature sensor (K-type thermocouple with required amplifier)(~3$)](https://ru.aliexpress.com/item/MAX6675-Module-K-Type-Thermocouple-Temperature-Sensor/32698965438.html)

1x [blower fan(~2.5$)](https://ru.aliexpress.com/item/Brushless-DC-Blower-Cooling-Fan-Sleeve-Bearing-7525S-12V-0-18A-75x33mm-75mm/32731387269.html)

1x [mosfet driver(~0.5$)](https://ru.aliexpress.com/item/TOP-MOSFET-Button-IRF520-MOSFET-Driver-Module-ARM-Raspberry-pi/32662485350.html)

1x [wires (~3$)](https://ru.aliexpress.com/item/Dupont-Wire-Male-to-Male-Male-to-Female-Female-to-Female-Jumper-Cable-120x10cm/32729974130.html)

1x [power converter (~1$)](https://ru.aliexpress.com/item/XL6009-DC-Adjustable-Step-up-boost-Power-Converter-Module-Replace-LM2577/32636472543.html)

##### Total: ~14$ (without delivery)
(I recommend to use same supplier for faster delivery)




Also, you can optionally purchase:

1x [servo (~3$)](https://ru.aliexpress.com/item/MG90S-Micro-Servo-RC/32843925047.html?spm=a2g0s.9042311.0.0.jh7MDU) for more fine aiflow control

1x [thermo probe (~7$)](https://ru.aliexpress.com/item/Thermopro-Thermometer-extra-probe/32778050836.html?spm=a2g0s.9042311.0.0.jh7MDU) for food(meat) temperature control. You can connect up to 8 probes.

![wiring](https://github.com/abuharsky/BBQer/raw/master/info/pic2.jpg)



### Software

first, you need to add to your arduino *[SerialCommand](https://github.com/kroimon/Arduino-SerialCommand)* library

next, flash an arduino using this **repository/arduino/BBQController/**

next, open serial monitor (baud 115200) and type any of commands below:

* **target** <temp> - set target temp to specified value, this command start temperature controller, example: "target 110" - set the target temp to 110С or "target 0" - disables temperature controller

optional, manual managment commands:

* **temp** - print current temp from all sensors
* **servo** <angle> - move servo to specified angle
* **damper** <position> - open or close damper, position value between 0.0(closed) and 1.0(fully opened)
* **motor** <power> - powers the blower, power value between 0.0(disabled) and 1.0(full power)


This is enougth! You can setup your arduino controller, all values will be stored in EEPROM(energy independent memory). So, set the target temp to 110C (for brisket) and use great and cheap temperature controller.

I think, that we don't need to setup an additional display for arduino controller (Maybe I'll change my opinion in the future).
If you need display or some kind of user interface to this arduino controller, let's go to part 2!

### Part 2. Orange PI + cloud monitoring

![wiring](https://github.com/abuharsky/BBQer/raw/master/info/bbq_diagram3.jpg)


### Hardware

I recommend to use an orange pi lite (~12$).

### Software

As I say before, I don't want to setup direct display with some buttons on arduino. Because everyone already have display!
We will use your phones and tablets! And, of course, internet!

So, after you setup your PI (you can use one of thousands instructions from internet), you need to setup python3 and required packages, just type in the pi terminal the commands below:

> apt-get install python3
> 
> apt-get install python3-pip
> 
> pip3 install requests
> 
> pip3 install telepot
> 
> pip3 install pyserial

After installing, copy 
BBQer/cloudClient/bbqcontroller.py

BBQer/cloudClient/configuration_empty.txt

to your pi home dir, then, **rename configuration_empty.txt** to **configuration.txt** and fill required parameters

We will use telegram bot for sending commands to controller and receive information.

### Cloud monitoring

I recommend to use **corlysis.com** , free account is enough for our temperature monitoring.

![wiring](https://github.com/abuharsky/BBQer/raw/master/info/temperature_graph.png)

Setup database, than copy required db name and token to **configuration.txt**, than open grafana and import dashboard from **grafana.json**

Now, you can use your mobile phone to control your BBQ, view temperature graphs and receive notifications!

![wiring](https://github.com/abuharsky/BBQer/raw/master/info/pic1.jpg)