  delay(10);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);          //UART2 Ausgangssignal weiterleiten (Amatron)
  //delay(10);
  Serial.println("Serial Txd liegt an Pin: " + String(TX));
