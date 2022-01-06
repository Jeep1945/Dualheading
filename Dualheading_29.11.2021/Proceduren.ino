//Paogicheck---------------------------------------------------------------------------------------------

void Paogicheck() {

  if ((Paogi_Long < 91) || (Paogi_Long > 105) || (jGGA6 != 45)) {
    Paogi_true_UBX = false;
  }
  if ((heading == 0) || (heading == 90) || (heading == 180) || (heading == 270))  Paogi_true_UBX = false;
  if ((speedUBXint < 0) || (speedUBXint > 55))  Paogi_true_UBX = false;
  if ((heading < 0) || (heading > 360))  Paogi_true_UBX = false;
  if (UBX_Satsi < 14)  Paogi_true_UBX = false;
  if ((roll < -30) || (roll > 30))  Paogi_true_UBX = false;
}   // end void Paogicheck


//Coordinaten_check---------------------------------------------------------------------------------------------

void  Coordinaten_check() {

//  Serial.print("      rollnord  "  + String(rollnord));
//  Serial.println("    rolleast  "  + String(rolleast));
  //  Serial.println(" 1  " + String(rolleast1));
  if ((abs(rolleast - rolleast_before) > 15000) && (Coodinate1_check1 < 1)) {
    Paogi_true_UBX = false;
    Coodinate1_check1++;
  }
  else {
    rolleast_before = rolleast;
    Coodinate1_check1 = 0;
  }
  //  Serial.println(" 2  " + String(rolleast1));
  if ((rolleast1 < 1000) && (rolleast1 > 0)) WEcoordinaten = ("00" + String(rolleast1, 7));
  if ((rolleast1 >= 1000) && (rolleast1 < 10000)) WEcoordinaten = ("0" + String(rolleast1, 7));
  if (rolleast1 >= 10000) WEcoordinaten = String(rolleast1, 7);

  if ((abs(rollnord - rollnord_before) > 15000) && (Coodinate1_check2 < 1)) {
    Paogi_true_UBX = false;
    Coodinate1_check2++;
  }
  else {
    rollnord_before = rollnord;
    Coodinate1_check2 = 0;
  }
  if (rollnord1 >= 1000)  NScoordinaten = String(rollnord1, 7);
  if (rollnord1 < 1000)  NScoordinaten = ("0" + String(rollnord1, 7));

}  // end void Coordinaten_check
