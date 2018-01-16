#include "DHT.h"

#define DHT11_PIN 2
#define DHTTYPE DHT11

DHT dht(DHT11_PIN, DHTTYPE);


void setup(){
  Serial.begin(115200);
  Serial.print("INICIANDO");
  dht.begin();
}

void loop(){
  delay(2000);
  Serial.print("Temperature = ");
  float temperatura = dht.readTemperature();
  Serial.println(temperatura);  
  Serial.print("Humidity = ");
  float humidade = dht.readHumidity();
  Serial.println(humidade);
  
}



