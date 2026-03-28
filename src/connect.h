#ifndef _CONNECT_H
#define _CONNECT_H

void inicializaMQTT();

void setupNTP();

void wifiManager(){
    wm.setTitle("Configuración WiFi");
    wm.setMenu(ap_menu, sizeof(ap_menu));

    bool res = wm.autoConnect(ap_name,ap_pass);
    if(!res) {
        Serial.println("Failed to connect");
        tRet.Set_font_color_pco(63488);
        tRet.setText("FALLÓ CONEXIÓN !!");
        wifiOK = 0;
        ESP.restart();
    } 
    else { 
        Serial.println("WiFi connected");
        tRet.Set_font_color_pco(2016);
        tRet.setText("* CONEXIÓN OK *");
        //vaSSID.setText(WiFi.SSID().c_str());
        //vaWiFi.setValue(1);   
        delay(2000);
        setupNTP();
        
        tRet.setText("Conectando Servidor"); 
        
        inicializaMQTT();        
        tRet.setText("Conexión exitosa"); 
        
        wifiOK = 1;

        delay(2000);
    }
}

void setupNTP(){
        tRet.setText("Actualizando Hora");        
        delay(1000);

        configTime(UTP_OFFSET, UTP_OFFSET_DST, NTP_SERVER);

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)){
            Serial.println("RTC error");
            return;
        } else {
            Serial.println("Hora Actualizada");
        }

        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

        //char buffer[28];
        //sprintf(buffer,"%02d/%02d/%04d %02d:%02d:%02d", timeinfo.tm_mday, (timeinfo.tm_mon)+1, (timeinfo.tm_year)+1900, timeinfo.tm_hour , timeinfo.tm_min, timeinfo.tm_sec);
        //Serial.println(buffer);

        /*
        uint32_t  time[7] = {2016,11,25,12,34,50};
        time[0]= (timeinfo.tm_year)+1900;
        time[1]= (timeinfo.tm_mon)+1;        
        time[2]= timeinfo.tm_mday;
        time[3]= timeinfo.tm_hour;
        time[4]= timeinfo.tm_min;
        time[5]= timeinfo.tm_sec;
        rtc.write_rtc_time(time);
        */
}


#endif