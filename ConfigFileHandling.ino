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

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  json.printTo(Serial);

  strcpy(ip,             json["ip"]);
  strcpy(netmask,        json["netmask"]);
  strcpy(gw,             json["gw"]);
  strcpy(refreshSeconds, json["refreshSeconds"]);
  strcpy(scrollPause,    json["scrollPause"]);
  strcpy(scrollSpeed,    json["scrollSpeed"]);
  strcpy(url,            json["url"]);

  intensity = json["intensity"];

  Serial.print("Loaded intensity: ");
  Serial.println(String(intensity));

  return true;
}

bool saveSysConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

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
  json.printTo(Serial);
  json.printTo(configFile);
  return true;
}
