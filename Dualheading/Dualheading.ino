TaskHandle_t Core1;
TaskHandle_t Core2;

// Dualhead for AGopenGPS
// Private-use only! (you need to ask for a commercial-use)
// by Franz Husch  31.12.2020 in cooperation with Matthias Hammer
// and Valentin Ernst
// **** OTA is possible over Network  **** //
// Antennas cross to driveDirection on cabin symmetrical
// right Antenna is Rover (A) for position, left Antenna is MB (B) for heading
// Ntrip client for 5 Router or Handy hotspots or from AgopenGPS
// Progamm tries one time to connect,
// you can install a button from GND to PIN 4
// by pressing the button a new WiFi scan starts.
// PIN 2 you can add an LED for ntrip is received, have a look to the Photo
// in AgopenGPS  offset is ZERO
// WiFi_scan_Delay is the mount of sec, you will need start router or hotspot
// in Data Sources
///by Fix "OGI", Heading GPS "Dual", Heading Correction Source "None", Roll "OGI"
// in Neigung/Richtung
// Bei Fix "OGI", Richtung GPS "Dual", Richtungskorrecturquelle "keine", Neigung "OGI"
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++to adjust++++++++++++++++++++++++++++++++++  */

//byte Eth_myip[4] = { 192, 168, 1, 11 };//IP address to send UDP data to
byte Eth_myip[4] = { 10, 0, 0, 22 };//IP address to send UDP data via router to tablett
int AntDistance = 95;       // distance between the two antennas in cm,+, 0 for automatic distance
double headingcorr = 90;     // right antenna A , left antenna B;
int tractorhight = 280;   // roll is in Position calculated, in AgOpenGPS mit 10cm
int WiFi_scan_Delay = 1;      // for router use 50 sec delay
//bool send_amatron_nmea = true;   // true for sending, false for not
bool send_amatron_nmea = false;    // true for sending, false for not
int send_Data_Via = 0;       // send Data via  0 = USB, 1 = Ethernet
byte Eth_CS_PIN = 5;       //CS PIN with SPI Ethernet hardware W 5500  SPI config: MOSI 23 / MISO 19 / CLK18 / CS5, GND, 3.3V
//  IMPORTANT  // For serial USB 38400 baude not 115400


int Ntriphotspot = 0;  // 0 = Ntrip from AGopenGPS, 1 = Ntrip direct with hotspot or router
// with Ntriphotspot = 0, no OTA possible

char WIFI_Network1[24] = "";            // WiFi network Client name
char WIFI_Password1[24] =  "";        // WiFi network password
char WIFI_Network2[24] =  "";            // WiFi network Client name
char WIFI_Password2[24] =  "";        // WiFi network password
char WIFI_Network3[24] =  "";            // WiFi network Client name
char WIFI_Password3[24] =  "";        // WiFi network password
char WIFI_Network4[24] =  "";            // WiFi network Client name
char WIFI_Password4[24] =  "";        // WiFi network password
char WIFI_Network5[24] =  "";            // WiFi network Client name
char WIFI_Password5[24] =  "";        // WiFi network password
char WIFI_Network6[24] =  "";            // WiFi network Client name
char WIFI_Password6[24] =  "";        // WiFi network password

#define WIFI_TIMEOUT_MS 50000
int GGA_Send_Back_Time = 0;  // after how many seconds a GGA msg is send back to Nripserver

char Ntrip_host[40] = "";  //"ntrip caster host";
char Ntrip_mntpnt[40] = "";         //"ntrip caster's mountpoint";
char Ntrip_user[40] = "";       //"ntrip caster's client user";
char Ntrip_passwd[40] = "";           //"ntrip caster's client password";

int Ntrip_httpPort = 2101;              //port 2101 is default port of NTRIP caster

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
//libraries -------------------------------
#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include "z_NTRIPClient.h"
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <AsyncUDP.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>



//connection plan:
// ESP32--- Right F9P GPS pos --- Left F9P Heading-----Sentences
//  RX1----------TX1--------------------------------UBX-Nav-PVT out   (=position+speed)
//  TX1----------RX1--------------------------------RTCM in           (NTRIP comming from AOG to get absolute/correct postion
//  RX2--------------------------------TX1----------UBX-RelPosNED out (=position relative to other Antenna)
//  TX2--------------------------------RX1----------
//               RX2-------------------TX2----------RTCM 1005+1074+1084+1094+1124+1230 (activate in PVT F9P!! = NTRIP for relative positioning)

// IO pins ----------------------------------------------------------------------------------------
#define RX0   3 //6; //USB
#define TX0   1 //5; //USB

#define RX1   27 //16,27;  //simpleRTK TX 1. Antenna = NMEA
#define TX1   16 //17,16;  //simpleRTK RX 1. Antenna = RTCM

#define RX2   25 //25,25;  //simpleRTK TX1 2. Antenna; UBX
#define TX2   17 //27,17;  //simpleRTK RX1 2. Antenna; free


//loop time variables in microseconds
long lastTime = 0;
float roll = 0;
byte XOR;
char c;
char b;
String t;
byte UDPPAOGIMsg[120];


// Ntrip
NTRIPClient ntrip_c;
byte WiFi_netw_nr = 0; 
String RTCM_Packet;
unsigned long startSend_back_Time = millis();
unsigned long ntriptime_from_AgopenGPS = millis();
unsigned long WiFi_scan_Delay_Time = millis();
unsigned long Amatron_begin_Time = millis();
unsigned long GPSagetime = millis();
unsigned long Ntrip_begin_Time = millis();

bool OTA_update = false;
int wait = 30000, WiFi_scan_Versuch = 1;
int durchlauf_nord = 0, durchlauf_east = 0;
double nordWinkel_old, eastWinkel_old;


int ntrip_from_AgopenGPS = 0, ntriptime_from_ESP32 = 0;
bool network_found = false, NtripCheckTrue = false;
int buttonState = 0, Ntriphotspotoriginal = 1;
byte IPadress[4] = { 0, 0, 0, 0 };
int net_found = 0, Ntriphotspot_an = 0;
#define Button_ReScan 4  // pin 4  if button pressed, WiFi scan is starting
#define LED_ntrip_ON  2  // pin 2  if ntrip on without AGopenGPS


// Ethernet
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xB3, 0x1B}; // original

byte Eth_ipDest_ending = 100;//ending of IP address to send UDP data to
unsigned int portMy = 5544;             //this is port of this module: Autosteer = 5577 IMU = 5566 GPS =
unsigned int AOGNtripPort = 2233;       //port NTRIP data from AOG comes in
unsigned int portDestination = 9999;    //Port of AOG that listens
bool Ethernet_running = false;
char Eth_NTRIP_packetBuffer[512];// buffer for receiving and sending data
byte ReplyBufferPAOGI[120] = "";        // a string to send back
unsigned int Bytelaenge;
int m;

IPAddress Eth_ipDestination;

// turn a round time
int Starttime = millis(), Countdurch = 0;
int Core1_time = millis(), Core1_break = 10000;

// Heading
double heading, headingUBX, headingzuvor = 0, headingzuvorVTG;
double headingUBXmin, headingUBXmax, headingVTGmin, headingVTGmax;
double speeed = 0, speeed1 = 0, headingnord, speeedzuvor = 0, speeedmin = 0.3, speeedmax = 0.3;


// roll
int rollaktiv = true;     // true roll activated
float rollCorrectionDistance = 0.00;
double rollnord = 0.0, rolleast = 0.0;
double rollnord_before = 0.0, rolleast_before = 0.0;
double relPosD, relPosDH;
double rollzuvor = 0;
double PI180 = 57.295791;
double baseline, baseline1, baselineHorizontal;

byte CK_A = 0, CK_B = 0;
byte incoming_char;

// NMEA erstellen
int inByte, start = 0, GGASats;
String nmea = "", GGAdaten = "", GGAdaten1 = "", VTGdaten = "", VTGspeed = "", VTGheadingnord = "";
String VTGSatz = "", GGASatz_old = "", GGAnord = "", GGAeast = "", GGAZeit = "", GGAWestEast = "", GGANordSued = "";
int j = 0, j2 = 0, jGGA = 0, jGGA2 = 0, jGGA3 = 0, jGGA4 = 0, jGGA5 = 0, jGGA6 = 0, jGGA7 = 0, jGGA78 = 0;
int jVTG1 = 0, jVTG2 = 0, jVTG3 = 0, jVTG4 = 0, jVTG5 = 0;
String GPSquali = "", WEcoordinaten, NScoordinaten, GGASat, GGAHDop;
String GGASatz = "", GGASatz_Korr, VTGSatz_Korr = "", GGASatz_send_back1 = "", GGASatz_send_back2 = "";
int GPSqualin1 = 0, GPSqualin2 = 1, GPSqualinzuvor = 1, GPSqualintime = 1, GGA_check = 0;
String ZDASatz = "", GGA_hDop, GGA_seahigh;
int  i = 0, ij = 0;
double GGAZeitNummerbevor, GGAZeitNummer;
double GGA_hDops, GGAage, GGA_seahighs;


// PAOGI erstellen
bool Paogi_true = true;
String RollHeadingrest = "", RollHeadingrest_befor = "", BS =",";
int Paogi_Long, Coodinate_check1, Coodinate_check2, heading_check1 = 0;
int Paogi_Shit = 0;

// Chechsum controll
String checksum = "", checksum_GGA = "", checksum_GGA_send = "", checksum_VTG = "", checksum_VTG_send = "";
String check_headingroll = "";
int j_checksum_GGA = 0, j_checksum_VTG = 0;

union UBXMessage {
  struct {
    unsigned char HeaderA;
    unsigned char HeaderB;
    unsigned char cls;
    unsigned char id;
    unsigned short len;
    unsigned char reserved;
    unsigned char reserved1;
    unsigned short refStationId;
    unsigned long iTOW;
    long relPosN;
    long relPosE;
    long relPosD;
    long relPosLength;
    long relPosHeading;
    unsigned long reserved2;
    char relPosHPN;
    char relPosHPE;
    char relPosHPD;
    char relPosHPLength;
    unsigned long accN;
    unsigned long accE;
    unsigned long accD;
    unsigned long accLength;
    unsigned long accHeading;
    unsigned long reserved3;
    unsigned long flags;
  } relposned;
  byte rawBuffer[72];
} ubxmessage;

union UBXMessagePVT {
  struct {
    uint8_t cls;
    uint8_t id;
    uint16_t len;
    unsigned long iTOW;  //GPS time ms
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t valid;
    unsigned long tAcc;
    long nano;
    uint8_t fixType;//0 no fix....
    uint8_t flags;
    uint8_t flags2;
    uint8_t numSV; //number of sats
    long lon;   //deg * 10^-7
    long lat;   //deg * 10^-7
    long height;
    long hMSL;  //heigt above mean sea level mm
    unsigned long hAcc;
    unsigned long vAcc;
    long velN;
    long velE;
    long velD;
    long gSpeed; //Ground Speed mm/s
    long headMot;
    unsigned long sAcc;
    unsigned long headAcc;
    uint16_t pDOP;
    uint8_t flags3;
    uint8_t reserved1[5];
    long headVeh;
    int16_t magDec;
    uint16_t magAcc;
    uint8_t CK0;
    uint8_t CK1;
  } PVT;
  byte rawBufferPVT[150];
} ubxmessagePVT;

//UBX
double speedUBXint;
String speedUBXstr;


//bool debugmode = true;  // GGA,VTG,
bool debugmode = false;
//bool debugmode1 = true;  // Heading
bool debugmode1 = false;
//  bool debugmode2 = true;  // Deviation
bool debugmode2 = false;
//  bool debugmode3 = true;  // roll
bool debugmode3 = false;
//bool debugmode4 = true;  //
bool debugmode4 = false;
//  bool debugProtokoll = true;  //Protocoll TestStation
bool debugProtokoll = false;
//bool debugmode_amatron = true;  //Protocoll Amatron
bool debugmode_amatron = false;

AsyncUDP udpNtrip;
EthernetUDP Eth_udpPAOGI;
EthernetUDP Eth_udpNtrip;
WiFiClient client;



// Setup procedure ------------------------
void setup() {
  ubxmessage.rawBuffer[0] = 0xB5;
  ubxmessage.rawBuffer[1] = 0x62;
  ubxmessage.rawBuffer[2] = 0x01;
  ubxmessage.rawBuffer[3] = 0x3C;

  ubxmessagePVT.rawBufferPVT[0] = 0xB5;
  ubxmessagePVT.rawBufferPVT[1] = 0x62;
  ubxmessagePVT.rawBufferPVT[2] = 0x01;
  ubxmessagePVT.rawBufferPVT[3] = 0x07;


  delay(10);
  Serial.begin(38400);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.println("Start setup");
  Serial1.begin(115200, SERIAL_8N1, RX1, TX1);
  delay(10);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);

  pinMode(Button_ReScan, INPUT_PULLUP);
  pinMode(LED_ntrip_ON, OUTPUT);
  digitalWrite(LED_ntrip_ON, HIGH);

  startSend_back_Time = millis() - (GGA_Send_Back_Time * 1000);
  ntriptime_from_AgopenGPS = millis();
  ntriptime_from_ESP32 = millis();
  WiFi_scan_Delay_Time = millis();
  Amatron_begin_Time = millis();

  delay(100);
  if  ((WIFI_Network1 == "") || (Ntriphotspot == 0)) {
    Ntriphotspot = 0;
  }
  else {
    Serial.println("Wait for Wifi_Scan; Delay_Time");
  }
  if  ((send_Data_Via == 0) && (Ntriphotspot == 0)) {
    wait = 500;
  }

  Serial.println("");
  Serial.println("End setup");
  Serial.println("");

  Ntriphotspotoriginal = Ntriphotspot;
  //------------------------------------------------------------------------------------------------------------
  //create a task that will be executed in the Core1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(Core1code, "Core1", 5000, NULL, 1, &Core1, 0);
  delay(wait);
  //create a task that will be executed in the Core2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(Core2code, "Core2", 5000, NULL, 1, &Core2, 1);
  delay(10);
  //------------------------------------------------------------------------------------------------------------

}

void loop() {
//  if (OTA_update) ArduinoOTA.handle();
  delay(10);

}
