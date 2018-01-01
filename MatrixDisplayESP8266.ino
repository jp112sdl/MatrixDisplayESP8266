#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>

char refreshSeconds[10] = "60";
char scrollPause[10] = "5";
char url[255] = "";
char scrollSpeed[10] = "25";

textEffect_t scrollEffectIn  = PA_SCROLL_LEFT;
textEffect_t scrollEffectOut = PA_SCROLL_UP;
textPosition_t scrollAlign   = PA_CENTER;

String configFilename     = "sysconf.json";

//fixe Display
#define MAX_DEVICES 8
#define CLK_PIN   D7
#define DATA_PIN  D5
#define CS_PIN    D8
MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
uint8_t degC[] = {5, 6 , 15 , 9 , 15 , 6 };
uint8_t line[] = {4,  0, 8, 8, 8 };
uint8_t plus[] = {5, 8, 8, 62, 8, 8};
uint8_t block[] =  {3, 255, 255, 255};
uint8_t heart[] = {5, 28, 62, 124, 62, 28};

char curMessage[75];

int loopCount = 0;
int cnt = 0;
long lastMillis = 0;
String valueArray[20];
int valueCount = 0;
int intensity = 0;
int modeCnt = 0;
byte timeSetTryCount = 0;

//WifiManager - don't touch
#define IPSIZE              16
bool shouldSaveConfig        = false;
bool wifiManagerDebugOutput  = false;
char ip[15]      = "0.0.0.0";
char netmask[15] = "0.0.0.0";
char gw[15]      = "0.0.0.0";
bool startWifiManager = false;

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
unsigned int localPort = 2390;
const char* ntpServerName = "ptbtime2.ptb.de";
WiFiUDP udp;

#define key1 D1
#define key2 D2
bool key1last = false;
bool key2last = false;

void setup()
{
  Serial.begin(115200);
  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  P.begin();
  P.setIntensity(intensity);
  P.displayClear();
  P.displaySuspend(false);
  P.addChar('$', degC);
  P.addChar('-', line);
  P.addChar('_', block);
  P.addChar('?', heart);
  P.displayText("Starte...", PA_LEFT, 25, 10, PA_PRINT, PA_PRINT);
  P.displayAnimate();

  if (digitalRead(key1) == LOW || digitalRead(key2) == LOW) {
    startWifiManager = true;
  }

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  } else {
    if (!loadSysConfig()) {
      Serial.println("Failed to load config");
    } else {
      Serial.println("Config loaded");
    }
  }

  //Nachdem die Config geladen wurde...
  P.setIntensity(intensity);

  if (doWifiConnect() == true) {
    udp.begin(localPort);
    setSyncProvider(getNtpTime);
    setSyncInterval(3600);
    while (timeStatus() == timeNotSet) {
      Serial.println("Waiting for Time set");
      delay(1500);
      timeSetTryCount++;
      if (timeSetTryCount > 4) {
        P.displayText("NTP FAILURE", PA_LEFT, 25, 10, PA_PRINT, PA_PRINT);
        P.displayAnimate();
        delay(2000);
        ESP.restart();
      }
    }
    P.displayText(curMessage, scrollAlign, String(scrollSpeed).toInt(), String(scrollPause).toInt() * 1000, scrollEffectIn, scrollEffectOut);
    startOTAhandling();
  }
  else ESP.restart();
}

void loop() {
  ArduinoOTA.handle();
  if (digitalRead(key1) == LOW ) {
    if (!key1last) {
      key1last = true;
      intensity = intensity + 2;
      if (intensity > 14) intensity = 0;
      P.setIntensity(intensity);
      Serial.println("Set intensity to " + String(intensity));
      if (!saveSysConfig()) {
        Serial.println("Failed to save config");
      } else {
        Serial.println("Config saved");
      }
      delay(50);
    }
  } else {
    key1last = false;
  }

  if (digitalRead(key2) == LOW ) {
    if (!key2last) {
      key2last = true;
      modeCnt++;
      if (modeCnt > valueCount + 1) modeCnt = 0;
      Serial.println("Mode: " + String(modeCnt));
      delay(50);
    }
  } else {
    key2last = false;
  }

  if (((millis() - lastMillis > String(refreshSeconds).toInt() * 1000) || lastMillis == 0) && String(url) != "") {
    Serial.println("Fetching data from URL...");
    String valueString = loadDataFromURL();
    char buf[valueString.length() + 1];
    valueString.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    valueCount = 0;
    while ((str = strtok_r(p, ";", &p)) != NULL) {
      valueArray[valueCount] = str;
      valueCount++;
    }
    lastMillis = millis();
  }

  if (modeCnt == 0) {
    if (P.displayAnimate())
    {
      loopCount++;
      if (loopCount > valueCount || valueCount == 0) {
        String Zeit = calcTime(now());
        Zeit.toCharArray(curMessage, 10);
        loopCount = -1;
      } else {
        String currentValue = valueArray[loopCount];
        currentValue.toCharArray(curMessage, currentValue.length() + 1);
      }
      P.displayReset();
      P.displayText(curMessage, scrollAlign, String(scrollSpeed).toInt(), String(scrollPause).toInt() * 1000, scrollEffectIn, scrollEffectOut);
    }
  } else {
    if (valueCount > 0) {
      String currentValue = valueArray[modeCnt - 1];
      currentValue.toCharArray(curMessage, currentValue.length() + 1);
    }
    if (modeCnt > valueCount) {
      String Zeit = calcTime(now());
      Zeit.toCharArray(curMessage, 10);
    }

    P.displayText(curMessage, PA_CENTER, String(scrollSpeed).toInt(), 10, PA_PRINT, PA_PRINT);
    P.displayAnimate();
  }
}

String loadDataFromURL() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.setTimeout(3000);
    Serial.println("getState url: " + String(url));
    http.begin(url);
    int httpCode = http.GET();
    String payload = "error";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      Serial.println("HTTP " + String(url) + " fail");
      payload = " HTTP ERROR ";
    }
    http.end();

    if (payload.indexOf("</ret>") > 0) {
      payload = payload.substring(payload.indexOf("<ret>"));
      payload = payload.substring(5, payload.indexOf("</ret>"));
    } else {
      payload = payload.substring(1, payload.length() - 1);
    }
    Serial.println("getState payload = " + payload);
    return payload;
  } else ESP.restart();
}

