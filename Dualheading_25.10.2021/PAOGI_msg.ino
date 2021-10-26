void PAOGI_builder() {

  speeed1 = speeed;

  Paogi_true = true;

  // build the PAOGI msg
  rolleast = (rolleast * 0.0000001);
  if ((abs(rolleast - rolleast_before) > 2) && (Coodinate_check1 < 4)) {
    Paogi_true = false;
    Coodinate_check1++;
  }
  else {
    rolleast_before = rolleast;
    Coodinate_check1 = 0;
  }
  if ((rolleast < 1000) && (rolleast > 0)) WEcoordinaten = ("00" + String(rolleast, 7));
  if ((rolleast >= 1000) && (rolleast < 10000)) WEcoordinaten = ("0" + String(rolleast, 7));
  if (rolleast >= 10000) WEcoordinaten = String(rolleast, 7);

  rollnord = (rollnord  * 0.0000001);
  if ((abs(rollnord - rollnord_before) > 2) && (Coodinate_check2 < 4)) {
    Paogi_true = false;
    Coodinate_check2++;
  }
  else {
    rollnord_before = rollnord;
    Coodinate_check2 = 0;
  }
  if (rollnord >= 1000)  NScoordinaten = String(rollnord, 7);
  if (rollnord < 1000)  NScoordinaten = ("0" + String(rollnord, 7));

  if ((heading > 3) && (heading < 357) && (GPSqualin1 > 3)) {
    if ((abs(heading - headingzuvor) > 3) && (heading_check1 < 4)) {
      if (speeed > 0.5) {
        heading = headingnord;
      }
      else   heading = headingzuvor;
      heading_check1++;
    }
    else {
      headingzuvor = heading;
      heading_check1 = 0;
    }
  }
  else headingzuvor = heading;


  RollHeadingrest = "$PAOGI,";
  (RollHeadingrest.concat(GGAZeit));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(NScoordinaten));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GGANordSued));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(WEcoordinaten));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GGAWestEast));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GPSqualin1));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GGASats));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GGA_hDops));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GGA_seahigh));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(GGAage));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(speedUBXint));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(heading));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(roll));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(ntrip_from_AgopenGPS));
  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat(Ntriphotspot_an));
  (RollHeadingrest.concat(BS));
//  (RollHeadingrest.concat(Paogi_Shit));
//  (RollHeadingrest.concat(BS));
  (RollHeadingrest.concat("*"));

  //  Serial.println(RollHeadingrest);

  //     RollHeadingrest = "$PAOGI," + GGAZeit + "," + NScoordinaten + "," + GGANordSued + "," + WEcoordinaten + "," + GGAWestEast + "," + String(GPSqualin1) + "," + String(GGASats) + "," + String(GGA_hDop) + "," + String(GGA_seahigh) + "," + String(GGAage) + "," + speedUBXint + "," + String(heading) + "," + String(roll) + ",,," + String(ntrip_from_AgopenGPS) + "," + String(Ntriphotspot_an) + ",*";

  for (XOR = 0, j = 0; j < RollHeadingrest.length(); j++) { // Berechnung Checksumme
    c = (unsigned char)RollHeadingrest.charAt(j);
    if (c == '*') break;
    if ((c != '$') && (c != '!')) XOR ^= c;
  }
  checksum = String(XOR, HEX);
  checksum.toUpperCase();
  RollHeadingrest.concat(checksum);

  //  Serial.println(RollHeadingrest);
  //  Serial.println(GGASatz);

  Paogi_Long = RollHeadingrest.length();
  //  Serial.println(Paogi_Long);

  Paogicheck();

  //  if ((Paogi_Long > 90) && (Paogi_Long < 97) && (jGGA6 == 45))
  
  if (Paogi_true)
  {
    if (send_Data_Via == 0) {
      unsigned int Bytelaenge = Paogi_Long + 1;
      RollHeadingrest.getBytes(UDPPAOGIMsg, Bytelaenge);
      UDPPAOGIMsg[Bytelaenge - 1] = 0x0D;
      UDPPAOGIMsg[Bytelaenge] = 0x0A;
      vTaskDelay(10);
      Serial.write(UDPPAOGIMsg, Bytelaenge + 1);
    }
    if (send_Data_Via == 1) {
      Bytelaenge = Paogi_Long + 1;
      RollHeadingrest.getBytes(ReplyBufferPAOGI, Bytelaenge);
      ReplyBufferPAOGI[Bytelaenge - 1] = 0x0D;
      ReplyBufferPAOGI[Bytelaenge] = 0x0A;
      Eth_udpPAOGI.beginPacket(Eth_ipDestination, portDestination);
      Eth_udpPAOGI.write(ReplyBufferPAOGI, Bytelaenge + 1);
      Eth_udpPAOGI.endPacket();
      if ((millis() - lastTime) > 10000) {
        //        Serial.println(" I´m still running " + String(millis() / 1000) + "0 Sekunden");
        Serial.println(RollHeadingrest);
        lastTime = millis();
      }
    }
  }
  else  {
    Paogi_Shit++;
    Paogi_true = true;
    /*    Serial.println(" ");
        Serial.println(" ");
        Serial.println(RollHeadingrest);
        Serial.println("Shit ist Sch.. " + String(Paogi_Long));
        Serial.println(" ");
        Serial.println(" ");
    */
  }
  if (send_amatron_nmea) {
    GGASatz_Korr.replace(GGAnord, NScoordinaten);
    GGASatz_Korr.replace(GGAeast, WEcoordinaten);
    Checksum_GGA_Korr();
    sendAMATRON();
  }
  //  Serial.println();
  if (!debugProtokoll)
  {
    //Serial.println(RollHeadingrest);
  }
  if (debugmode )    Serial.println();
  if (debugmode1)    Serial.println();
  if (debugmode2)    Serial.println();

}  // end PAOGI_builder
