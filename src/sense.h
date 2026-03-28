#ifndef _SENSE_H
#define _SENSE_H

float rFlow(); 

void calWEAE(){
  Serial.println("INICIO CALIBRACION DE BASE");
  float sum_WE = 0.0f;
  float sum_AE = 0.0f;

  for(int i=0; i<10; i++){
    sum_WE += ads.computeVolts(ads.readADC_SingleEnded(0))*1000;
    sum_AE += ads.computeVolts(ads.readADC_SingleEnded(1))*1000;
    delay(200);
  }

  WEt = sum_WE/10.0f;
  AEt = sum_AE/10.0f;
  Serial.println("FINAL CALIBRACION DE BASE");
  Serial.printf("WEt = %.2fmV - AEt = %.2fmV \n",WEt, AEt);

}

void startExhalation(){
    recording = true;
    flowValid = true;
    exhaleStartMs = millis();
    lastSampleMs = exhaleStartMs;

    sum_xC = 0.0f;
    sum_x2 = 0.0f;
    sum_err2 = 0.0f;
    sampleCount = 0;

    sum_z = 0.0f;
    sum_WEu = 0.0f;
    sum_AEu = 0.0f;
    sum_zWEu = 0.0f;
    sum_zAEu = 0.0f;
    sum_z2 = 0.0f;
    sum_err2 = 0.0f;
    sampleCount = 0;
    regressionDone = false;

    baseline_sum = 0.0f;
    baseline_count = 0;
    baseline_ready = false;

    /* Proceso de calibración de base de medición */
    //tMedicion.Set_font_color_pco(63488);
    //tMedicion.setText("AGUARDE...");
    //calWEAE();        // función de calibración de WEt y AEt
    tMedicion.setText("");
    crop_Sople.Set_background_crop_picc(2);

    Serial.println("Exhalación iniciada");
    Serial.printf("T90 = %.1f s | tau = %.2f s\n", T90, TAU);

    tPPB.Set_font_color_pco(65535);
    tPPB.setText("----");  // reinicia PPB resultado
}

float calcPPB(float WEu, float AEu){
    float WEu_h = WEu * kH;
    float WEc = (WEu_h-WEe)-(kT*(WEo/AEo)*(AEu-AEe)); 
  return WEc*1000/ISB;
}

float readSensorWEu(){
  return ads.computeVolts(ads.readADC_SingleEnded(0))*1000;
}

float readSensorAEu(){
  return ads.computeVolts(ads.readADC_SingleEnded(1))*1000;
}

float estimateCInfinity(){
  if (sum_x2 < 1e-6f) return 0.0f;
  return sum_xC / sum_x2;
}

float V45_est(float sum_zv, float sum_v,float denom, float z45){
  float A = (sampleCount * sum_zv - sum_z * sum_v) / denom;
  float Cinf = (sum_v - A * sum_z) / sampleCount;
  return Cinf + A * z45;
}

void testExhalation(){
  if (!recording) return;

  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_MS) return;
  lastSampleMs = now;

  float t= (now - exhaleStartMs) / 1000.0f;
  
  float flow = rFlow();
  float WEu = readSensorWEu();
  float AEu = readSensorAEu();
  Serial.printf("WEt = %4.1fmV | AEt = %4.1fmV | WEu = %.1fmV | AEu = %4.1fmV | flow = %.2f | t = %.3f\n",WEt, AEt, WEu, AEu, flow, t);
  
  /* ----------- FIN DE MEDICIÓN ----------- */
  if (t >= 45.0){
    recording = false;
    Serial.println("-------------------");
  }
}

void vExhalation(){
  if (!recording) return;

  static unsigned long lastSample = 0;
  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_MS) return;
  lastSample = now;

  float t = (now - exhaleStartMs) / 1000.0f;

  /* Muestra segundos en el HMI */
  nCount.Set_font_color_pco(63488);   // color del contador de seg de medicion estable HMI ROJO
  nCount.setValue(t);             // segundos de medicion HMI

  /* ----------- CONTROL DE FLUJO ----------- */
  float flow = rFlow();             // Obtiene el flujo de aire litros/min  

  /* Visualiza valores de Flujo en el HMI */
  u_int32_t aux = int(flow*100);
  if (aux < 30) aux = 30;
  if (aux > 70) aux = 70;
  u_int32_t flow_hmi = map(aux,30,70,0,600);   
  slider.setValue(flow_hmi);                  // muestra en el HMI el flujo escalado
  

  /* ----------- LECTURA SENSOR NOx ----------- */
  if (t >= WINDOW_START && t <= WINDOW_END){
    
    /* compara parametros validos de flujo */
    if (flow < FLOW_MIN || flow > FLOW_MAX){
      flowValid = false;
    }
    
    float WEu = readSensorWEu();
    float AEu = readSensorAEu();
    Serial.printf("WEt = %4.1fmV | AEt = %4.1fmV | WEu = %4.1fmV | AEu = %4.1fmV | flow = %.2f \n", WEt, AEt, WEu, AEu, flow);

    float z = expf(-t / TAU);

    sum_z  += z;
    sum_WEu  += WEu;
    sum_AEu  += AEu;
    sum_zWEu += z * WEu;
    sum_zAEu += z * AEu;
    sum_z2 += z * z;

    sampleCount++;
  }

  if (!regressionDone && t > WINDOW_END && sampleCount > 5){
    float denom = sampleCount * sum_z2 - sum_z * sum_z;

    if (fabsf(denom) < 1e-6f){
        Serial.println("Regresión inválida");
        regressionDone = true;
        return;
    }

    float z45 = expf(-PROJECTION_TIME / TAU);
    float WEu_est = V45_est(sum_zWEu, sum_WEu, denom, z45);
    float AEu_est = V45_est(sum_zAEu, sum_AEu, denom, z45);
    float NOx_45s = calcPPB(WEu_est, AEu_est) - noisePPB;

    Serial.println("----- RESULTADO PROYECTADO -----");
    Serial.printf("WEu_45s = %.3f mV | AEu_45s = %.3f mV \n",WEu_est, AEu_est);
    Serial.printf("Nox = %4.0f\n", NOx_45s);
    Serial.println("--------------------------------");


    /* Final de medición no estable */
    if(!flowValid){
      tMedicion.Set_font_color_pco(63488);
      tMedicion.setText("Medición NO estable");

      // restart banderas
      regressionDone = true;
      recording = false;
      flowValid = true;
      calibrated = false;

      // restart display
      slider.setValue(0);                       // slider a 0
      crop_Sople.Set_background_crop_picc(3);   // borra la leyenda "sople..."
      nCount.Set_font_color_pco(68);            // elimina color del contador de seg de medicion 
      nCount.setValue(0);                       // segundos de medicion HMI

      digitalWrite(pump, HIGH);
      delay(15000);
      digitalWrite(pump, LOW);
      return;
    }


    /* Final de medición ESTABLE */
    tMedicion.Set_font_color_pco(2016);
    tMedicion.setText("Medición Exitosa");
                    
    tPPB.Set_font_color_pco(2016);
    char ppbNOc[5];
    sprintf(ppbNOc,"%4.0f",NOx_45s);
    tPPB.setText(ppbNOc);

    //habilita boton trasnmisión
    //bTX.Set_background_crop_picc(2);
    
    // restart banderas
    regressionDone = true;
    recording = false;
    calibrated = false;

    // restart display
    slider.setValue(0);                       // slider a 0
    crop_Sople.Set_background_crop_picc(3);   // borra la leyenda "sople..."
    nCount.Set_font_color_pco(68);            // elimina color del contador de seg de medicion 
    nCount.setValue(0);                       // segundos de medicion HMI

      digitalWrite(pump, HIGH);
      delay(15000);
      digitalWrite(pump, LOW);
  }

}

#endif