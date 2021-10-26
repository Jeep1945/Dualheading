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

int AntDistance = 95;       // distance between the two antennas in cm,+, 0 for automatic distance
double headingcorr = 90;     // right antenna A , left antenna B;
int tractorhight = 280;   // roll is in Position calculated, in AgOpenGPS mit 10cm
int WiFi_scan_Delay = 0;      // for router use 50 sec delay
int send_Data_Via = 1;       // send Data via  0 = USB, 1 = WiFi


char ssid[24] = "";           // WiFi network Client name
char password[24] = "";    // WiFi network password//Accesspoint name and password

//Accesspoint name and password
char ssid_ap[24] = "Autonomes Lenken";  // name of Access point, if no WiFi found, NO password!!
int timeoutRouter = 2;                 //time (s) to search for existing WiFi, than starting Accesspoint
#define WIFI_TIMEOUT_MS 100000
int Ntrip_httpPort = 2101;              //port 2101 is default port of NTRIP caster

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
//libraries -------------------------------
#include <Wire.h>
#include <WiFi.h>
#include <math.h>
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
byte UDPPAOGIMsg[100];


// Ntrip
unsigned long WiFi_scan_Delay_Time = millis();
bool OTA_update = false;
int wait = 30000;


int buttonState = 0;
#define Button_ReScan 4  // pin 4  if button pressed, WiFi scan is starting
#define LED_ntrip_ON  2  // pin 2  if ntrip on without AGopenGPS

//static IP
byte myip[4] = { 192, 168, 1, 79 };     // Roofcontrol module
byte gwip[4] = { 192, 168, 1, 1 };      // Gateway IP also used if Accesspoint created
byte mask[4] = { 255, 255, 255, 0 };
byte myDNS[4] = { 8, 8, 8, 8 };         //optional
byte ipDestination[4] = { 192, 168, 1, 255};//IP address of router to send UDP data to
//byte ipDestination[4] = { 10, 0, 0, 11};//IP address of router to send UDP data to
byte myIPEnding = 79;             //ending of IP adress x.x.x.79 of ESP32

unsigned int portMy = 5544;             //this is port of this module: Autosteer = 5577 IMU = 5566 GPS =
unsigned int AOGNtripPort = 2233;       //port NTRIP data from AOG comes in
unsigned int portAOG = 8888;            //port to listen for AOG
unsigned int portDestination = 9999;    //Port of AOG that listens
byte ReplyBufferPAOGI[100];
unsigned int Bytelaenge;
int m;

IPAddress ipDestination1;

AsyncUDP udpRoof;
AsyncUDP udpNtrip;
WiFiServer server(80);

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

// PAOGI erstellen
bool Paogi_true = true;
int inByte, start = 0, GGASats;
String nmea = "", GGAdaten = "", GGAdaten1 = "", VTGdaten = "", VTGspeed = "", VTGheadingnord = "", RollHeadingrest = "", RollHeadingrest_befor = "";
String VTGSatz = "", GGASatz_old = "", GGAnord = "", GGAeast = "", GGAZeit = "", GGAWestEast = "", GGANordSued = "";
int j = 0, j2 = 0, jGGA = 0, jGGA2 = 0, jGGA3 = 0, jGGA4 = 0, jGGA5 = 0, jGGA6 = 0, jGGA7 = 0, jGGA78 = 0, j_checksum_GGA = 0;
int jVTG1 = 0, jVTG2 = 0, jVTG3 = 0, jVTG4 = 0, jVTG5 = 0, j_checksum_VTG = 0;
String GPSquali = "", WEcoordinaten, NScoordinaten, GGASat, GGAHDop;
String GGASatz = "", GGASatz_Korr, VTGSatz_Korr = "";
int GPSqualin1 = 0, GPSqualin2 = 1, GPSqualinzuvor = 1, GPSqualintime = 1;
int i = 0, ij = 0, Paogi_Long, Coodinate_check1, Coodinate_check2, GGA_check = 0;
double GGAZeitNummerbevor, GGAZeitNummer;

// Chechsum controll
String checksum = "", checksum_GGA = "", checksum_GGA_send = "", checksum_VTG = "", checksum_VTG_send = "";
String check_headingroll = "";


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
  byte rawBufferPVT[100];
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
  Serial.println("Start setup");
  Serial1.begin(115200, SERIAL_8N1, RX1, TX1);
  delay(10);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);

  pinMode(Button_ReScan, INPUT_PULLUP);
  pinMode(LED_ntrip_ON, OUTPUT);
  digitalWrite(LED_ntrip_ON, HIGH);

  WiFi_scan_Delay_Time = millis();

  delay(100);

  if  (send_Data_Via == 0) {
    wait = 500;
  }
/*  if (udpNtrip.listen(AOGNtripPort))
  {
    Serial.print("NTRIP UDP Listening to port: ");
    Serial.println(AOGNtripPort);
    Serial.println();
  }
  delay(50);
  if (udpRoof.listen(portMy))
  {
    Serial.print("UDP writing to IP: ");
    Serial.println(ipDestination1);
    Serial.print("UDP writing to port: ");
    Serial.println(portDestination);
    Serial.print("UDP writing from port: ");
    Serial.println(portMy);
  }
*/  delay(200);
//  server.begin();
  delay(50);

  Serial.println("End setup");

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
  ArduinoOTA.handle();

}
