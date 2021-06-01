# Dualheading
Heading for autonomous driving, specially for AGopenGPS. 
With an ESP32 and 2 Ardusimple F9P, heading and Roll for a vehicle is calculated. 
A little instruction is at the begin of every sketch. 
In Version Dualheading you connect by USB or Ethernet an you can use Ntrip directly with an handy hotspot or router or by AGopenGPS over wifi or Ethernet.
This version is also for BENJAMINS PCB with two esp32 for AMATRON. 
When you want to use the second esp32, take the dual_1.13_F9P_AMA config.
For the 2.f9p the Program is 2._F9p_Baudwandler.
You can input 5 Networks, for example, 5 different User with different handy hotspots.
The program scans WiFi and connect with the first found, if it fails, it tries again.
You can install a button, for scanning WiFi again pin 4 connect to GND.
Also a message is send back to Ntrip server. !!!!!
Also you can update Over The Air, OTA
Single_Core_5 is only one ardusimple F9P and an esp32.
This 2 versions work with USB
The Photo is a symbol wireing. the Gpio Pin for LED is 2 and Gpio Pin for Ntrip is 4
Dualheading is a version where both cores are in use. One for WiFi and second for position.
The benefit is that you have no break in AgopenGPS when Wifi make a break.
Ntrip with APOS is possible.

