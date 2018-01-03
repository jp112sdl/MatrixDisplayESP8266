time_t getNtpTime() {
  IPAddress ntpServerIP;
  while (NTPudp.parsePacket() > 0) ;
  WiFi.hostByName(ntpServerName, ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1600) {
    int size = NTPudp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      NTPudp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      Serial.println("NTP time was set");
      return secsSince1900 - 2208988800UL;
    }
  }
  return 0;
}

unsigned long sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  NTPudp.beginPacket(address, 123);
  NTPudp.write(packetBuffer, NTP_PACKET_SIZE);
  NTPudp.endPacket();
}

String calcTime(time_t t) {
  byte Stunde = hour(t) + 1;
  Stunde = (summertime(t, 0)) ? Stunde + 1 : Stunde;
  return String((Stunde < 10) ? "0" : "" ) + String(Stunde) +  ":" + String((minute(t) < 10) ? "0" : "" )  + String(minute(t)) + " Uhr";
}

boolean summertime(time_t t, byte tzHours) {
  if (month(t) < 3 || month(t) > 10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (month(t) > 3 && month(t) < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (month(t) == 3 && (hour(t) + 24 * day(t)) >= (1 + tzHours + 24 * (31 - (5 * year(t) / 4 + 4) % 7)) || month(t) == 10 && (hour(t) + 24 * day(t)) < (1 + tzHours + 24 * (31 - (5 * year(t) / 4 + 1) % 7)))
    return true;
  else
    return false;
}
