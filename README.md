# Dualheading
Heading for autonomous driving, specially for AGopenGPS. 
With an ESP32 and 2 Ardusimple F9P, heading and Roll for a vehicle is calculated. 
A little instruction is at the begin of every sketch. 
In Version Dual_NTRIP you can use Ntrip directly with an handy hotspot or router.
You can input 5 Networks, for example, 5 different User with different handy hotspots.
The program scans WiFi and connect with the first found, if it fails, it takes Nrtip over AGopengps.
You can install a button, for scanning WiFi again.
Also a message is send back to Ntrip server. !!!!!
Single_Ntrip is similar to Dual, but only one ardusimple F9P.
This 2 versions work with USB
