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

1x [servo (~3$)](https://ru.aliexpress.com/item/MG90S-Micro-Servo-RC/32843925047.html?spm=a2g0s.9042311.0.0.jh7MDU)

1x [thermo probe (~7$)](https://ru.aliexpress.com/item/Thermopro-Thermometer-extra-probe/32778050836.html?spm=a2g0s.9042311.0.0.jh7MDU)  

