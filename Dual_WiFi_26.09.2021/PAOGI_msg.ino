void PAOGI_builder() {

  speeed1 = speeed;

  // build the PAOGI msg
  rolleast = (rolleast * 0.0000001);
  if ((abs(rolleast - rolleast_before) < 2) || (Coodinate_check1 < 4)) {
    rolleast = rolleast_before;
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
    rollnord = rollnord_before;
    Coodinate_check2++;
  }
  else {
    rollnord_before = rollnord;
    Coodinate_check2 = 0;
  }
  if (rollnord >= 1000)  NScoordinaten = String(rollnord, 7);
  if (rollnord < 1000)  NScoordinaten = ("0" + String(rollnord, 7));



  //  RollHeadingrest = "$PAOGI," + GGAZeit + "," + NScoordinaten + "," + GGANordSued + "," + WEcoordinaten + "," + GGAWestEast + "," + String(GPSqualin1) + "," + GGAdaten1 + ",," + speeed1 + "," + String(heading) + "," + String(roll) + ",,,*";
  RollHeadingrest = "$PAOGI," + GGAZeit + "," + NScoordinaten + "," + GGANordSued + "," + WEcoordinaten + "," + GGAWestEast + "," + String(GPSqualin1) + "," + GGAdaten1 + ",," + speeed1 + "," + String(heading) + "," + String(roll) + ",,,*";
  headingzuvor = heading;
  //        Serial.println(RollHeadingrest);

  for (XOR = 0, j = 0; j < RollHeadingrest.length(); j++) { // Berechnung Checksumme
    c = (unsigned char)RollHeadingrest.charAt(j);
    if (c == '*') break;
    if ((c != '$') && (c != '!')) XOR ^= c;
  }
  checksum = String(XOR, HEX);
  checksum.toUpperCase();
  RollHeadingrest += checksum;

  //  Serial.println(RollHeadingrest);
  //  Serial.println(GGASatz);

  Paogi_Long = RollHeadingrest.length();
  //  Serial.println(Paogi_Long);

  Paogi_true = true;

  Paogicheck();

  //  if ((Paogi_Long > 90) && (Paogi_Long < 97) && (jGGA6 == 45))
  if (Paogi_true)
  {
    if (send_Data_Via == 0) {
      unsigned int Bytelaenge = RollHeadingrest.length() + 1;
      RollHeadingrest.getBytes(UDPPAOGIMsg, Bytelaenge);
      UDPPAOGIMsg[Bytelaenge - 1] = 0x0D;
      UDPPAOGIMsg[Bytelaenge] = 0x0A;
      delay(10);
      Serial.write(UDPPAOGIMsg, Bytelaenge + 1);
    }
    if (send_Data_Via == 1) {
      Bytelaenge = RollHeadingrest.length() + 1;
      RollHeadingrest.getBytes(ReplyBufferPAOGI, Bytelaenge);
      ReplyBufferPAOGI[Bytelaenge - 1] = 0x0D;
      ReplyBufferPAOGI[Bytelaenge] = 0x0A;
      delay(10);
      udpRoof.writeTo(ReplyBufferPAOGI, Bytelaenge + 1, ipDestination1, portDestination);
      if ((millis() - lastTime) > 10000) {
        //        Serial.println(" I´m still running " + String(millis() / 1000) + "0 Sekunden");
        Serial.println(RollHeadingrest);
        lastTime = millis();
      }
    }
  }
  else  {
    Paogi_true = true;
    //    Serial.println(" ");
    //    Serial.println(RollHeadingrest);
    //    Serial.println("Shit ist Sch.. " + String(Paogi_Long));
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
