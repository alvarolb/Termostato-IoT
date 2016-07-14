#include <ESP8266WiFi.h>
#include <ThingerWifi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            D4    
#define DHTTYPE           DHT11   

#define USERNAME "usuario"
#define DEVICE_ID "id_dispositivo"
#define DEVICE_CREDENTIAL "credenciales"

#define SSID "wifi_ssid"
#define SSID_PASSWORD "wifi_password"

ThingerWifi thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
Adafruit_BMP085 bmp;
DHT_Unified dht(DHTPIN, DHTTYPE);

unsigned long m = 0;

void setup() {
  Serial.begin(9600);
  bmp.begin();
  thing.add_wifi(SSID, SSID_PASSWORD);
  dht.begin();
  thing["temperatura"] >> outputValue((float)bmp.readTemperature(););
  thing["humedad"] >> outputValue(leerhumedad());
  thing["presion"] >> outputValue(bmp.readPressure());
  m = millis();
}

int leerhumedad() {
  sensors_event_t event;  
  dht.humidity().getEvent(&event);
  return event.relative_humidity;
}

void loop() { 
  thing.handle();
  // envío de temperatura/humedad/presión cada minuto al termostato
  if(millis() >= 60000+m){
    float temperatura = (float)bmp.readTemperature();
    int humedad = leerhumedad();
    long presion = bmp.readPressure();
    pson termostato; 
    termostato["temperatura"] = temperatura;
    termostato["humedad"] = humedad;
    termostato["presion"] = presion;
    Serial.println("enviado");
    // call Thinger.io Device Call Endpoint
    thing.call_endpoint("termostato", termostato);
    m=millis();
  }
}
