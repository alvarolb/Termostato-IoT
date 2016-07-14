# Termostato Conectado

Vamos a diseñar un termostato conectado que será capaz de controlar la temperatura y la humedad ambiente de nuestra casa. Para ello contaremos con una caldera y un humidificador. Además, este termostato contará con algunas funciones sociales, y reacciones "inteligentes".

## Integración con redes sociales

Crear una cuenta de Twitter para el termostato, que permita configurar tanto la temperatura, como la humedad, a través de un Tweet en el que le mencionen. Deberá controlar quién es el autor del mensaje para no permitir que cualquiera cambie los parámetros. Cuando la temperatura se haya cambiado, deberá enviar un Tweet indicando el cambio realizado. Usar IFTTT para reaccionar a un nuevo tweet, y para enviar uno nuevo.

Para nota: Sería interesante poder configurar la persona de Twitter que tiene acceso a hacer los cambios de forma remota a través de la API, y contestar negativamente a una petición de alguien que no tiene acceso. 

## Integración con servicios de predicción meteorológica

El termostato deberá ser capaz de recibir todos los días a las 10 de la mañana, la predicción del tiempo con el fin de visualizar la predicción meteorológica (idealmente en una pantalla). Nosotros utilizaremos el Dashboard para mostrar la predicción de forma textual. Usar IFTTT para que envíe la predicción meteorológica.

## Integración con localización

El termostato deberá ser capaz de desactivarse temporalmente si nos alejamos de un radio de más de 500 KM (aproximadamente). Y volverse a activar si volvemos a entrar en el radio de acción. Usar IFTTT con Android Location.

## Integración con otros sensores

El termostato deberá poder recibir información de otro sensor conectado que leerá la temperatura y humedad del exterior. Este sensor simulará ser un sensor externo que enviará la información periódicamente cada 1 minuto. El termostato conectado guardará esta información para su posterior monitorización. Usar un Endpoint en el sensor externo para que llame al termostato cada minuto.

## Opciones configurables

El termostato permitirá configurar los siguientes parámetros, ya sea a través de un Dashboard o de la API.

* Temperatura Objetivo
* Humedad Objetivo
* Histéresis Humedad (por tiempo o por tolerancia)
* Histéresis Temperatura (por tiempo o por tolerancia)
* Termostato activado o desactivado
* Activación/Desactivación de control por twitter (para que no reaccione)
* Activación/Desactivación de control por distancia (para que no reaccione)

## Dashboards

* Visualización de temperatura y humedad del interior y exterior
* Visualización de presión atmosférica
* Ajuste de histéresis para la temperatura y humedad
* Ajuste de temperatura y humedad objetivo
* Predicción meteorológica para el día actual
* Configuración de modo activado / desactivado
* Estado de la caldera (encendida/apagada)
* Estado del humidificador (encendido/apagado)

## Otras necesidades

El termostato deberá permitir configurar la temperatura y humedad (exclusivamente) de forma eventual a otra persona que no sea la dueña del termostato, a través de la aplicación móvil.

## Extras

Permitir la configuración de las credenciales de la plataforma a través de una página web generada por el termostato. Echar un vistazo al ejemplo ESP8266WebConfig de thinger.io.

## Recomendaciones

Para el desarrollo de este proyecto se recomiendan seguir los siguientes pasos:

* Empezar por la integración de los sensores, los actuadores, y la lógica básica de gestión del termostato (inclusive sin la conexión a thinger.io), comprobando que el sensor se lee bien, la caldera/humidificador se activa cuando baja la temperatura/humedad, y la histéresis funciona bien. Declarar todos los umbrales como variables globales del proyecto que luego modificaremos de forma remota
* Integrar conectividad y visualización en la plataforma. Añadiendo los primeros controles básicos para modificar las temperatura/humedad objetivo, visualizar el estado del termostato, etc.
* Probar las opciones de compartir termostato dando acceso sólo al control de temperatura y humedad
* Añadir la integración con el sensor externo, diseñando el sketch para el envío de los datos (necesitaréis otro dispositivo con los mismos sensores)
* Añadir la integración con el servicio de predicción meteorológica
* Añadir la integración con el servicio de localización de Android
* Añadir la integración con el servicio de Twitter
* Permitir configurar las credenciales del termostato a partir del acceso Web

Documentación Arduino + Thinger.io

http://docs.thinger.io/arduino/

Ejemplos en la comunidad

https://community.thinger.io

# Recordatorio Integración IFTTT -> Dispositivo

## URL 

https://api.thinger.io/v2/users/<<username>>/devices/<<device_id>>/<<resource>>?authorization=<<device_token>

(Cuidado con los espacios. Todo debe ir en una única línea)

### Método 

`POST`

### Content Type

`application/json`

## Body

```
{
    "in": {
        "name": "{{FullName}}",
        "location": "{{Location}}",
        "website": "{{UserWebsiteUrl}}"
    }
}
```

Esto sería un ejemplo particular que enviaría “name”, “location”, y “website” al dispositivo como cadena de caracteres. Si se van a enviar números, los ingredientes de la receta pueden ir sin las comillas para obtener los valores más fácilmente. Cambiar ingredientes de la receta y nombres de variables según sea necesario. “in” debe aparecer para asociarlo a la entrada del recurso.

Para obtener esta información en el dispositivo, podríamos tener un recurso como este:

```
thing["recurso"] << [](pson& in){
    const char* name = (const char*) in["name"];
    const char* location = (const char*) in["location"];
    const char* website = (const char*) in["website"];
    // trabajar aquí con el texto / información
 }; 
```
