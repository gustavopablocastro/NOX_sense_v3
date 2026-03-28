#ifndef _CONFIG_H
#define _CONFIG_H

#define deviceID 1

/*  Librerias */
#include <WiFiManager.h>
#include "Nextion.h"
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>
#include "root_CA_HIVEMQ.h"
#include <PubSubClient.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <math.h>

#define pump 1

/*  Definciión de variables globales  */
unsigned long time_med = 0;
unsigned long ts = 0;
char lastPPB[5] = "----"; 
//bool medir = false;

// parametros de corrección sensor NO
float kH = 1.046;       //1,04610383064516;
float kT = 0.9;     //0.9
float WEe = 264.5;
float AEe = 273.5;
float WEt = 327.6;              
float AEt = 318.6;              
float WEo = WEt - WEe;       //WEt - WEe 
float AEo = AEt - AEe;       //AEt - AEe 

float ISB = 432.0;          //sensibilidad en mv/ppm

///////////////////////////

const float T90 = 45.0f;
const float TAU = T90 / logf(10.0f);

const unsigned long SAMPLE_MS = 200;        // periodo entre muestras
const float RECORD_SECONDS = 10.0f;         // tiempo de medición efectiva
const float noisePPB = 15.0f;               //ISB noise  

// Límites de flujo válidos
const float FLOW_MIN = 0.45f;
const float FLOW_MAX = 0.55f;

unsigned long exhaleStartMs = 0;
unsigned long lastSampleMs = 0;
bool calibrated = false;
bool recording = false;
bool flowValid = true;

float sum_xC = 0.0f;
float sum_x2 = 0.0f;
float sum_err2 = 0.0f;
unsigned int sampleCount = 0;

float baseline_ppb = 0.0f;
float baseline_sum = 0.0f;
unsigned int baseline_count = 0;
bool baseline_ready = false;

////////////////////////////////////////////

const float WINDOW_START = 5.0f;   // inicio ajuste
const float WINDOW_END   = 15.0f;  // fin ajuste

const float PROJECTION_TIME = 45.0f;

float sum_z = 0.0f;
float sum_WEu = 0.0f;
float sum_AEu = 0.0f;
float sum_zWEu = 0.0f;
float sum_zAEu = 0.0f;
float sum_z2 = 0.0f;

bool regressionDone = false;


////////////////////////////////////////////
    
/* Definiciòn de variables WiFiManager */
const char* ap_name = "NO_Sense";
const char* ap_pass = "12345678";
const char* ap_menu[] = {"wifi","exit"};
//const char* ap_menu[] = {"wifi","exit","sep","update"};
bool wifiOK = 0;

/* Definición de variables ADS */
float multiplier = 0.125F;  //float multiplier = 0.1876F;
#define n_muestras 50
int NOX[n_muestras];
int promedio = 0;

//*** Parámetros MQTT
const int MQTT_PUERTO = 8883;
const char* MQTT_BROKER = "613edb4f1859456688f6a97e96ccbcdc.s1.eu.hivemq.cloud";
const String MQTT_RAIZ = "LuGu/";
const String MQTT_ESTADO = "data/";
const String MQTT_TOPICO_PUB_ESTADO = MQTT_RAIZ + MQTT_ESTADO;

//***  Credenciales MQTT
const String espMAC = WiFi.macAddress();
const char* MQTT_USU = "GC_I+D";
const char* MQTT_PWD = "Argentina1978";


/* Definición de variables NTP server */
#define NTP_SERVER "pool.ntp.org"
#define UTP_OFFSET  -10800
#define UTP_OFFSET_DST  0

/*  Definición de objetos  */
NexPage Main(0,0,"Main");
NexVariable vaWiFi(0,13,"vaWiFi");
NexVariable vaSSID(0,14,"vaSSID");
NexButton bConfig(2,13,"bConfig");
NexButton bMedicion(1,3,"bMedicion");
NexButton bTX(1,5,"bTX");
NexText tRet(2,14,"tRet");
NexSlider slider(1,2,"slider");
NexNumber nCount(1,15,"nCount");
NexCrop crop_Sople(1,16,"qSople");
NexText tMedicion(1,17,"tMedicion");
NexText tPPB(1,4,"tPPB");
NexText tTS(1,6,"tTS");
NexText tTX(1,18,"tTX");
NexRtc rtc;


NexTouch *nex_listen_list[] = 
{
    &bConfig,
    &bMedicion,
    &bTX,
    NULL
};

WiFiManager wm;
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
Adafruit_ADS1115 ads;


/* Librerias locales */
#include "sense.h"
#include "connect.h"
#include "helpers.h"
#include "mqtt.h"

#endif