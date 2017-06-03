# MatrixDisplayESP8266
Use MAX7219 DotMatrix Displays with ESP8266 and fetch Text from URL

## Verdrahtung
DotMatrix | Wemos D1
------------ | -------------
VCC       | +5V
GND       | GND
DIN       | D5
CLK       | D7
CS        | D8
Taste1 | D1
Taste2 | D2

Die Taster an D1 und D2 sind gegen GND zu schalten. *(Pullup-Widerstand wird nicht benötigt)*

## Taster - Funktion
* Drücken der Taste 1 ändert die Helligkeit der DotMatrix-Anzeige
* Drücken der Taste 2 wechselt zwischen dem Automatik-Modus (nacheinander Einblenden der Werte) und der Auswahl einer fixen Anzeige eines bestimmten Wertes.
* ein Gedrückthalten der Taste 1 oder der Taste 2 bei Einschalten/Stromzufuhr startet das Modul in den Konfigurationsmodus

## Konfiguration
Wird einer der beiden Taster bei Einschalten/Stromzufuhr gedrückt gehalten, startet das Modul im AP-Modus.
Es erscheint bei der WLAN-Suche vom Notebook/Handy ein neues WLAN namens DotMatrix-xx:xx:xx:xx:xx:xx.
Nach dem Verbinden mit diesem WLAN wird automatisch ein Popup des Konfigurationsportals geöffnet.

**WLAN konfigurieren** anklicken

![Konfiguration1](/images/Konfiguration1.png)


![Konfiguration2](/images/Konfiguration2.png)

**Beispiel**

![Konfiguration3](/images/Konfiguration3.png)



Die anzuzeigenden Werte sind durch **Semikolon getrennt** zu liefern und der **gesamte String muss in Anführungszeichen eingeschlossen sein.**
Die Rückgabe des HTTP-Requests sieht bspw. so aus: "Text1;Text2;Text3;Textn"

*Sonderzeichen: Aufgrund der UTF-Zeichencodierung muss das Grad-Zeichen (°) als Dollar ($) übergeben werden.
Beispiel: "Luft: 12.5$C" wird dargestellt als "Luft: 12.5°C"*
