#include <ESP8266Wifi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "secret.h"

#define BUFF_SIZE 255

using ul = unsigned long;

const char *SSID{secret::SSID};
const char *PASSWORD{secret::PASSWORD};
const char *SERVER_NAME{"http://192.168.56.1:5000/update-sensor"};
int SENSOR_ID{1};

ul lastTime{0U};
ul timerDelay{5000U};

void setup()
{
  Serial.begin(9600);
  delay(3000);
  WiFi.begin(SSID, PASSWORD);

  // Print SSID
  Serial.print("Connecting to WiFi network with the SSID: ");
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // Print local IP
  Serial.print("Local IP is: ");
  Serial.println(WiFi.localIP());

  // Print initial reading delay
  Serial.print("First reading delay set to ");
  Serial.print(timerDelay);
  Serial.println(" milliseconds.");
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {

    if (WiFi.status() == WL_CONNECTION_LOST)
    {
      HTTPClient http;

      http.begin(SERVER_NAME);

      // Content type header
      http.addHeader("Content-Type", "application/json");

      // Format data to JSON
      char buff[BUFF_SIZE];
      const char *baseJsonStr{"{\"api_key\": \"%s\", \"sensor_id\": %i, \"value1\": %f}"};
      sprintf(buff, baseJsonStr, secret::API_KEY, SENSOR_ID, 25.666f);

      // Send HTTP POST request
      int httpResponseCode{http.POST(buff)};

      // Report back response
      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Reponse code: ");
        Serial.println(httpResponseCode);
        String payload{http.getString()};
        Serial.println(payload);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}