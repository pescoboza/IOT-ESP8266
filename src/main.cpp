#include <ESP8266Wifi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "config.h"

#define BUFF_SIZE 255
unsigned long lastTime{0U};

void setup(){
    Serial.begin(cfg::BAUD_RATE);
    delay(cfg::INITIAL_DELAY_MS);
    WiFi.begin(cfg::SSID, cfg::PASSWORD);

    // Print SSID
    Serial.print("Connecting to WiFi network with the SSID: ");
    Serial.println(cfg::SSID);
    while (WiFi.status() != WL_CONNECTED){
        delay(cfg::WIFI_CONNECT_DELAY_MS);
        Serial.print(".");
    }
    Serial.println("");

    // Print local IP
    Serial.print("Local IP is: ");
    Serial.println(WiFi.localIP());

    // Print initial reading delay
    Serial.print("First reading delay set to ");
    Serial.print(cfg::TIME_DELAY_MS);
    Serial.println(" milliseconds.");
}

void loop(){
    if ((millis() - lastTime) > cfg::TIME_DELAY_MS){

        if (WiFi.status() != WL_CONNECTION_LOST){
            HTTPClient http;

            http.begin(cfg::SERVER_NAME);

            // Content type header
            http.addHeader("Content-Type", "application/json");

            // Format data to JSON
            char buff[BUFF_SIZE];
            const char *baseJsonStr{"{\"api_key\": \"%s\", \"sensor_id\": %i, \"value1\": %f}"};
            sprintf(buff, baseJsonStr, cfg::API_KEY, cfg::SENSOR_ID, 25.666f);

            // Send HTTP POST request
            int httpResponseCode{http.POST(buff)};

            // Report back response
            if (httpResponseCode > 0){
                Serial.print("HTTP Reponse code: ");
                Serial.println(httpResponseCode);
                String payload{http.getString()};
                Serial.println(payload);
            }
            else{
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }

            http.end();
        }
        else{
            Serial.println("WiFi Disconnected, rebooting...");
            ESP.restart();
        }
        lastTime = millis();
    }
}