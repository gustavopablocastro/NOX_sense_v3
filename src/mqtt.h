#ifndef _MQTT_H
#define _MQTT_H

/*
Verificador de la conexión MQTT
*/
void reconnectMqtt() {
	//int8_t ret;
	Serial.println("Connecting to MQTT... ");  	
	uint8_t retries = 10;
  	// Loop until we're reconnected
	while (!mqttClient.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Create a random client ID
		String clientId = espMAC;
		clientId += "-";
		clientId += String(random(0xffff), HEX);
		// Attempt to connect
		if (mqttClient.connect(clientId.c_str(),MQTT_USU,MQTT_PWD)) {
			Serial.println("MQTT connected");
			//mqtt_client.publish(MQTT_TOPICO_PUB_ESTADO.c_str(), "Hi EMQX I'm ESP32 ^^");
			//clientMQTT.subscribe(MQTT_TOPICO_SUB_COMANDO.c_str(),1);
		} else {
		Serial.print("failed, rc=");
		Serial.print(mqttClient.state());
		Serial.println(" try again in 5 seconds");
		}
		// Wait 5 seconds before retrying
		delay(5000);
		retries--;
		if (retries == 0) {
			ESP.restart();
		}
	}
	//Serial.println(" >> MQTT Connected!");
}

/*
Verificador de la conexión MQTT
*/
void verificarMqtt() {
  	if (!mqttClient.connected()){
		reconnectMqtt();
		}
}


/*
Forma y envía por MQTT un mensaje de estado
*/
void reporteMQTT() {
	char contenido[256];
	//sprintf(contenido, "{\"id\":\"%s\",\"t\":\"%.2f\",\"h\":\"%.2f\",\"p\":\"%.2f\"}",espMAC.c_str(),temp, hum, pres);
	mqttClient.publish(MQTT_TOPICO_PUB_ESTADO.c_str(), contenido);	
}

/*
Inicializa MQTT broker
*/
void inicializaMQTT(){
	mqttClient.setServer(MQTT_BROKER, MQTT_PUERTO);
  	mqttClient.setKeepAlive(60);
  	//clientMQTT.setCallback(callback);
	mqttClient.setSocketTimeout(20);
	reconnectMqtt();
}


#endif
