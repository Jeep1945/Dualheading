# Dualheading
Heading for autonomous driving, specially for AGopenGPS. 
With an ESP32 and 2 Ardusimple F9P, heading and Roll for a vehicle is calculated. 
A little instruction is at the begin of every sketch. 
In Version Dual_NTRIP you can use Ntrip directly with an handy hotspot or router.
You can input 5 Networks, for example, 5 different User with different handy hotspots.
The program scans WiFi and connect with the first found, if it fails, it takes Nrtip over AGopengps.
You can install a button, for scanning WiFi again pin 4 connect to GND.
Also a message is send back to Ntrip server. !!!!!
Single_Ntrip is similar to Dual, but only one ardusimple F9P.
This 2 versions work with USB
The Photo is a symbll wireing. the Gpio Pin for LED is 2 and Gpio Pin for Ntrip is 4
Dual_COre_Beta5 is a testversion where both cores are use. One for WiFi and second for position.
The benefit is that you have no break in AgopenGPS when Wifi make a break.
Ntrip with APOS is possible.
