String handleUDP() {
  int packetSize = CNTRLudp.parsePacket();
  if (packetSize) {
    Serial.println("Received " + String(packetSize) + " bytes from " + CNTRLudp.remoteIP().toString() + ", port " + String(CNTRLudp.remotePort()));
    int len = CNTRLudp.read(incomingPacket, 255);
    if (len > 0)
      incomingPacket[len] = 0;

    Serial.println("UDP packet contents: " + String(incomingPacket));

    CNTRLudp.beginPacket(CNTRLudp.remoteIP(), CNTRLudp.remotePort());
    CNTRLudp.write("OK");
    CNTRLudp.endPacket();

    String message = String(incomingPacket);
    message.trim();
    return message;
  } else return "";
}
