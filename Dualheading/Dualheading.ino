
String VERS = "Version Dual 1.1   14.01.2022";

// Dualhead for AGopenGPS
// Private-use only! (you need to ask for a commercial-use)
// by Franz Husch  31.12.2020 in cooperation with Matthias Hammer
// and Valentin Ernst
// **** OTA is possible over Network  **** //
// Antennas cross to driveDirection on cabin symmetrical
// right Antenna is Rover (A) for position, left Antenna is MB (B) for heading
// Ntrip client for 7 Router or Handy hotspots or from AgopenGPS
// Progamm tries one time to connect,
// you can install a button from GND to PIN 4
// by pressing the button a new WiFi scan starts or Accesspoint for OTA.
// PIN 2 you can add an LED for ntrip is received, have a look to the Photo
// LED is on when no Network could be connected
// Dual_Antenna  0: for single antenna, 1: for Dualantenna
// send_amatron_nmea  to send GGA,VTG,GSA and ZDA to a second ESP32 0: for not 1: for sending,
// rollaktiv (0) for use roll in AgopenGPS, offset and hight of Antenna set in setup AOG
// rollaktiv (1) for use roll by ESP32, offset and hight of Antenna set to 0 in setup AOG
// WiFi_scan_Delay is the mount of sec, you will need start router or hotspot
// in Data Sourcess
// rollaktiv is 0, is done in AOG, so Antenna offset and hight is to do in AOG
// GGA_Send_Back_Time for SAPOS or Apos set to 10,
///by Fix "OGI", Heading GPS "Dual"

//  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  +++++++++++++++++++++++++++++++  BEGIN Setup +++++++++++++++++++++++++++++++++++++++
//  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int AntDistance = 95;       // distance between the two antennas in cm,+, 0 for automatic distance
double headingcorr = 90;     // right antenna A , left antenna B;
int tractorhight = 280;   // roll is in Position calculated, in AgOpenGPS mit 0 cm
int WiFi_scan_Delay = 1;      // for router use 50 sec delay
int Dual_Antenna = 1;  // 1: for Dualantenna, 0: for single antenna;
int send_amatron_nmea = 0;    // 1: for sending, 0: for not
byte Eth_CS_PIN = 5;       //CS PIN with SPI Ethernet hardware W 5500  SPI config: MOSI 23 / MISO 19 / CLK18 / CS5, GND, 3.3V
int rollaktiv = 0;     // 0: roll in AOG  1: roll activated in Dualheading
//  IMPORTANT  // For serial USB 38400 baude not 115400


int send_Data_Via = 0;       // send Data via  0: USB, 1: Ethernet, 2: WiFi with router and Ntrip from AOG

int Ntriphotspot = 0;  // 0: Ntrip from AOG(USB or by Ethernet)   1: Ntrip by Ethernet via Router
//                        2: Ntrip by WiFi via Hotspot or Router  3: Ntrip by WiFi via Router from AOG

//  if router exists, use 1. Network for him
//  2 - 7 Network and Passwords for Hotspots
char WIFI_Network1[24] =  "";           // WiFi network for router
char WIFI_Password1[24] = "";             // WiFi password for router
char WIFI_Network2[24] = "";            // WiFi network Client name
char WIFI_Password2[24] =  "";        // WiFi network password
char WIFI_Network3[24] =  "";            // WiFi network Client name
char WIFI_Password3[24] =  "";        // WiFi network password
char WIFI_Network4[24] =  "";            // WiFi network Client name
char WIFI_Password4[24] =  "";        // WiFi network password
char WIFI_Network5[24] =  "";            // WiFi network Client name
char WIFI_Password5[24] =  "";        // WiFi network password
char WIFI_Network6[24] =  "";            // WiFi network Client name
char WIFI_Password6[24] =  "";        // WiFi network password
char WIFI_Network7[24] =  "";            // WiFi network Client name
char WIFI_Password7[24] =  "";        // WiFi network password

#define WIFI_TIMEOUT_MS 50000
int GGA_Send_Back_Time = 0;  // after how many seconds a GGA msg is send back to Nripserver


char Ntrip_host[40] = "";  //"ntrip caster host";
char Ntrip_mntpnt[40] = "";         //"ntrip caster's mountpoint";
char Ntrip_user[40] = "";       //"ntrip caster's client user";
char Ntrip_passwd[40] = "";           //"ntrip caster's client password";
int Ntrip_httpPort = 2101;              //port 2101 is default port of NTRIP caster

//Accesspoint name and password for OTA
char ssid_ap[24] = "Autonomes Lenken";  // name of Access point, if no WiFi for OTA found
int timeoutRouter = 600;                 //time (s) to hold AP for OTA


//  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  ++++++++++++++++++++++++++++++++  END Setup  +++++++++++++++++++++++++++++++++++++++
//  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


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
#define RX0   3 //USB
#define TX0   1 //USB

#define RX1   27 // simpleRTK TX 1. Antenna = NMEA
#define TX1   16 // simpleRTK RX 1. Antenna = RTCM

#define RX2   25 // simpleRTK TX1 2. Antenna; UBX
#define TX2   17 // simpleRTK RX1 2. Antenna; free


//loop time variables in microseconds
long lastTime = 0;
float roll = 0;
byte XOR;
char c;
char b;
String t;
bool OTA_update = true;
bool Ethernet_need = false, Ethernet_need_AOG = false;
bool Ntrip_AOG_USB = false, Ntrip_AOG_Eth = false, Ntrip_WiFi = false, Ntrip_Eth_router = false;
String Conn = " all right";


// Ntrip WiFi
byte WiFi_netw_nr = 0;
String RTCM_Packet;
unsigned long startSend_back_Time = millis();
unsigned long ntriptime_from_AgopenGPS = millis();
unsigned long WiFi_scan_Delay_Time = millis();
unsigned long Amatron_begin_Time = millis();
unsigned long Ntrip_begin_Time = millis();
unsigned long OTA_begin_Time = millis();
unsigned long Single_begin_Time = millis();
unsigned long Single_begin_Time_VTG = millis();
int durchlauf_nord = 0, durchlauf_east = 0, WiFi_scan_Attempt = 1;
double nordWinkel_old, eastWinkel_old;
int ntrip_from_AgopenGPS = 0;
bool network_found = false;
int buttonState = 0, Ntriphotspotoriginal = 1;
byte IPadress[4] = { 0, 0, 0, 0 };
int net_found = 0, Ntriphotspot_an = 0;
#define Button_ReScan 4  // pin 4  if button pressed, WiFi scan is starting
#define LED_ntrip_ON  2  // pin 2  if ntrip on without AGopenGPS

//static IP for WiFi to Router
byte myip[4] = { 192, 168, 1, 79 };     // Roofcontrol module
byte gwip[4] = { 192, 168, 1, 1 };      // Gateway IP also used if Accesspoint created
byte mask[4] = { 255, 255, 255, 0 };
byte myDNS[4] = { 8, 8, 8, 8 };         //optional
byte ipDestination[4] = { 192, 168, 1, 255}; //IP address of router to send UDP data to
//byte ipDestination[4] = { 10, 0, 0, 18};       //IP address of router to send UDP data to
byte myIPEnding = 79;             //ending of IP adress x.x.x.79 of ESP32

// Ethernet
byte Eth_myip[4] = { 192, 168, 1, 80 };//IP address to send UDP data to
//byte Eth_myip[4] = { 10, 0, 0, 22 };//IP address to send UDP data via router to tablett
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xB3, 0x1B}; // original
byte Eth_ipDest_ending = 100; //ending of IP address to send UDP data to router
unsigned int portMy = 5544;             //this is port of this module: Autosteer = 5577 IMU = 5566 GPS =
unsigned int AOGNtripPort = 2233;       //port NTRIP data from AOG comes in
unsigned int portDestination = 9999;    //Port of AOG that listens
unsigned int localPort = 8888;       // local port to listen for UDP packets
bool Ethernet_running = false;
char Eth_NTRIP_packetBuffer[512];// buffer for receiving and sending data
byte ReplyBufferPAOGI[120] = "";        // a string to send back
int m;

IPAddress Eth_ipDestination;
IPAddress ipDestination1;
byte my_WiFi_Mode = 0;  // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint


NTRIPClient_Eth ntrip_e;
NTRIPClient_WiFi ntrip_c;
EthernetUDP Eth_udpPAOGI;
EthernetUDP Eth_udpNtrip;
EthernetUDP Eth_udpNtrip_Router;
WiFiClient client;
EthernetClient client_Eth;
AsyncUDP udpRoof;
AsyncUDP udpNtrip;
WiFiServer server(80);

// Heading
double heading, headingUBX, headingzuvor = 0, headingzuvorVTG;
double headingUBXmin, headingUBXmax, headingVTGmin, headingVTGmax;
double speeed = 0, speeed1 = 0, headingnord, speeedzuvor = 0, speeedmin = 0.3, speeedmax = 0.3;


// roll
float rollCorrectionDistance = 0.00;
double rollnord = 0.0, rolleast = 0.0;
double rollnord1 = 0.0, rolleast1 = 0.0;
double rollnord_before = 0.0, rolleast_before = 0.0;
double relPosD, relPosDH;
double rollzuvor = 0;
double PI180 = 57.295791;
double baseline, baseline1, baselineHorizontal;
double fixnorddeci, fixeastdeci; // coordinates in decimalen
double fixnorddeci_old, fixeastdeci_old; // coordinates in decimalen

byte CK_A = 0, CK_B = 0;
byte incoming_char;

// NMEA erstellen
int inByte;
int start = 0, GGASats;
String nmea = "", GGAdaten = "", GGAdaten1 = "", VTGdaten = "", VTGspeed = "", VTGheadingnord = "";
String VTGSatz = "", GGASatz_old = "", GGAnord = "", GGAeast = "", GGAZeit = "", GGAWestEast = "", GGANordSued = "";
int j = 0, j2 = 0, jGGA = 0, jGGA2 = 0, jGGA3 = 0, jGGA4 = 0, jGGA5 = 0, jGGA6 = 0, jGGA7 = 0, jGGA78 = 0;
int jVTG1 = 0, jVTG2 = 0, jVTG3 = 0, jVTG4 = 0, jVTG5 = 0;
String GPSquali = "", WEcoordinaten, NScoordinaten, GGASat, GGAHDop;
String GGASatz = "", GGASatz_Korr, VTGSatz_Korr = "", GGASatz_send_back = "";
int GPSqualin1 = 0, GPSqualin2 = 1, GPSqualinzuvor = 1, GPSqualintime = 1, GGA_check = 0;
String ZDASatz = "", GGA_hDop, GGA_seahigh;
int  i = 0, ij = 0;
double GGAZeitNummerbevor, GGAZeitNummer;
double GGA_hDops, GGAage, GGA_seahighs;

// PAOGI erstellen
bool Paogi_true_UBX = true, Paogi_true = true;
String RollHeadingrest = "", RollHeadingshit = "", RollHeadingrest_befor = "", BS = ",";
int Paogi_Long, Coodinate_check1, Coodinate_check2, heading_check1 = 0;
int Paogi_Long1, Coodinate1_check1, Coodinate1_check2, heading1_check1 = 0;
int Paogi_Shit = 0, Paogi_Shit1 = 0;

byte UDPPAOGIMsg[100], UDPVTGMsg[40], UDPGGAMsg[90];
unsigned int Bytelaenge, BytelaengeVTG, BytelaengeGGA;
byte  ReplyBufferVTG[40] = "", ReplyBufferGGA[90] = "";       // a string to send back

//UBX
double speedUBXint, UBX_lon, UBX_lat;
int UBX_lon_int, UBX_lat_int;
double UBX_lon_double, UBX_lat_double, UBX_time1, UBX_time2, UBX_time3, UBX_time4;
int UBX_time1i, UBX_time2i, UBX_time3i, UBX_time4i, UBX_fixtypei, UBX_Satsi, UBX_Sealeveli;
String speedUBXstr, UBX_time, UBX_time_befor;
double UBX_Sats, UBX_Sealeveld, UBX_DOP, UBX_fixtype, UBX_Sealevel, UBX_timed;


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

//bool debugmode = true;  // GGA,VTG,
bool debugmode = false;
//bool debugmode1 = true;  // Heading
bool debugmode1 = false;
//  bool debugmode2 = true;  // Deviation
bool debugmode2 = false;
//  bool debugmode3 = true;  // roll
bool debugmode3 = false;
//bool debugmode_UBX = true;  //  Protocoll UBX einlesen
bool debugmode_UBX = false;
//  bool debugProtokoll = true;  //Protocoll TestStation
bool debugProtokoll = false;
//bool debugmode_amatron = true;  //Protocoll Amatron
bool debugmode_amatron = false;



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
  Serial.println(VERS);
  Serial.println("");
  Serial.println("Start setup");
  Serial1.begin(115200, SERIAL_8N1, RX1, TX1);
  delay(10);
  if ((Dual_Antenna == 1) || (send_amatron_nmea = 1)) {
    Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  }
  pinMode(Button_ReScan, INPUT_PULLUP);
  pinMode(LED_ntrip_ON, OUTPUT);
  digitalWrite(LED_ntrip_ON, HIGH);

  startSend_back_Time = millis() - (GGA_Send_Back_Time * 1000);
  ntriptime_from_AgopenGPS = millis();
  WiFi_scan_Delay_Time = millis();
  Amatron_begin_Time = millis();
  OTA_begin_Time = millis();

  delay(100);
  Serial.print("send_Data_Via : "); Serial.println(send_Data_Via);
  Serial.print("Ntriphotspot : "); Serial.println(Ntriphotspot);
  if  ((WIFI_Network1 != " ") || (Ntriphotspot > 1) || (send_Data_Via == 2)) {
    Serial.println("");
  }
  else {
    Serial.println("Delay_Time ");
  }
  delay(5000);
  Serial.print("Check_connections : ");
  Check_connections();
  Serial.println(Conn);
  delay(1000);
  Start_connections();
  delay(1000);
  if (my_WiFi_Mode == 0)  WiFi_Start_AP();
  if (my_WiFi_Mode != 0)  OTA_update_ESP32();

  Serial.println("");
  Serial.println("End setup");
  Serial.println("");

  Ntriphotspotoriginal = Ntriphotspot;
}


void loop() {

  //  Start a WiFi scan with pressing the button
  if ((my_WiFi_Mode == 2) && (millis() - OTA_begin_Time > timeoutRouter * 1000)) {
    WiFi.mode(WIFI_OFF);
    Serial.println("End Accesspoint");
    (my_WiFi_Mode = 0);
  }
  buttonState = digitalRead(Button_ReScan);
  if ((buttonState == 0) && (ntrip_from_AgopenGPS == 0)) {
    WiFi.mode(WIFI_OFF);
    digitalWrite(LED_ntrip_ON, LOW);
    Network_built_up();
    if (my_WiFi_Mode == 0)  WiFi_Start_AP();
    OTA_begin_Time = millis();
  }

  //  Start up WiFi connection if lost

  if ((Ntrip_WiFi) && (WiFi.status() != WL_CONNECTED)) {
    Network_built_up();
  }

  //  receive RTCM3 data by USB from AOG #############################################
  //   if ((send_Data_Via == 0) && (Ntriphotspot == 0)
  if (Serial.available()) {      // If RTCM3 comes in Serial (USB),
    char C = Serial.read();      // read a byte, then
    Serial1.write(C);            // send it out Serial1 from PIN 16 to simpleRTK RX1 1. Antenna = RTCM
    if (C != '  ') {             // if the byte is a newline character
      ntripcheck();
    }
    ntriptime_from_AgopenGPS = millis();
  }
  else {
    ntripcheck();
  }

  //  receive RTCM3 data by Ethernet from AOG  ##########################################
  //   if ((send_Data_Via == 1) && (Ntriphotspot == 0)
  if ((Ethernet_running) && (Ntriphotspot == 0)) {
    doEthUDPNtrip();  // If RTCM3 comes in received by Ethernet from AOG
  }

  // If RTCM3 comes in received by Ethernet from Router ####################################
  if ((send_Data_Via == 1) && (Ntriphotspot == 1) && (ntrip_from_AgopenGPS == 0) && (Ethernet_running)) { //  Ntrip_begin_Time
    if (ntrip_e.available()) {         // If RTCM3 comes in received by WIFI
      Serial1.write(ntrip_e.read());   // read RTCM3  and send from ESP32 16 to simpleRTK RX 1. Antenna = RTCM
    }
  }

  // If RTCM3 comes in received by Ethernet from Router ####################################
  if ((send_Data_Via == 1) && (Ntriphotspot == 2) && (Ethernet_running)) { //  Ntrip_begin_Time
    if (ntrip_c.available()) {         // If RTCM3 comes in received by WIFI
      Serial1.write(ntrip_c.read());   // read RTCM3  and send from ESP32 16 to simpleRTK RX 1. Antenna = RTCM
    }
  }

  //  receive RTCM3 data by WiFi from Hotspot    ############################################
  if ((send_Data_Via == 2) && (Ntriphotspot == 2) && (ntrip_from_AgopenGPS == 0) && (WiFi.status() == WL_CONNECTED)) { //  Ntrip_begin_Time
    if (ntrip_c.available()) {         // If RTCM3 comes in received by WIFI
      Serial1.write(ntrip_c.read());   // read RTCM3  and send from ESP32 16 to simpleRTK RX 1. Antenna = RTCM
    }
  }

  //  receive RTCM3 data by WiFi from Router(AOG)   ########################################
  if ((send_Data_Via == 2) && (Ntriphotspot == 3)) {
    doUDPNtrip ();  // If RTCM3 comes in received by WiFi
  }

  //  read NMEA msg from F9P (PVT) and pars them in NMEA_read()   ##############################################
  if (Serial1.available()) { // If anything comes in Serial1
    inByte = Serial1.read(); // read it and send for NMEA_PAOGI
    NMEA_read();
  }

  //  Send GGA MSG back to Base     ###########################################################################
  if (GGA_Send_Back_Time != 0)  sendGGA_WiFi();
  if (GGA_Send_Back_Time != 0)  sendGGA_Eth();

  //  read UBX msg from F9P (heading)    ######################################################################
  if (Dual_Antenna == 1) {
    if (Serial2.available()) {         // If anything comes in Serial2
      incoming_char = Serial2.read();  // ESP32 read RELPOSNED from F9P
      if (i < 4 && incoming_char == ubxmessage.rawBuffer[i]) {
        i++;
      }
      else if (i > 3) {
        ubxmessage.rawBuffer[i] = incoming_char;
        i++;
      }
    }
    if (i > 71) {
      CK_A = 0;
      CK_B = 0;
      for (i = 2; i < 70 ; i++) {
        CK_A = CK_A + ubxmessage.rawBuffer[i];
        CK_B = CK_B + CK_A;
      }

      if (CK_A == ubxmessage.rawBuffer[70] && CK_B == ubxmessage.rawBuffer[71]) {

        rollundheading();   // calculate roll        ########################################################
        PAOGI1_builder();   // built the PAOGI MSG   ########################################################
      }
      else {
        // Serial.println("ACK Checksum Failure: ");
      }
      i = 0;
    }
  }
  // Wifi LED on == no Ntrip from ESP32, off == Ntrip from ESP32   #########################################
  if ((Ntriphotspot_an == 1) || (Ntriphotspot == 0)) {
    digitalWrite(LED_ntrip_ON, LOW);
  }
  else {
    digitalWrite(LED_ntrip_ON, HIGH);
  }
  if (OTA_begin_Time > (millis() - 90000)) {
    OTA_update = true;
    ArduinoOTA.handle();   //  update the ESP32 via WiFi.
  }
  else  OTA_update = false;

}//end main loop

//------------------------------------------------------------------------------------------
