#ifndef _SENSE_H
#define _SENSE_H

float rFlow(); 



void startExhalation()
{
    recording = true;
    flowValid = true;
    exhaleStartMs = millis();
    lastSampleMs = exhaleStartMs;

    baseline_sum = 0.0f;
    baseline_count = 0;
    baseline_ready = false;

    sum_xC = 0.0f;
    sum_x2 = 0.0f;
    sum_err2 = 0.0f;
    sampleCount = 0;

    Serial.println("Exhalación iniciada");
    Serial.printf("T90 = %.1f s | tau = %.2f s\n", T90, TAU);
}

float readSensorPpb(){
    float WEu = ads.computeVolts(ads.readADC_SingleEnded(0))*1000;
    float AEu = ads.computeVolts(ads.readADC_SingleEnded(1))*1000;
    Serial.printf("WEu = %4.1fmV | AEu = %4.1fmV | ", WEu, AEu);

    float WEu_h = WEu * kH;
    float WEc = (WEu_h-WEe)-(kT*(WEo/AEo)*(AEu-AEe)); 
  return WEc*1000/ISB;
}

float estimateCInfinity(){
  if (sum_x2 < 1e-6f) return 0.0f;
  return sum_xC / sum_x2;
}



void testExhalation()
{
  if (!recording) return;

  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_MS) return;
  lastSampleMs = now;

  float t= (now - exhaleStartMs) / 1000.0f;
  
  float flow = rFlow();
  float C = readSensorPpb();
  Serial.printf("%.3f ; %.2f \n ",C, flow);
  

  /* ----------- FIN DE MEDICIÓN ----------- */
  if (t >= 45.0F)
  {
    recording = false;
    Serial.println("-------------------");
  }
}

#endif