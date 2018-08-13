# HAModule1
HAModule1 ist ein Modul für die Heimautomatisierung mit dem ESP32. 

## Aufsetzen von von der Build Umgebung

Es wird eine Linux zum Builden empfohlen.
ToolChain aufsetzen diesen Link folgen 
  https://esp-idf.readthedocs.io/en/latest/get-started/linux-setup.html
Die Schritte ausführen.

## Bibliothek
Verwendet werden Folgende bibliotheken.
in Folgender Struktur:
  ### ~/esp32/Arduino  
    
    git clone https://github.com/espressif/arduino-esp32.git
    
    Falls beim Komipieren probleme auftauchen sollten und das ist zu erwarten da sie die repository ständig in entwicklung 
    befindet.
    Die Version mit der es sicher funktionieren sollte.
      
      1. cd ~/esp32/Arduino
      2. git reset e346f20aa9f455f3147d9d018e09dd7c05818951 --hard
      3. git submodule foreach --recursive git reset --hard

  ### ~/esp32/esp/
    
    git clone --recursive https://github.com/espressif/esp-idf.git
    
    Falls beim Komipieren probleme auftauchen sollten und das ist zu erwarten da sie die repository ständig in entwicklung 
    befindet.
    Die Version mit der es sicher funktionieren sollte.
      
      1. cd $IDF_PATH
      2. git reset 8d5ec413d56975df2f893c98644a98a9c93b8e34 --hard
      3. git submodule foreach --recursive git reset --hard
   
   ### ~/esp32/HAModule1
      
      git clone https://github.com/WuXiaoqiao/HAModule1.git

Boost bibliothek installiern
  sudo apt-get install libboost-all-dev

Wenn alles richtig Eingerichtet worden ist solltest ihr in dem Ordner userhome/esp32/HAModule1 mit 
 make die software bauen können.

# Konfiguration

## WLAN

Wlan SSID und Passwort werden in der  HAModule1/main/include/Common.h konifguriert.

#define SSID_MACRO "Your_SSID"
#define PASS_MACRO "YourNetWorkpassword"

Einfach hier die beiden Strings ändern.
Danach kann mit dem CMD-Line git update-index --assume-unchanged Common.h das File vom push rausgenommen werden.

## Konfiguration der Schalter

Die Lichtschalter werden in dem main.cpp File von Zeile 55 bis 85 konfiguriert.
Hier werden in einem Vector sämtliche Schaltern die das ESP Modul schalten kann instanziert.
Es gibt 3 Klassen 
LichtSchalter(uint8_t in, uint8_t out, std::string bezeichnung);
Der Lichtschalter ist ein einfacher Toggle der zwischen An und Aus wechselt. 
in/out ist die Pin-Nummer auf der Platine. In sollte mit dem Taster verbunden sein.
Out sollte zum Relais gehen welches geschaltet werden soll.

RolloSchalter(uint8_t inAuf, uint8_t outAuf, uint8_t inAb, uint8_t outAb, std::string bezeichnung);
Der Rolloschalter hat 2 Taster und deshalb je 2 in und out parametern.

AutoLichtSchalter(uint8_t in, uint8_t out, std::string bezeichnung);
Der AutoLichtSchalter schaltet sich standardmäßig nach 10 min ab.
In und Out sind gleich wie beim Lichtschalter.









