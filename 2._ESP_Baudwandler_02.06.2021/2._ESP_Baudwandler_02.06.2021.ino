#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "xx";
const char* password = "xx";


//UART1 Dateneingang vom ESP32 "DualGPS"
#define RXD1 27
#define TXD1 16

//UART2 Daten Ausgang zum TTL/Serial Wandler
#define RXD2 25
#define TXD2 17

#define herz1 190
#define herz2 980

unsigned long OTA_End_Time = millis();
bool OTA_update = true;

int send_Time1Hz, send_Time5Hz,  looptime;
int inByte, start = 0, i , j, m;
String nmea = "";
char GGASentence[100] = "";
char VTGSentence[100] = "";
char GSASentence[100] = "";
char ZDASentence[100] = "";

//bool debugmode_amatron = true;  //Protocoll Amatron
bool debugmode_amatron = false;



void setup() {

  Serial.begin(38400);                                   //UART0 Serial über USB;
  delay(50);
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);         //UART1 Eingangssignal auslesen
  delay(10);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);          //UART2 Ausgangssignal weiterleiten (Amatron)
  //delay(10);
  Serial.println("Serial Txd liegt an Pin: " + String(TX));
  delay(10);
  Serial.println("Serial Rxd liegt an Pin: " + String(RX));

  send_Time5Hz = millis();
  send_Time1Hz = millis();

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  //   ArduinoOTA.setHostname("Dualesp32");
  ArduinoOTA.setHostname("Amatronesp32");


  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("   Ready 2");
}  // end of setup

void loop() {
  if ((OTA_update) && (OTA_End_Time < millis() - 60000)) {
    ArduinoOTA.handle();
  }
  else {
    OTA_update = false;
    WiFi.disconnect();
  }
  if (Serial1.available()) { // If anything comes in Serial1
    inByte = Serial1.read(); // read it and send for PARSER
    MSG_build();
  }

  if ((millis() - send_Time5Hz) > herz1) {
    Serial2.println(GGASentence);
    Serial2.println(VTGSentence);
    send_Time5Hz = millis();
    if (debugmode_amatron) {
      Serial.println(GGASentence);
      Serial.println(VTGSentence);
    }
  }

  if ((millis() - send_Time1Hz) > herz2) {
    Serial2.println(GSASentence);
    Serial2.println(ZDASentence);
    send_Time1Hz = millis();
    if (debugmode_amatron) {
      Serial.println(ZDASentence);
    }
  }

} // end of loop
