# Getting Started
* Check your WiFi SSID and PASSWORD
* Edit the **SSID** and **PASS** in the *main.c*, change these lines:
```
const char *SSID = "YOUR_WIFI_SSID";    // SSID
const char *PASS = "YOUR_WIFI_PASS";    // Password
```


# Supported GET Commands

* **192.168.43.2/adcs** Get value of all ADCs
* **192.168.43.2/adc0** Get value of all ADC0
* **192.168.43.2/adc1** Get value of all ADC1
* **192.168.43.2/adc2** Get value of all ADC2
* **192.168.43.2/adc3** Get value of all ADC3

* **192.168.43.2/psws** Get status of all PSWs
* **192.168.43.2/psw0** Get status of PSW0
* **192.168.43.2/psw1** Get status of PSW1
* **192.168.43.2/psw2** Get status of PSW2
* **192.168.43.2/psw3** Get status of PSW3

* **192.168.43.2/leds** Get status of all LEDs
* **192.168.43.2/led0** Get status of LED0
* **192.168.43.2/led1** Get status of LED1
* **192.168.43.2/led2** Get status of LED2
* **192.168.43.2/led3** Get status of LED3

* **192.168.43.2/led0on** Turn on LED0
* **192.168.43.2/led1on** Turn on LED1
* **192.168.43.2/led2on** Turn on LED2
* **192.168.43.2/led3on** Turn on LED3

* **192.168.43.2/led0off** Turn off LED0
* **192.168.43.2/led1off** Turn off LED1
* **192.168.43.2/led2off** Turn off LED2
* **192.168.43.2/led3off** Turn off LED3

* **192.168.43.2/led0inv** Toggle LED0
* **192.168.43.2/led1inv** Toggle LED1
* **192.168.43.2/led2inv** Toggle LED2
* **192.168.43.2/led3inv** Toggle LED3
