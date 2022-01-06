//Nmeacheck---------------------------------------------------------------------------------------------

void Paogicheck() {

  if ((Paogi_Long < 91) || (Paogi_Long > 98) || (jGGA6 != 45)) {
    Paogi_true = false;
  }
  if ((abs(GGAZeitNummer - GGAZeitNummerbevor) > 45) && (GGA_check < 3))
  { Paogi_true = false;
    GGA_check++;
  }
  else {
    GGAZeitNummerbevor = GGAZeitNummer;
    GGA_check = 0;
  }
  if ((GPSqualin1 < 0) || (GPSqualin1 > 5))  Paogi_true = false;
  if ((GGASats < 10) || (GGASats > 12))  Paogi_true = false;
  if ((GGA_hDops < 0) || (GGA_hDops > 2))  Paogi_true = false;
  if ((GGA_seahighs < -100) || (GGA_seahighs > 2000))  Paogi_true = false;
  if ((speeed1 < 0) || (speeed1 > 55))  Paogi_true = false;
  if ((heading < 0) || (heading > 360))  Paogi_true = false;
  if ((roll < -30) || (roll > 30))  Paogi_true = false;
  if ((ntrip_from_AgopenGPS != 0) && (ntrip_from_AgopenGPS != 1))  Paogi_true = false;
  if ((Ntriphotspot_an != 0) && (Ntriphotspot_an != 1))  Paogi_true = false;
}   // end void Paogicheck

//sendGGA---------------------------------------------------------------------------------------------

void sendGGA() {
  // char GGASatz_send_back[100] = "$GPGGA,051601.650,4812.439,N,01633.778,E,1,12,1.0,0.0,M,0.0,M,,*60";//hc create via www.nmeagen.org

  if ((Ntriphotspot == 1) && ((millis() - startSend_back_Time) > (GGA_Send_Back_Time * 1000)) && (GPSqualin1 > 0)) {  // send back GGA to Ntripserver
    ntrip_c.print(GGASatz_send_back1);
    ntrip_c.print("\r\n");
    //    Serial.println(GGASatz_send_back2);
    //    Serial.println(GGASatz_send_back1);
    startSend_back_Time = millis();
  }
} // end void sendGGA

//doUDPNtrip---------------------------------------------------------------------------------------------

void doEthUDPNtrip() {
  unsigned int packetLenght = Eth_udpNtrip.parsePacket();
  //  Serial.println(packetLenght);
  if (packetLenght > 0) {
    //   Serial.println(packetLenght);
    Eth_udpNtrip.read(Eth_NTRIP_packetBuffer, packetLenght);
    //Eth_udpNtrip.flush();
    //     Serial.println("Hallo 2");
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
