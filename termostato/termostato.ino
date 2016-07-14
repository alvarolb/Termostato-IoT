#define _DEBUG_
#include <ESP8266WiFi.h>
#include <ThingerWifi.h>
#include <string.h>
#include <stdlib.h>

#define USERNAME "usuario"
#define DEVICE_ID "id_dispositivo"
#define DEVICE_CREDENTIAL "credenciales"
#define SSID "wifi_ssid"
#define SSID_PASSWORD "wifi_password"

#define PIN_CALDERA D5
#define PIN_HUM D6
#define PIN_ROJO D0
#define PIN_VERDE D1
#define PIN_AZUL D2

ThingerWifi thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

float temperatura;
int humedad;
int presion;
float t_deseada;
int h_deseada;
int histeresis_temp;
int histeresis_hum;
boolean estado_caldera = false;
boolean estado_humidificador = false;
boolean estado = false;
boolean activar_twitter = false;
boolean activar_geo = false;

const char twitter_user[] = "evajurado_";

char salida[100] = "";

void setup() {
  pinMode(PIN_CALDERA, OUTPUT);
  pinMode(PIN_HUM, OUTPUT);
  pinMode(PIN_ROJO, OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AZUL, OUTPUT);
  Serial.begin(9600);

  thing.add_wifi(SSID, SSID_PASSWORD);
  
  thing["sensor"] << [](pson& in){
    Serial.println("Dato recibido");
    temperatura = in["temperatura"];
    humedad = in["humedad"]; 
    presion = in["presion"];
    thing.stream(thing["medidas_recibidas"]); 
  };
  
  thing["medidas_recibidas"] >> [](pson& out){
    out["temperatura"] = temperatura;
    out["humedad"] = humedad;
    out["presion"] = presion;
  };
  
  thing["temperatura_deseada"] << inputValue(t_deseada);
  thing["humedad_deseada"] << inputValue(h_deseada);
  thing["histeresis_temperatura"] << inputValue(histeresis_temp);
  thing["histeresis_humedad"] << inputValue(histeresis_hum);
  
  thing["prediccion"] << [](pson& in){
    int pred_temp = (int)in["temperatura"];
    int pred_hum = (int)in["humedad"];
    const char *pred_viento = (const char *)in["viento"];
    Serial.println("PREDICCION");
    sprintf(salida, "Temperatura = %dC  Humedad = %d  Viento = %s", pred_temp, pred_hum, pred_viento);
    Serial.println(salida);
    thing.stream(thing["prediccion_thinger"]);
  };
  
  thing["prediccion_thinger"] >> outputValue((const char *)salida);

  thing["twitter"] << [](pson& in){
    
      const char *usuario = in["usuario"];
      const char *texto = in["texto"];
      Serial.println("TWITTER");
      Serial.println(usuario);
      Serial.println(texto);
  
  if(activar_twitter){
      if(!strcmp(usuario, twitter_user)){
        char *pos_temp;
        Serial.println("Usuario autorizado");
        
        pos_temp = strstr(texto, " T ");
        t_deseada = atoi(pos_temp + 3);
        Serial.println(t_deseada);
  
        pos_temp = strstr(texto, " H ");
        h_deseada = atoi(pos_temp + 3);
        Serial.println(h_deseada);

        pson mensaje;
        mensaje["value1"] = "@evajurado_ Termostato configurado";
        thing.call_endpoint("responder_tweet", mensaje);
      }
      else {
        char aux[140] = "@";
        strcat(aux, usuario);
        strcat(aux, " No eres bienvenido :P");
        Serial.println(aux);
        pson mensaje;
        mensaje["value1"] = (const char *)aux;
        thing.call_endpoint("responder_tweet", mensaje);
      }    
    }
    else{
      char aux[140] = "@";
      strcat(aux, usuario);
      strcat(aux, " Servicio desactivado");
      Serial.println(aux);
      pson mensaje;
      mensaje["value1"] = (const char *)aux;
      thing.call_endpoint("responder_tweet", mensaje);  
    }
  };

  thing["encendido"] << inputValue(estado);
  thing["activar_twitter"] << inputValue(activar_twitter);
  thing["activar_geo"] << inputValue(activar_geo);
  
  thing["localizacion"] << [](pson& in){
    if(activar_geo){
      if(!strcmp((const char *)in["pos"], "entered"))
        estado = true;
      if(!strcmp((const char *)in["pos"], "exited"))
        estado = false; 
    } 
  };

  thing["estado_caldera"] >> [](pson& out){
    const char enc[] = "Encendida";
    const char apa[] = "Apagada";
    
    if(estado_caldera && estado)
      out = enc;
    else
      out = apa;
  };

  thing["estado_hum"] >> [](pson& out){
    const char enc[] = "Encendido";
    const char apa[] = "Apagado";
    
    if(estado_humidificador && estado)
      out = enc;
    else
      out = apa;
  };
}

void loop() {
  thing.handle();
  //Control caldera
  if(estado){
    if (temperatura < t_deseada - (float)histeresis_temp){
      estado_caldera = true;
    }
    if (temperatura > t_deseada + (float)histeresis_temp){
      estado_caldera = false; 
    }
    //Control humidificador
    if (humedad < h_deseada - histeresis_hum){
      estado_humidificador = true;
    }
    if (humedad > h_deseada + histeresis_hum){
      estado_humidificador = false;
    }
    digitalWrite(PIN_CALDERA, estado_caldera);
    digitalWrite(PIN_HUM, estado_humidificador);    
    //Control led estado
    if(estado_caldera){
      digitalWrite(PIN_ROJO, HIGH);
      digitalWrite(PIN_VERDE, LOW);
    }
    else
      digitalWrite(PIN_ROJO, LOW);
    if(estado_humidificador){
      digitalWrite(PIN_AZUL, HIGH);
      digitalWrite(PIN_VERDE, LOW);
    }
    else
      digitalWrite(PIN_AZUL, LOW);
    if(!estado_caldera && !estado_humidificador){
      digitalWrite(PIN_ROJO, LOW);
      digitalWrite(PIN_AZUL, LOW);
      for(int i = 0; i < 255; i+=5){
        analogWrite(PIN_VERDE, i);
        delay(10);
      }
      for(int i = 255; i > 0; i-=5){
        analogWrite(PIN_VERDE, i);
        delay(10);
      }    
    }
  }
  else {
      digitalWrite(PIN_CALDERA, LOW);
      digitalWrite(PIN_HUM, LOW);
      digitalWrite(PIN_ROJO, LOW);
      digitalWrite(PIN_VERDE, LOW);
      digitalWrite(PIN_AZUL, LOW); 
  }
}

