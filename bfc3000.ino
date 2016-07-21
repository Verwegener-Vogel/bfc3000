/*  bfc3000 - partly sophisticated bird feeding measurement
 *  
 *  Glue for sensors to measure bird feeding pattern with a low power embedded board.
 *  Sleeps in deep sleep until the bird is detected by a magnetic metal touch sensor.
 *  Writes a timestamp to a SD card for later data analysis.
 *  
 * Sleep code from http://playground.arduino.cc/Learning/ArduinoSleepCode
 * Copyright (C) 2016 Bartholomäus Dedersen
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
#include <avr/sleep.h>

 
int wakePin = 2;
int sdreaderVccPin = 7;

RTC_DS1307 RTC;
 
void wakeUpNow()
{
  // nop
}
 
void setup()
{
  pinMode(wakePin, INPUT_PULLUP);
  pinMode(sdreaderVccPin, OUTPUT);

  digitalWrite(sdreaderVccPin, LOW);
 
  Serial.begin(9600);
 
  Wire.begin();
  RTC.begin();
 
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 
  /* Now it is time to enable an interrupt. In the function call
   * attachInterrupt(A, B, C)
   * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
   *
   * B   Name of a function you want to execute while in interrupt A.
   *
   * C   Trigger mode of the interrupt pin. can be:
   *             LOW        a low level trigger
   *             CHANGE     a change in level trigger
   *             RISING     a rising edge of a level trigger
   *             FALLING    a falling edge of a level trigger
   *
   * In all but the IDLE sleep modes only LOW can be used.
   */
 
  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                      // wakeUpNow when pin 2 gets LOW
}
 
void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    /* Now it is time to enable an interrupt. We do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
 
    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
}
 
void loop()
{
  showTime();

  // wait until button gets released
  while(digitalRead(wakePin) == false)
  {
  }

  // real sensor bounces. maybe some wait before shutdown?
  // delay(5000);
  
  sleepNow();
}

void showTime() {
  // Power additional devices
   digitalWrite(sdreaderVccPin, HIGH);
   Serial1.begin(115200); 
  
   DateTime now = RTC.now();
   char timestamp[64];
   
   sprintf(timestamp,"%04i;%02i;%02i;%02i;%02i;%02i", now.year(), now.day(), now.month(), now.hour(), now.minute(), now.second());

   Serial.println(timestamp);
   
   Serial1.println(timestamp);
   Serial.flush();
   
   Serial1.end();
   digitalWrite(sdreaderVccPin, LOW);
 }
 
