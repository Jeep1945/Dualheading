void UBX_Data() {
  int Unit = 23;
  UBX_Sats = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;   // read UBX Number of sats from 1. F9P
  UBX_Satsi = int(UBX_Sats);
  Unit = 36;
  UBX_Sealevel  = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;            // read UBX Number of sats from 1. F9P
  UBX_Sealevel += (long)ubxmessagePVT.rawBufferPVT[Unit + 7] << 8;
  UBX_Sealevel += (long)ubxmessagePVT.rawBufferPVT[Unit + 8] << 16 ;
  UBX_Sealevel += (long)ubxmessagePVT.rawBufferPVT[Unit + 9] << 24 ;
  UBX_Sealeveld = UBX_Sealevel / 1000;
  Unit = 76;
  /*  UBX_DOP  = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;            // read UBX Number of sats from 1. F9P
    UBX_DOP += (long)ubxmessagePVT.rawBufferPVT[Unit + 7] << 8;
    UBX_DOP /=100;
  */
  //  if (UBX_DOP > 20) Serial.println(" UBX_DOP  " + String(UBX_DOP));
}  // end UBXSpeed()

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

// UBX_Timesec *********************************************************************************

void UBX_Timesec() {
  UBX_time_befor = UBX_time;
  int Unit = 8;
  UBX_time1 = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;            // read UBX hour from 1. F9P
  UBX_time2 = (long)ubxmessagePVT.rawBufferPVT[Unit + 7] ;            // read UBX min from 1. F9P
  UBX_time3 = (long)ubxmessagePVT.rawBufferPVT[Unit + 8] ;            // read UBX sec from 1. F9P
  Unit = 16;
  UBX_time4  = (long)ubxmessagePVT.rawBufferPVT[Unit + 6]  ;
  UBX_time4 += (long)ubxmessagePVT.rawBufferPVT[Unit + 7] << 8 ;
  UBX_time4 += (long)ubxmessagePVT.rawBufferPVT[Unit + 8] << 16 ;
  UBX_time4 += (long)ubxmessagePVT.rawBufferPVT[Unit + 9] << 24 ;
  UBX_time4 /= 10000000;

  UBX_time1i = int(UBX_time1);
  UBX_time2i = int(UBX_time2);
  UBX_time3i = int(UBX_time3);
  UBX_time4i = int(UBX_time4);
  //  Serial.print  (" UBX_time1  " + String(UBX_time1 * 10000, 0));
  //  Serial.print  (" UBX_time2  " + String(UBX_time2 * 100));
  UBX_time = "";
  if ((UBX_time1i) >= 10)  UBX_time.concat(UBX_time1i); else {
    UBX_time.concat("0");
    UBX_time.concat(UBX_time1i);
  }
  if ((UBX_time2i) >= 10)  UBX_time.concat(UBX_time2i); else {
    UBX_time.concat("0");
    UBX_time.concat(UBX_time2i);
  }
  if ((UBX_time3i) >= 10)  UBX_time.concat(UBX_time3i); else {
    UBX_time.concat("0");
    UBX_time.concat(UBX_time3i);
  }
  UBX_time.concat(".");
  if ((UBX_time4i) >= 10)  UBX_time.concat(UBX_time4i); else {
    UBX_time.concat("0");
    UBX_time.concat(UBX_time4i);
  }
  UBX_timed = UBX_time.toDouble();
}  // end UBX_Timesec()

// *********************************************************************************

void UBXCoordinats() {

  int Unit = 28;
  UBX_lat  = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;            // read UBX Latitude from 1. F9P
  UBX_lat += (long)ubxmessagePVT.rawBufferPVT[Unit + 7] << 8;
  UBX_lat += (long)ubxmessagePVT.rawBufferPVT[Unit + 8] << 16 ;
  UBX_lat += (long)ubxmessagePVT.rawBufferPVT[Unit + 9] << 24 ;
  UBX_lat /= 10000000;
  UBX_lat_double = double(UBX_lat);
  UBX_lat_int = (UBX_lat / 1);
  UBX_lat_double -= UBX_lat_int;

  if (debugmode_UBX) {
    Serial.print(" UBX_lat  " + String(UBX_lat, 7));
    Serial.print(" UBX_lat_int  " + String(UBX_lat_int));
    Serial.println(" UBX_lat_double  " + String(UBX_lat_double, 7));
  }
  Unit = 24;
  UBX_lon  = (long)ubxmessagePVT.rawBufferPVT[Unit + 6] ;            // read UBX Longitude from 1. F9P
  UBX_lon += (long)ubxmessagePVT.rawBufferPVT[Unit + 7] << 8;
  UBX_lon += (long)ubxmessagePVT.rawBufferPVT[Unit + 8] << 16 ;
  UBX_lon += (long)ubxmessagePVT.rawBufferPVT[Unit + 9] << 24 ;
  UBX_lon /= 10000000;
  UBX_lon_double = double(UBX_lon);
  UBX_lon_int = (UBX_lon / 1);
  UBX_lon_double -= UBX_lon_int;
  if (debugmode_UBX) {
    Serial.print(" UBX_lon  " + String(UBX_lon, 7));
    Serial.print(" UBX_lon_int  " + String(UBX_lon_int));
    Serial.println(" UBX_lon_double  " + String(UBX_lon_double, 7));
  }
}  // end UBXCoordinats()
// *********************************************************************************
