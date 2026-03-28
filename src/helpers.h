#ifndef _HELPERS_H
#define _HELPERS_H

void inicializaMQTT();


void sendNOX(){
    /* {"dev": 1 , "ts": "08-06-23 00:48:10" , "id": 23801336 , "NOX":[
    1, 4, 7, 10, 12, 14, 16, 18, 20, 22,
    24, 26, 28, 30, 32, 34, 36, 38, 40, 42,
    44, 46, 48, 50, 52, 54, 56, 58, 60, 62,
    64, 66, 68, 70, 72, 74, 76, 78, 80, 82,
    84, 86, 88, 90, 92, 94, 96, 98, 100]}
    */

    String buffer="{\"dev\": ";
    buffer += deviceID;
    buffer += ", \"ts\": ";


    struct tm timeinfo;
    getLocalTime(&timeinfo);
    char ts[32];
    sprintf(ts,"\"%02d/%02d/%02d %02d:%02d:%02d\"", timeinfo.tm_mday, (timeinfo.tm_mon)+1, (timeinfo.tm_year)-100, timeinfo.tm_hour , timeinfo.tm_min, timeinfo.tm_sec);

    buffer += ts;
    buffer += ", \"id\": ";

    char DNI[9];   
    tTS.getText(DNI, 8);
    int xx = atoi(DNI); 

    buffer += xx;
    buffer += ", ";

    String ppbJSON = "\"NOX\":[";
    for(int x=0; x  < n_muestras ; x++){
        if(x<=48) {
            ppbJSON += NOX[x];
            ppbJSON +=",";
            //Serial.printf("%3d,",NOX[x]);
        } else {
            //Serial.printf("%3d]\n",NOX[x]);    
            ppbJSON += NOX[x];
            ppbJSON +="]";
        }
    }

    buffer += ppbJSON;
    buffer += "}";

    Serial.println(buffer);
    mqttClient.publish(MQTT_TOPICO_PUB_ESTADO.c_str(), buffer.c_str());
}

void bConfigPopCallback(void *ptr){
    Serial.println("Configuración de WiFi Solicitada desde Touch");
    tRet.setText("Configurando...");  
    wifiManager();
    Main.show();
}

void bMedicionPopCallback(void *ptr){
    if(!recording) startExhalation();
}

void bTXPopCallback(void *ptr){
    Serial.println("Envío de medición - Touch");
    tTX.setText("Enviando Data");
    tPPB.getText(lastPPB, 5);
    if(strcmp(lastPPB, "----") == 0 ? 0 : 1){
        sendNOX();
        Serial.println("MQTT enviado");
        tTX.setText("Data ENVIADA");
    }

}


void inicializarSetup(){
    Serial.begin(115200);

    pinMode(pump, OUTPUT);
    digitalWrite(pump, LOW);

    espClient.setCACert(root_ca);
    
    //wm.resetSettings();

    nexInit();
    //vaWiFi.setValue(0);    
    //vaSSID.setText("");

    bConfig.attachPop(bConfigPopCallback, &bConfig);
    bTX.attachPop(bTXPopCallback,&bTX);
    bMedicion.attachPop(bMedicionPopCallback, &bMedicion); 

    delay(2000);

    ads.setGain(GAIN_ONE);            // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    if (!ads.begin()) {
        Serial.println("Failed to initialize ADS.");
        //while (1);
    }

    Serial.println("Sistema Inicializado...");
    
}

float rFlow(){
  int adc2=0;
  float volts2=0.0;
  double flow=0.0;
  adc2 = ads.readADC_SingleEnded(2);
  volts2 = ads.computeVolts(adc2);
  flow = ((0.2107*pow(volts2,3))-(0.6943*pow(volts2,2))+(0.9057*pow(volts2,1))-0.2861);
  //Serial.println("\n-----------------------------------------------------------");
  //Serial.printf("AIN2: %1.3f V \n",volts); 
  //Serial.printf("Flujo: %1.3f l/min \n",flow);  
  return flow;
}


#endif