//Core1:  this task handles the Wifi and NTRIP Client

void Core1code( void * pvParameters ) {

  delay(3000);

  if (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(100);
    connectToWiFi();
    //    WiFi_Start_AP();
  }
  //  else // delay(500);

  //  vTaskDelay(100);

  for (;;) { // MAIN LOOP FOR THIS CORE

    if (send_Data_Via == 0) {
      WiFi.disconnect();
      digitalWrite(LED_ntrip_ON, LOW);
    }
    buttonState = digitalRead(Button_ReScan);
    if (buttonState == 0) {
      ESP.restart();
    }
    if ((send_Data_Via == 1 ) && (millis() - WiFi_scan_Delay_Time > (WiFi_scan_Delay * 1000))) {
      vTaskDelay(100);
      if (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(100);
           connectToWiFi();
        //    WiFi_Start_AP();
      }
    }
    else {
      //      Ntriphotspot_an = 0;
      vTaskDelay(100);
    }
    vTaskDelay(100);
  } // End of (main core1)
} // End of core1code



//###########################################################################################
