//Nmeacheck---------------------------------------------------------------------------------------------

void Paogicheck() {
  //  RollHeadingrest = "$PAOGI," "  + String(roll) + ",,," + String(ntrip_from_AgopenGPS) + "," + String(Ntriphotspot_an) + ",*";

  if ((Paogi_Long < 87) || (Paogi_Long > 92) || (jGGA6 != 45))  Paogi_true = false;
  if ((abs(GGAZeitNummer - GGAZeitNummerbevor) > 2) && (GGA_check < 3))
  { Paogi_true = false;
    GGA_check++;
  }
  else {
    GGAZeitNummerbevor = GGAZeitNummer;
    GGA_check = 0;
  }
  if ((GPSqualin1 < 0) || (GPSqualin1 > 5))  Paogi_true = false;
  if ((GGASats < 10) || (GGASats > 12))  Paogi_true = false;
  if ((speeed1 < 0) || (speeed1 > 55))  Paogi_true = false;
  if ((heading < 0) || (heading > 360))  Paogi_true = false;
  if ((roll < -30) || (roll > 30))  Paogi_true = false;
}   // end void Paogicheck


//---------------------------------------------------------------------
// start WiFi Access Point = only if no existing WiFi

void WiFi_Start_AP() {
  WiFi.mode(WIFI_AP);   // Accesspoint
  WiFi.softAP(ssid_ap, "");
  while (!SYSTEM_EVENT_AP_START) // wait until AP has started
  {
    delay(100);
    Serial.print(".");
  }
  delay(100);//right IP adress only with this delay
  WiFi.softAPConfig(gwip, gwip, mask);  // set fix IP for AP
  delay(300);
  IPAddress myIP = WiFi.softAPIP();
  delay(300);

  //AP_time = millis();
  Serial.print("Accesspoint started - Name : ");
  Serial.println(ssid_ap);
  Serial.print( " IP address: ");
  ipDestination1 = myIP;
  Serial.println(ipDestination1);
  ipDestination[3] = 255;
  digitalWrite(LED_ntrip_ON, LOW);

  if (udpNtrip.listen(AOGNtripPort))
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
  delay(200);
  server.begin();
  delay(50);

}

//-------------------------------------------------------------------------------------------------

void doUDPNtrip() {
  udpNtrip.onPacket([](AsyncUDPPacket packet)
  {
    if (debugmode) {
      Serial.println("got NTRIP data");
    }
    for (unsigned int i = 0; i < packet.length(); i++)
    {
      Serial1.write(packet.data()[i]);
    }
  });  // end of onPacket call
}

//connectToWiFi---------------------------------------------------------------------------------------------

void connectToWiFi() {
  Serial.print("try to connect to WiFi: ");
  WiFi.mode(WIFI_STA);
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    delay(300);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    Serial.println("WiFi offline ");
  }
  if (WiFi.status() == WL_CONNECTED) {
    delay(200);
    Serial.println();
    Serial.print("WiFi Client successfully connected to : ");
    Serial.print("IP address: ");
    IPAddress myIP = WiFi.localIP();
    Serial.println(myIP);
    //after connecting get IP from router -> change it to x.x.x.IP Ending (from settings)
    myIP[3] = myIPEnding; //set ESP32 IP to x.x.x.myIP_ending
    Serial.print("changing IP to: ");
    Serial.println(myIP);
    IPAddress gwIP = WiFi.gatewayIP();
    delay(200);
    Serial.print("Connected IP - Address : ");
    myIP = WiFi.localIP();
    ipDestination1 = myIP;
    ipDestination1[3] = 255;
    Serial.println(myIP);
    Serial.print("Gateway IP - Address : ");
    Serial.println(gwIP);
    ipDestination[0] = myIP[0];
    ipDestination[1] = myIP[1];
    ipDestination[2] = myIP[2];
    ipDestination[3] = 255;//set IP to x.x.x.255 according to actual network
    digitalWrite(LED_ntrip_ON, LOW);
    if (udpNtrip.listen(AOGNtripPort))
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
    delay(200);

    if (OTA_update)
    {
      ArduinoOTA.setHostname("Dualesp32");
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

    }
  }
}  // end connectToWiFi

//***************************************************************************************************************
