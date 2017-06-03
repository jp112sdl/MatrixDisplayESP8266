String loadDataFromURL() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.setTimeout(5000);
    Serial.println("getState url: " + String(url));
    http.begin(url);
    int httpCode = http.GET();
    String payload = "error";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      Serial.println("HTTP " + String(url) + " fail");
    }
    http.end();

    payload = payload.substring(1, payload.length() - 1);
    Serial.println("getState payload = " + payload);
    return payload;
  } else ESP.restart();
}

/*bool doWifiConnect() {
  Serial.println("Connecting WLAN...");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, key);
  int waitCounter = 0;
  digitalWrite(LED_BUILTIN, LOW);
  String dots = "_";
  char a[] = "";
  while (WiFi.status() != WL_CONNECTED) {
    waitCounter++;
    dots = dots + "_";
    dots.toCharArray(a, 15);
    P.displayText(a, PA_LEFT, scrollSpeed, 10, PA_PRINT, PA_PRINT);
    P.displayAnimate();
    if (waitCounter == 20) {
      digitalWrite(LED_BUILTIN, HIGH);
      return false;
    }
    delay(500);
  }
  P.displayClear();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Wifi Connected");
  return true;
}*/
