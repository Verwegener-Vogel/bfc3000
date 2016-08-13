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
#include <Phant.h>
#include <SNTPtime.h>


// set your wifi password in this header
#include "config.h"


SNTPtime NTP("de.pool.ntp.org");

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

void setup()
{

  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT); 
}


void loop()
{
  connectWiFi();
  postToPhant(getTime());

  // sleep wakes on low edge on RST pin
  ESP.deepSleep(0);
}

// connect wifi and return when connected
void connectWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }
}

// Gets the current time from NTP, prints it on serial
// Returns a time stamp
char *getTime() {
  strDateTime currentDate;
  currentDate = NTP.getTime(1.0, 1);
  NTP.printDateTime(currentDate);

  char timestamp[64];
  sprintf(timestamp,"%04i;%02d;%02d;%02d;%02d;%02d", currentDate.year, currentDate.day, currentDate.month, currentDate.hour, currentDate.minute, currentDate.second);

  return timestamp;
}

// Posts timestamp and unique id from device to phant service
// Returns 1 on success
int postToPhant(char timeStamp[])
{
  // LED turns on when we enter, it'll go off when we
  // successfully post.
  digitalWrite(BUILTIN_LED, HIGH);

  // Declare an object from the Phant library - phant
  // Gets its information from config.h
  Phant phant(PHANT_HOST, PUBLIC_KEY, PRIVATE_KEY);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String postedID = "wemos-" + macID;

  // add ID and timestamp string
  phant.add("id", postedID);
  phant.add("timestamp", timeStamp);

  // connect
  WiFiClient client;
  if (!client.connect(PHANT_HOST, PHANT_PORT))
  {
    // If we fail to connect, return 0.
    return 0;
  }
  
  // If we successfully connected, print our Phant post:
  client.print(phant.post());

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  // Before we exit, turn the LED off.
  digitalWrite(BUILTIN_LED, LOW);

  return 1;
}
