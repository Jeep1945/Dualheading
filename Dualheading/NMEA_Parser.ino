void NMEA_read() {

  if ((start == 0) && ((inByte == '$') || (inByte == '!'))) {
    start = 1; nmea = ""; // begin $ or !
  }
  jGGA = 0;
  jGGA7 = 0;
  if (start == 1) {
    nmea.concat((char)inByte); // add sign
  }
  if ((inByte == 13) && (start == 1)) { // CR > Datensatzende > NMEA ausgeben
    start = 0;

    if (nmea.substring(3, 6) == "GGA") {   // Data from GGA with high and ,
      for (j = 0; j < nmea.length(); j++) {
        c = (unsigned char)nmea.charAt(j);
        //        Serial.println(nmea);
        if ((c == ',') && (j > 8)  && (j < 27)) {
          jGGA3 = j + 1;
        }
        if (((c == 'N') || (c == 'S')) && (j > 5)) {
          jGGA4 = j - 1;
          jGGA5 = j + 2;
        }
        if ((c == 'E') || (c == 'W')) {
          jGGA6 = j - 1;
          jGGA2 = j + 2;
        }
        if ((c == ',') && (j > 59)  && (jGGA7 == 0)) {
          jGGA7 = j;
        }
        if ((c == 'M') && (jGGA == 0)) {
          jGGA = j;
        }
        if (c == '*') {
          j_checksum_GGA = j + 1;
          break;
        }
      }
      GGASatz = nmea;
      if ((millis() - Single_begin_Time) > 100) {
        Single_begin_Time = millis();
        BytelaengeGGA = GGASatz.length() + 1;
        if (BytelaengeGGA < 100) {
          if ((send_Data_Via == 0) && (!Dual_Antenna)) {
            Serial.println(GGASatz);
          }
          if ((send_Data_Via > 0) && (!Dual_Antenna)) {
            GGASatz.getBytes(ReplyBufferGGA, BytelaengeGGA);
            ReplyBufferGGA[BytelaengeGGA - 1] = 0x0D;
            ReplyBufferGGA[BytelaengeGGA] = 0x0A;
            if (send_Data_Via == 1) {
              Eth_udpPAOGI.beginPacket(Eth_ipDestination, portDestination);
              Eth_udpPAOGI.write(ReplyBufferGGA, BytelaengeGGA + 1);
              Eth_udpPAOGI.endPacket();
            }
            if (send_Data_Via == 2) {
              udpRoof.writeTo(ReplyBufferGGA, BytelaengeGGA + 1, ipDestination1, portDestination);
            }
          }
        }
      }
      //      if ()
      GGAWestEast = (nmea.substring(jGGA6 + 1, jGGA6 + 2));  // looks for W or E in nmea
      GGANordSued = (nmea.substring(jGGA4 + 1, jGGA4 + 2));  // looks for N or S in nmea
      GPSquali = (nmea.substring(jGGA2, jGGA2 + 1));  //GPS Signal Indikator
      GPSqualin1 = GPSquali.toDouble();
      GGA_hDop = (nmea.substring(jGGA2 + 5, jGGA2 + 9)); //How many hDop;
      GGA_hDops = GGA_hDop.toDouble();
      //       Serial.println(nmea);
      GGASatz_Korr = (nmea.substring(0, j_checksum_GGA));

      if (debugmode)
      {
        Serial.println(GGASatz);
        Serial.print(" GGAWestEast:" + GGAWestEast);
        Serial.print(" GGAnord:" + GGAnord);
        Serial.print(" GGAeast:" + GGAeast);
      }
    }


    /*    $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
      VTG          Track made good and ground speed
      054.7,T      True track made good (degrees)
      034.4,M      Magnetic track made good
      005.5,N      Ground speed, knots
      010.2,K      Ground speed, Kilometers per hour
      48          Checksum
    */

    if (nmea.substring(3, 6) == "VTG")  {   // Data from VTG, speed in knoten and VTG Heading

      for (j = 4; j < nmea.length(); j++) {
        c = (unsigned char)nmea.charAt(j);
        if (c == 'G') jVTG3 = j + 2;  // True track made good (degrees)
        if (c == 'T') jVTG4 = j - 1;  // Heading by geographical North
        if (c == 'M') jVTG1 = j + 2;  // Ground speed, knots
        if (c == 'N') jVTG2 = j - 1;  // Ground speed, Kilometers per hour
        if (c == '*') {
          j_checksum_VTG = j + 1;
          break;
        }
      }
      checksum_VTG_send = (nmea.substring(j_checksum_VTG, j_checksum_VTG + 2));  // actuell checksum
      for (XOR = 0, j = 0; j < nmea.length(); j++) { // Berechnung Checksumme
        c = (unsigned char)nmea.charAt(j);
        if (c == '*') break;
        if ((c != '$') && (c != '!')) XOR ^= c;
      }
      checksum_VTG = String(XOR, HEX);
      checksum_VTG.toUpperCase();
      //      Serial.print(VTGSatz);
      //      Serial.print(" checksum_VTG_send:" + checksum_VTG_send);
      //      Serial.println(" checksum_VTG:" + checksum_VTG);
      if (checksum_VTG_send != checksum_VTG) {
        //        Serial.println(" Checksumme ist nicht in Ordnung");
        nmea = VTGSatz;
      }
      else {
        VTGSatz = nmea;
      }
      if ((millis() - Single_begin_Time_VTG) > 100) {
        Single_begin_Time_VTG = millis();
        BytelaengeVTG = VTGSatz.length() + 1;
        if (BytelaengeVTG < 40) {
          if ((send_Data_Via == 0) && (!Dual_Antenna)) {
            Serial.println(VTGSatz);
          }
          if ((send_Data_Via > 0) && (!Dual_Antenna)) {
            VTGSatz.getBytes(ReplyBufferVTG, BytelaengeVTG);
            ReplyBufferVTG[BytelaengeVTG - 1] = 0x0D;
            ReplyBufferVTG[BytelaengeVTG] = 0x0A;
            if (send_Data_Via == 1) {
              Eth_udpPAOGI.beginPacket(Eth_ipDestination, portDestination);
              Eth_udpPAOGI.write(ReplyBufferVTG, BytelaengeVTG + 1);
              Eth_udpPAOGI.endPacket();
            }
            if (send_Data_Via == 2) {
              udpRoof.writeTo(ReplyBufferVTG, BytelaengeVTG + 1, ipDestination1, portDestination);
            }
          }
        }
      }

      VTGspeed = (nmea.substring(jVTG1, jVTG2));        // Ground speed, knots
      speeed = VTGspeed.toDouble();
      VTGheadingnord = (nmea.substring(jVTG3, jVTG4));  // Heading by geographical North
      if (VTGheadingnord == "") headingnord = 0;
      else headingnord = VTGheadingnord.toDouble();
      if ((headingzuvorVTG > 3) && (headingzuvorVTG < (360 - 3)))  {
        headingVTGmin = headingzuvorVTG - 3;
        headingVTGmax = headingzuvorVTG + 3;
        headingnord = constrain(headingnord, headingVTGmin, headingVTGmax);
        headingzuvorVTG = headingnord;
      }
      //      Serial.println(nmea);
      if (debugmode1)  Serial.println(" Hallo headingnord  :" + String(headingnord));
      VTGSatz_Korr = (nmea.substring(0, j_checksum_VTG)); //Checksum cut off
      VTGSatz = nmea;
      if (send_amatron_nmea == 1) {
        VTGSatz_Korr.replace(VTGheadingnord, String(heading));
        // VTGSatz_Korr.replace(VTGspeed, String(speedUBXint));
        for (XOR = 0, j = 0; j < VTGSatz_Korr.length(); j++) { // Berechnung Checksumme
          c = (unsigned char)VTGSatz_Korr.charAt(j);
          if (c == '*') break;
          if ((c != '$') && (c != '!')) XOR ^= c;
        }
        checksum = String(XOR, HEX);
        checksum.toUpperCase();
        VTGSatz_Korr += checksum;
      }
    }  // end VTG

    if (send_amatron_nmea == 1) {
      if (nmea.substring(3, 6) == "ZDA")  {   // Data from ZDA,
        ZDASatz = nmea;
      }
    }
  }
  //  read UBX PVT from 1. F9P
  if (ij < 4 && inByte == ubxmessagePVT.rawBufferPVT[ij]) {  // read UBX PVT from 1. F9P
    ij++;
  }
  else if (ij > 3) {
    ubxmessagePVT.rawBufferPVT[ij] = inByte;
    ij++;
  }  // end UBX PVT

  if (ij > 99) {
    UBXSpeed();
    UBXCoordinats();
    UBX_Data();
    ij = 0;
  }
} //Ende NMEA_PAOGI
// ************************************************************

void sendAMATRON() {
  if (Amatron_begin_Time < millis() - 60000)
  {
    Serial2.println(GGASatz_Korr);
    Serial2.println(VTGSatz_Korr);
    Serial2.println(ZDASatz);

    if (debugmode_amatron) {
      Serial.println(GGASatz_Korr);
      Serial.println(VTGSatz_Korr);
      Serial.println(ZDASatz);
    }
  }
} //Ende sendAMATRON()

// ************************************************************

void Checksum_GGA_Korr() {
  for (XOR = 0, j = 0; j < GGASatz_Korr.length(); j++) { // Berechnung Checksumme
    c = (unsigned char)GGASatz_Korr.charAt(j);
    if (c == '*') break;
    if ((c != '$') && (c != '!')) XOR ^= c;
  }
  checksum = String(XOR, HEX);
  checksum.toUpperCase();
  GGASatz_Korr += checksum;

}// Ende Checksum_GGA()
