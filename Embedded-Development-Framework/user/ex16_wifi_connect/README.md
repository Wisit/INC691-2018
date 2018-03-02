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
 
