//***************************************************************************************************************
// Ntrip
void connectToNtrip() {
  int ntripVersuch = 0;
  while (ntripVersuch < 3) {
    Serial.println("Requesting SourceTable");
    delay(10);
    if (ntrip_c.reqSrcTbl(Ntrip_host, Ntrip_httpPort)) {
      char buffer[1024];
      delay(1000);
      while (ntrip_c.available()) {
        ntrip_c.readLine(buffer, sizeof(buffer));
        Serial.print(" Buffer: ");
        Serial.println(buffer);
      }
    }
    else {
      Serial.println("SourceTable request error");
    }
    Serial.print("Requesting SourceTable is OK\n");

    ntrip_c.stop(); //Need to call "stop" function for next request.
    delay(1000);
    Serial.println("Requesting MountPoint's Raw data");
    //
    if (!ntrip_c.reqRaw(Ntrip_host, Ntrip_httpPort, Ntrip_mntpnt, Ntrip_user, Ntrip_passwd)) {
      delay(1000);
      Serial.println("no ntrip connection");
      Serial.println(" ");
      ntripVersuch++;
      Ntriphotspot = 0;
    }
    else {
      if (GGA_Send_Back_Time != 0)  sendGGA();
      delay(1000);
      Ntrip_begin_Time = millis();
      Serial.println("Requesting MountPoint is OK, all good");
      Serial.println(" ");
      Ntriphotspot = 1;
      ntripVersuch = 4;
    }
  }
}  // end connectToNtrip

//sendGGA---------------------------------------------------------------------------------------------

void sendGGA() {
  // char GGASatz_send_back[100] = "$GPGGA,051601.650,4812.439,N,01633.778,E,1,12,1.0,0.0,M,0.0,M,,*60";//hc create via www.nmeagen.org
  GGASatz_send_back = "$GPGGA,";
  (GGASatz_send_back.concat(UBX_time));
  (GGASatz_send_back.concat("0,"));
  (GGASatz_send_back.concat(NScoordinaten));
  (GGASatz_send_back.concat(BS));
  (GGASatz_send_back.concat(GGANordSued));
  (GGASatz_send_back.concat(BS));
  (GGASatz_send_back.concat(WEcoordinaten));
  (GGASatz_send_back.concat(BS));
  (GGASatz_send_back.concat(GGAWestEast));
  (GGASatz_send_back.concat(",1,12,1.0,0.0,M,0.0,M,,*"));
  /*      GGASatz_send_back = ("$GPGGA," + GGAZeit + "0," + GGAnord.substring(0, 8) + "," + GGANordSued + "," + GGAeast.substring(0, 9) +
        "," + GGAWestEast + ",1,12,1.0,0.0,M,0.0,M,,*");
  */
  for (XOR = 0, j = 0; j < GGASatz_send_back.length(); j++) { // Berechnung Checksumme
    c = (unsigned char)GGASatz_send_back.charAt(j);
    if (c == '*') break;
    if ((c != '$') && (c != '!')) XOR ^= c;
  }
  checksum = String(XOR, HEX);
  checksum.toUpperCase();
  GGASatz_send_back.concat(checksum);

  if ((Ntriphotspot == 1) && ((millis() - startSend_back_Time) > (GGA_Send_Back_Time * 1000))) {  // send back GGA to Ntripserver
    ntrip_c.print(GGASatz_send_back);
    ntrip_c.print("\r\n");
    //    Serial.println(GGASatz_send_back);
    startSend_back_Time = millis();
  }
} // end void sendGGA

//doUDPNtrip---------------------------------------------------------------------------------------------

void doEthUDPNtrip() {
  unsigned int packetLenght = Eth_udpNtrip.parsePacket();
  if (packetLenght > 0) {
    Eth_udpNtrip.read(Eth_NTRIP_packetBuffer, packetLenght);
    for (unsigned int ik = 0; ik < packetLenght; ik++)
    {
      Serial1.write(Eth_NTRIP_packetBuffer[ik]);
    }
    //    Serial1.println(); //really send data from UART buffer
  }  // end of Packet
} // end doEthUDPNtrip


//ntripcheck---------------------------------------------------------------------------------------------

void ntripcheck() {
  if (ntriptime_from_AgopenGPS > (millis() - 4000)) {
    ntrip_from_AgopenGPS = 1;
    NtripCheckTrue = true;
  }
  else {
    ntrip_from_AgopenGPS = 0;
    if (NtripCheckTrue) {
      if (Ntriphotspotoriginal == 1)
      {
        //      ESP.restart();
      }
    }
    NtripCheckTrue = false;
  }
} // end void ntripcheck
