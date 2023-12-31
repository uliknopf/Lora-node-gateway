# Lora-node-gateway #
Node: ATtiny85 + RFM95 / Gateway: ESP32 + RFM95

## LoRa mit RFM95 Funkmodul. ##
 Mein Projekt widmet sich der Übertragung von Temperatur-Daten über LoRa-Funk <ins>ohne</ins> LoRaWAN als Funkprotokoll. Durch Energieoptimierung ist eine mehrjährige Laufzeit der Node möglich. Dabei dient die Arduino IDE 2.2.1 als Entwicklungsumgebung.
 ---
<img src="./node.jpg" width="200px" height="200">

**Node:**
Das Funkmodul RFM95 (Semtech), zusammen mit dem Mikrocontroller ATtiny85, bilden die Sende-Einheit, ein Taster ermöglicht eine zusätzliche Auslösung einer Funk-Sendung. Um den Mikrocontroller ATtiny85 (ohne Entwicklungsboard) über die Arduino IDE zu programmieren, benutze ich einen seperaten Arduino Uno als ISP-Programmer. Hier der Link:

[link ISP-Programmer.](https://wolles-elektronikkiste.de/attiny-mit-arduino-code-programmieren)
Die Bibliothek RFM95_S ist speziell für die geringe Leistung des ATtinys ausgelegt und befindet sich im Library Ordner und muß in den library-Ordner der Arduino-IDE kopiert werden. Aufgrund von Energiesparmaßnahmen wird die Funk-Zelle ausschließlich senden, aber keine Nachrichten vom Gateway empfangen.
Die andere Bibliothek tinySPI (von Jack Christensen) kann über den Library-Manager der Arduino IDE bezogen werden. 
Alle anderen Bibliotheken sind Standard-Bibliotheken.
Ein ATtiny85 verbraucht bei 3 Volt im normalen Betrieb ca. 1 mA. Bei einer Batteriekapazität von ca. 1000 mAh ergibt das eine ungefähre Laufzeit von 40 Tagen. Zu wenig, daher wird jeweils für 1/2 Sekunde der Tiefschlafmodus aufgerufen. Das ermöglicht eine mehrjährige Betriebszeit. (Allerdings muß dann der Taster zur Auslösung einer Funknachricht auch mindestens 1/2 Sekunde gedrückt werden.)
Der Temperatursensor des ATtiny85 gibt momentan nur eine ungefähre Temperatur an, denn er muß individuell kalibriert werden (siehe .ino file, und es können auch keine negativen Werte gesendet werden).
Ich möchte noch erwähnen, daß im Schaltplan der Node, der MOSI-Pin des ATtiny85 an den MISO-Pin des RFM95 verbunden ist, und umgekehrt. Es handelt sich nicht um einen Druckfehler!

<img src="./gateway.jpg" width="200px" height="200">

**Gateway:**
Für das Gateway habe ich ein ESP32 (30 Pin Ausführung) ausgewählt.
In der Arduino-IDE kann man den Board-Manager aufrufen und den DOIT ESP32 DEVKIT V1 von Espressif auswählen um den Programm-Code zu übertragen. 
Bei Problemen folgender Link:

[link Board-Manager.](https://devarounder.de/esp32-board-in-arduino-ide-installieren-windows-mac-os-x-linux/) Die Bibliothek LoRa (von sandeepmistry) kann ebenso über den Library-Manager der Arduino IDE bezogen werden.

Die Antenne des RFM95 (für Node und Gateway) besteht aus einem 8,3 cm langen Drahtstummel.
Die Schaltpläne zeigen die Pin-Belegung der Komponenten und liegen als Bilddatei im png-Format vor.
Im freien Gelände lassen sich Nachrichten bis 1,5 km übertragen. In städtischer Umgebung hingegen nur 150 m.
Nach erfolgreicher Übertragung der Nachricht, kann ein Funkprotokoll ausgearbeitet werden und der ESP32 kann durch das integrierte Wlan-Modul ein privates Funknetz aufbauen.