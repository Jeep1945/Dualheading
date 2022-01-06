//Core2: this task handles the GPS NMEAs, calculats roll and heading

void Core2code( void * pvParameters ) {

  delay(3000);

  for (;;) { //main loop core2

    if (Serial.available()) {      // If RTCM3 comes in Serial (USB),
      char C = Serial.read();      // read a byte, then
      Serial1.write(C);            // send it out Serial1 from 16 to simpleRTK RX 1. Antenna = RTCM
    }

        if (send_Data_Via == 1) {
          doUDPNtrip ();  // If RTCM3 comes in received by WiFi
        }
    
    if (Serial1.available()) { // If anything comes in Serial1
      inByte = Serial1.read(); // read it and send for NMEA_PAOGI
      NMEA_read();
    }

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
        rollundheading();
        PAOGI_builder();
      }
      else {
        // Serial.println("ACK Checksum Failure: ");
      }
      i = 0;
    }
    // Wifi LED
    if (send_Data_Via == 1) {
      digitalWrite(LED_ntrip_ON, LOW);
    }
    else {
      digitalWrite(LED_ntrip_ON, HIGH);
    }
  }//end main loop core 2
}
//------------------------------------------------------------------------------------------
