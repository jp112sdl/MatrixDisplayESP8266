# MatrixDisplayESP8266

![Sample1](/images/Sample1.png)


## benötigte Hardware
* 1x Wemos D1 mini (z.B.: http://www.ebay.de/itm/172357074201)
* 2x Dot Matrix Modul 4 8x8 1088AS Matrix mit MAX7219-Treiber (z.B.: http://www.ebay.de/itm/232176384928)
* 2x Taster (beliebig... irgendwas, das beim Draufdrücken schließt :smiley:)

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

![Konfiguration1](/images/Back1.jpg)

## Programmierung 

Wie der Wemos D1 (Mini) angeschlossen und in die Arduino IDE integriert wird, ist hier gut beschrieben:

http://www.instructables.com/id/Programming-the-WeMos-Using-Arduino-SoftwareIDE/

Dort wird zwar das große Board gezeigt, aber die Integration in Arduino ist identisch mit dem Mini.

Wer sich nicht die Mühe mit der Arduino IDE machen möchte, kann sich mithilfe des `esptools` die fertig kompilierte Firmware `MatrixDisplayESP8266.ino.d1_mini.bin` auf den Wemos laden. 
Diese ist für den Einsatz von 2 4er Matrix-Module kompiliert.

Der Aufruf sollte in etwa so aussehen: 

`esptool.py --port COM4 write_flash 0x00000 MatrixDisplayESP8266.ino.d1_mini.bin` 

Herunterladen kann man das esptool bei Expressif:
https://github.com/espressif/esptool



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



## Bereitstellung der Daten
Der Wemos ruft zyklisch die Daten von der URL ab.
Die anzuzeigenden Werte sind als plain text, ohne jegliche Formatierung, nur mit einem **Semikolon getrennt** zu liefern und der **gesamte String muss in Anführungszeichen eingeschlossen sein.**
Die Rückgabe des HTTP-Requests sieht bspw. so aus: "Text1;Text2;Text3;Textn"

Die Daten können auch aus einer HomeMatic Systemvariable vom Typ "Zeichenkette" abgerufen werden.<br>
Dafür muss bei URL eingegeben werden:<br>
`http://1.2.3.4:8181/a.exe?ret=dom.GetObject(%22SV_Matrix%22).State()`<br>
wobei `1.2.3.4` durch die IP der CCU2 und `SV_Matrix` durch den Namen der Systemvariablen ersetzt werden müssen (die %22 müssen bestehen bleiben!).


*Sonderzeichen: Aufgrund der UTF-Zeichencodierung muss das Grad-Zeichen (°) als Dollar ($) übergeben werden.
Beispiel: "Luft: 12.5$C" wird dargestellt als "Luft: 12.5°C"*

### Eine weitere, detailliertere Anleitung (inkl. 3D Gehäuse-Vorlage) findet ihr auf [Michaels Blog]( https://www.schellenberger.biz/matrix-display-fuer-homematic-im-nachbau/).

## Anpassung der Anzahl der verwendeten DotMatrix-Module
Möchte man mehr/weniger Matrix-Module anschließen, so kann die Anzahl im Code leicht geändert werden.
In der Datei MatrixDisplayESP8266.ino:
```C
#define MAX_DEVICES 8
```
