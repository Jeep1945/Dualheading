void UBXSpeed() {
  int Unit = 60;
  speedUBXint  = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;            // read UBX gSpeed from 1. F9P
  speedUBXint += (long)ubxmessagePVT.rawBufferPVT[Unit + 7] << 8;
  speedUBXint += (long)ubxmessagePVT.rawBufferPVT[Unit + 8] << 16 ;
  speedUBXint += (long)ubxmessagePVT.rawBufferPVT[Unit + 9] << 24 ;
  //  speedUBXint = speedUBXint / 514.444 / 1.9438445; // Km/h
  speedUBXint = speedUBXint / 514.444;  // Knoten
 //   Serial.println(" Speed UBX  " + String(speedUBXint, 3));
}  // end UBXSpeed()

// *********************************************************************************

void rollundheading() {
  //begin parser

  headingUBX  = (long)ubxmessage.rawBuffer[24 + 6] ;            // HeadingUBX read
  headingUBX += (long)ubxmessage.rawBuffer[25 + 6] << 8;
  headingUBX += (long)ubxmessage.rawBuffer[26 + 6] << 16 ;
  headingUBX += (long)ubxmessage.rawBuffer[27 + 6] << 24 ;
  headingUBX  = headingUBX / 100000;
  if (debugProtokoll) {
    Serial.print("  " + String(headingUBX, 2));
  }
  if (headingUBX < 0.1)  headingUBX = headingzuvor;
  else headingUBX += headingcorr;
  if (headingUBX >= 360) {
    headingUBX -= 360;
  }
  heading = headingUBX;
  if (debugmode1)    Serial.println(" UBX : " + String(headingUBX));
  //
  baseline  = (long)ubxmessage.rawBuffer[20 + 6] ;
  baseline += (long)ubxmessage.rawBuffer[21 + 6] << 8;
  baseline += (long)ubxmessage.rawBuffer[22 + 6] << 16 ;
  baseline += (long)ubxmessage.rawBuffer[23 + 6] << 24 ;
  baseline1 = (long)ubxmessage.rawBuffer[35 + 6];
  baseline1 = baseline1 / 100.0;
  if (debugProtokoll) {
    Serial.print(", B " + String(baseline, 2));
    Serial.print(", B1 " + String(baseline1, 2));
    Serial.print(", BL " + String(baseline + baseline1, 2));
  }
  baseline = baseline + baseline1;

  relPosD  = (long)ubxmessage.rawBuffer[16 + 6] ;              // hight read in cm
  relPosD += (long)ubxmessage.rawBuffer[17 + 6] << 8;
  relPosD += (long)ubxmessage.rawBuffer[18 + 6] << 16 ;
  relPosD += (long)ubxmessage.rawBuffer[19 + 6] << 24 ;
  relPosDH = (long)ubxmessage.rawBuffer[34 + 6];  // hight read in cm
  if (relPosDH >= 128) {
    relPosDH -= 256.0;
  }
  relPosDH = relPosDH / 100.0;
  relPosD = relPosD + relPosDH;          // in mm
  if (debugProtokoll) {
    Serial.print(", DH " + String(relPosDH, 2));
    Serial.print(", D " + String(relPosD, 2));
  }
  relPosD = relPosD / 100.0;

  // roll calculation
  rollcodi();
  if (debugProtokoll) {
    Serial.print(", roll " + String(roll, 2));
    Serial.print(", rollcorr " + String(rollCorrectionDistance, 2));
    Serial.print(",  ");
  }
}
// ende parser
