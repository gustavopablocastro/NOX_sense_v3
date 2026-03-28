#include <Arduino.h>
#include "config.h"

void setup() {
    inicializarSetup();
}
void loop() {
    nexLoop(nex_listen_list);
    
    if(wifiOK){
        vaWiFi.setValue(1); 
        vaSSID.setText(WiFi.SSID().c_str());
    } else {
        vaWiFi.setValue(0); 
        vaSSID.setText("");
    }
    vExhalation();
    delay(300);
   
}
