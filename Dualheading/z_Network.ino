//scan_WiFi_connections---------------------------------------------------------------------------------------------

void scan_WiFi_connections()
{
  Serial.println("");
  Serial.println("scan start " + String(WiFi_scan_Versuch));
  net_found = 0;
  // WiFi.scanNetworks will return the number of networks found
  net_found = WiFi.scanNetworks();
  Serial.println("scan done");
  if (net_found == 0) {
    Serial.println("no networks found");
    WiFi_netw_nr = 0;
  }
  else
  {
    Serial.print(net_found);
    Serial.println(" networks found");
    for (int i = 0; i < net_found; ++i) {
      Serial.println(String(i + 1) + ". Netzwerk : " + WiFi.SSID(i));
    }
    delay(2000);
    Serial.println("");
    for (int i = 0; i < net_found; ++i) {
      if (WiFi.SSID(i) == WIFI_Network1) {
        Serial.println("Connect with : " + WiFi.SSID(i));
        network_found = true;  // network found in list
        WiFi_netw_nr = 1;
        break;
      }
      if (WiFi.SSID(i) == WIFI_Network2) {
        Serial.println("Connect with : " + WiFi.SSID(i));
        network_found = true;  // network found in list
        WiFi_netw_nr = 2;
        break;
      }
      if (WiFi.SSID(i) == WIFI_Network3) {
        Serial.println("Connect with : " + WiFi.SSID(i));
        network_found = true;  // network found in list
        WiFi_netw_nr = 3;
        break;
      }
      if (WiFi.SSID(i) == WIFI_Network4) {
        Serial.println("Connect with : " + WiFi.SSID(i));
        network_found = true;  // network found in list
        WiFi_netw_nr = 4;
        break;
      }
      if (WiFi.SSID(i) == WIFI_Network5) {
        Serial.println("Connect with : " + WiFi.SSID(i));
        network_found = true;  // network found in list
        WiFi_netw_nr = 5;
        break;
      }
      if (WiFi.SSID(i) == WIFI_Network6) {
        Serial.println("Connect with : " + WiFi.SSID(i));
        network_found = true;  // network found in list
        WiFi_netw_nr = 6;
        break;
      }
    }
  }
  if (WiFi_scan_Versuch > 2) Ntriphotspot = 0;
  if (WiFi_netw_nr > 0) WiFi_scan_Versuch = 1;
  else WiFi_scan_Versuch++;

}  //end scan_WiFi_connections()

//WiFi---------------------------------------------------------------------------------------------

void connectToWiFi() {
  if (network_found) {
    WiFi.mode(WIFI_STA);
    switch (WiFi_netw_nr) {
      case 0:  break;
      case 1: WiFi.begin(WIFI_Network1, WIFI_Password1); break;
      case 2: WiFi.begin(WIFI_Network2, WIFI_Password2); break;
      case 3: WiFi.begin(WIFI_Network3, WIFI_Password3); break;
      case 4: WiFi.begin(WIFI_Network4, WIFI_Password4); break;
      case 5: WiFi.begin(WIFI_Network5, WIFI_Password5); break;
      case 6: WiFi.begin(WIFI_Network6, WIFI_Password6); break;
    }
    //    Serial.print("try to connect to WiFi: ");

    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
      delay(300);
      //      Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected");
      Serial.println("WiFi offline ");
      Ntriphotspot = 0;
    }
    if (WiFi.status() == WL_CONNECTED) {
      delay(200);
      Serial.println();
      Serial.print("WiFi Client successfully connected to : ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println();
    }
    /*    if (OTA_update)
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
    */
  }
}  // end connectToWiFi
