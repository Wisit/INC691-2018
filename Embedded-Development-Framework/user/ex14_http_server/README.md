# Getting Started
* Check your WiFi SSID and PASSWORD
* Edit the **SSID** and **PASS** in the *main.c*, change these lines:
```
const char *SSID = "YOUR_WIFI_SSID";    // SSID
const char *PASS = "YOUR_WIFI_PASS";    // Password
```

# System Indicators
* **LED3 flashing 5 Hz:** Cannot connect to the network or no IP address is provided
* **LED3 flashing 1 Hz:** Connected to network and got an IP address 
* **Low Frequency Beep:** Command timeout, the WiFi module not response

# Basic GET Commands
* **192.168.43.2/led0-on:** Turn on LED0
* **192.168.43.2/led1-on:** Turn on LED1
* **192.168.43.2/led2-on:** Turn on LED2
* **192.168.43.2/led3-on:** Turn on LED3
* **192.168.43.2/led0-off:** Turn off LED0
* **192.168.43.2/led1-off:** Turn off LED1
* **192.168.43.2/led2-off:** Turn off LED2
* **192.168.43.2/led3-off:** Turn off LED3
* **192.168.43.2/beep:** Beep sound
