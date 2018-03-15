# BBQer
Arduino based BBQ smoker temperature controller with cloud analitics and notifications("add charcoal", "add wood", etc)

Hello, my name is Alex, I'm from Russia and I love BBQ!
So, BBQ Smokers are not so popular in Russia, thats why I'am the best in my small city ( not serious :) )!

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