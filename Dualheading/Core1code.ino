//Core1:  this task handles the Wifi and NTRIP Client

void Core1code( void * pvParameters ) {

  vTaskDelay(3000);

  if (send_Data_Via == 1) {
    Serial.println("Start Ethernet");
    Eth_Start();
  }
  //  else // delay(500);

  //  vTaskDelay(100);

  for (;;) { // MAIN LOOP FOR THIS CORE
    

    if ((ntrip_from_AgopenGPS == 1) || (Ntriphotspot == 0)) {
      WiFi.disconnect();
      digitalWrite(LED_ntrip_ON, LOW);
    }
    buttonState = digitalRead(Button_ReScan);
    if ((buttonState == 0) && (ntrip_from_AgopenGPS == 0)) {
      //      ESP.restart();
      WiFi.disconnect();
     digitalWrite(LED_ntrip_ON, LOW);
      scan_WiFi_connections();
    }
    if ((Ntriphotspot == 1 ) && (millis() - WiFi_scan_Delay_Time > (WiFi_scan_Delay * 1000))) {
      vTaskDelay(100);
      //      Serial.println("Hallo 1");
      //      Ntriphotspot_an = 1;
      if (WiFi.status() != WL_CONNECTED) {
        Ntriphotspot_an = 0;
        digitalWrite(LED_ntrip_ON, HIGH);
        vTaskDelay(1000);
        scan_WiFi_connections();
        vTaskDelay(1000);
        //        Serial.println("Hallo 2");
        if (WiFi_netw_nr > 0) {
          //          Serial.println("Hallo 3");
          connectToWiFi();
          vTaskDelay(1000);
          if (WiFi.status() == WL_CONNECTED) {
            connectToNtrip();
//            vTaskDelay(10000);
          }
        }
//        else vTaskDelay(100);
      }
      else  Ntriphotspot_an = 1;
    }
    else {
      //      Ntriphotspot_an = 0;
      vTaskDelay(100);
    }
    vTaskDelay(100);
  } // End of (main core1)
} // End of core1code



//###########################################################################################
