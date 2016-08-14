/*  bfc3000 for ESP8266 - partly sophisticated bird feeding measurement
    Copyright (C) 2016 Bartholomäus Dedersen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Wire.h>
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <SNTPtime.h>


// set your wifi password in this header
#include "config.h"


SNTPtime NTP("2.de.pool.ntp.org");

/*
   The structure contains following fields:
   struct strDateTime
  {
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte dayofWeek;
  boolean valid;
  };
*/
strDateTime currentDate;
char timestamp[64];

void setup()
{

  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT); 
}


void loop()
{
  Serial.println("starting bfc3000");
  connectWiFi();
  Serial.println("connected to wifi");

 // set internal clock with SNTP server's time
  while (!NTP.setSNTPtime()) 
  {
      Serial.println("Trying to set time.."); // set internal clock
  }

  currentDate = NTP.getTime(1.0, 1);
  sprintf(timestamp,"%04i;%02d;%02d;%02d;%02d;%02d", currentDate.year, currentDate.day, currentDate.month, currentDate.hour, currentDate.minute, currentDate.second);
  Serial.print("Timestamp: ");
  Serial.println(timestamp);

  postToPhant();
  Serial.println("going to sleep");
  
  // sleep wakes on low edge on RST pin
  ESP.deepSleep(0);
}

// connect wifi and return when connected
void connectWiFi()
{
  Serial.print("connecting to wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }

  Serial.println(" - connection successful");
}

// Posts timestamp and unique id from device to phant service
void postToPhant()
{
  // LED turns on when we enter, it'll go off when we
  // successfully post.
  digitalWrite(BUILTIN_LED, HIGH);

  // get unique identifier by MAC of posting device
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String postedID = "wemos-" + macID;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(PHANT_HOST, PHANT_PORT)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/input/";
  url += PUBLIC_KEY;
  url += "?private_key=";
  url += PRIVATE_KEY;
  url += "&id=";
  url += postedID;
  url += "&feedtime=";
  url += timestamp;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + PHANT_HOST + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
    
  Serial.println("closing connection");

  // Before we exit, turn the LED off.
  digitalWrite(BUILTIN_LED, LOW);
}
