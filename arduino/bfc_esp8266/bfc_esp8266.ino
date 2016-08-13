/*  bfc3000 for ESP8266 - partly sophisticated bird feeding measurement
 *  Copyright (C) 2016 Bartholomäus Dedersen
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Wire.h>
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <NTPtimeESP.h>

// set your wifi password in this header
#include "config.h"


NTPtime NTP("de.pool.ntp.org");

/*
 * The structure contains following fields:
 * struct strDateTime
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
}
}
 
 
void loop()
{
  connectWiFi();
  strDateTime dateTime = getTime();
  

  // sleep wakes on low edge on RST pin
  systemdeep_sleep(0)
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
// returns: strDateTime current date
strDateTime getTime() {
   strDateTime dateTime;

   dateTime = NTP.getNTPtime(1.0, 1);
   NTP.printDateTime(dateTime);
   return dateTime
}

 
