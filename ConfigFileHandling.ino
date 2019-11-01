bool loadSysConfig() {
  File configFile = SPIFFS.open("/" + configFilename, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("loadSystemConfig JSON DeserializationError");
    return false;
  }
  JsonObject json = doc.as<JsonObject>();

  serializeJson(doc, Serial);
  ((json["ip"]).as<String>()).toCharArray(ip, IPSIZE);
  ((json["netmask"]).as<String>()).toCharArray(netmask, IPSIZE);
  ((json["gw"]).as<String>()).toCharArray(gw, IPSIZE);

  ((json["refreshSeconds"]).as<String>()).toCharArray(refreshSeconds, 10);
  ((json["scrollPause"]).as<String>()).toCharArray(scrollPause, 10);
  ((json["scrollSpeed"]).as<String>()).toCharArray(scrollSpeed, 10);
  ((json["url"]).as<String>()).toCharArray(url, 255);

  intensity = json["intensity"];

  Serial.print("Loaded intensity: ");
  Serial.println(String(intensity));

  return true;
}

bool saveSysConfig() {
  StaticJsonDocument<1024> doc;
  JsonObject json = doc.to<JsonObject>();

  json["ip"] = ip;
  json["netmask"] = netmask;
  json["gw"] = gw;
  json["refreshSeconds"] = refreshSeconds;
  json["scrollPause"] = scrollPause;
  json["url"] = url;
  json["scrollSpeed"] = scrollSpeed;
  json["intensity"] = intensity;

  File configFile = SPIFFS.open("/" + configFilename, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(doc, Serial);
  serializeJson(doc, configFile);
  Serial.println();
  return true;
}
