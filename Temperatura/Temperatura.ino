#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <math.h>

#define WIFI_SSID "Como estou dirigindo?"
#define WIFI_PASS "curupira cambota"
#define WIFI_CONN_BACKOFF_MS 1000

#define MQTT_SERVER_IP "192.168.1.9"
#define MQTT_SERVER_PORT 1883

#define STATUS_LED_PIN D0

float tempCelcius = 0.0f;
float tempFaren = 0.0f;
String deviceId;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Forward decls
void connectWifi(const char* ssid, const char* password);
void adcValToTemperatures(float adcVal, float *celcius, float *farenheit);
void printTempToSerial(float adcVal, float tempCelcius, float tempFaren);
void sendTemperature(float tempCelcius);
String getDeviceId();

void setup() {
  Serial.begin(115200);

  // Turn off WiFi to resolve an issue with unstable readings
  // See this: https://github.com/esp8266/Arduino/issues/2070
  WiFi.disconnect(true);
  connectWifi(WIFI_SSID, WIFI_PASS);
  mqttClient.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  deviceId = getDeviceId();
}

void loop() {
  int i = 0;
  float soma = 0;
  while(i<4){
  float adcVal = analogRead(A0);
  adcValToTemperatures(adcVal, &tempCelcius, &tempFaren);
  printTempToSerial(adcVal, tempCelcius, tempFaren);
  soma = soma + tempCelcius;
  i++;
  delay(500);
  }
  tempCelcius = soma/4;
  sendTemperature(tempCelcius);
  Serial.print(tempCelcius);
  delay(60000);
}

void connectWifi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.isConnected() != true) {
    Serial.print(".");
    delay(WIFI_CONN_BACKOFF_MS);
  }

  Serial.print("IP=");
  Serial.println(WiFi.localIP());
}

void adcValToTemperatures(float adcVal, float *celcius, float *farenheit) {
  *celcius = ((adcVal * (3300.0/1024)) - 500) / 10;
  *farenheit = (*celcius * 1.8) + 32;
}

void printTempToSerial(float adcVal, float tempCelcius, float tempFaren) {
  Serial.print("adc=");
  Serial.print(adcVal);
  Serial.print(" celcius=");
  Serial.print(tempCelcius);
  Serial.print(" farenheit=");
  Serial.println(tempFaren);
}

void sendTemperature(float tempCelcius) {
  digitalWrite(STATUS_LED_PIN, LOW);
  //connectWifi(WIFI_SSID, WIFI_PASS);
  if (mqttClient.connect(deviceId.c_str(), "pi", "raspberry")) {
    mqttClient.publish("jamal/temperatura", String(round(tempCelcius)).c_str(), true);
  } else {
    Serial.print("MQTT Send failed\n");
  }
  //WiFi.disconnect(true);
  digitalWrite(STATUS_LED_PIN, HIGH);
}

String getDeviceId() {
  return ("iot_" + WiFi.macAddress());
}
