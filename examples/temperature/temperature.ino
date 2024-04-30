/*************************************************

  ***********************************************
    Sketch temperature.ino   a sample forlib betaEvents to deal nicely with events programing with Arduino
    Copyright 2024 Pierre HENRY net23@frdev.com All - right reserved.

  This file is part of betaEvents.

    betaEvents is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    betaEvents is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with betaEvents.  If not, see <https://www.gnu.org/licenses/lglp.txt>.

  temperature is a an exemple of stand alone code to understand Betaevents concept
  this version is specifty crafted for 32 bit processor as ESP8266

  
*/


// Name of this application
#define APP_NAME "temperature V1.0"




// Eventslist
/* Evenements du Manager (voir evHandlers.h)
  evNill = 0,      // No event  about 1 every milisecond but do not use them for delay Use delayedPushMillis(delay,event)
  ev100Hz,         // tick 100HZ    non cumulative (see betaEvent.h)
  ev10Hz,          // tick 10HZ     non cumulative (see betaEvent.h)
  ev1Hz,           // un tick 1HZ   cumulative (see betaEvent.h)
  ev24H,           // 24H when timestamp pass over 24H
  evInit,
  evInChar,
  evInString,
*/


// List of Event specific for this APP

// Utilisation d'enum class pour une meilleure encapsulation et un typage fort
enum tevUser : uint8_t {
  evBP0 = 100,   // standard event for bp0 on D4
  evLed0,       // standard event for led0 on D4
 
};
//}



#include <BetaEventsESP.h>  // this will create an evManager called Events with a push buton BP0 and a Led LED0


void setup() {
  Events.begin();

  Serial.println(F("\r\n\n" APP_NAME));

  T_println("Bonjour...");
}

bool sleep = true;  //Allow to disable sleep time with a "S" command line

void loop() {
  Events.get(sleep);  //get the next event
  //if (Events.code >= 100) { DV_println(Events.code); };
  Events.handle();        //pass the event to every evHandlers
  switch (Events.code) {  //deal whith th event
    case evInit:          //evInit append once affter every evHandles haves done with 'begin'
      {
        Serial.println("Init");  //just infor the user that an evInit append
      }
      break;





    case ev1Hz:
      {
        //DV_print(second());
        //DV_println(helperFreeRam());
      }
      break;

    case evBP0:  //
      switch (Events.ext) {
        case evxOn:
          Serial.println("BP0 ON");
          Led0.setMillisec(500);
          break;
        case evxLongOn:
          Serial.println("BP0 LONG ON");
          break;
        case evxOff:
          Serial.println("BP0 OFF");
          Led0.setMillisec(5000);
          break;
        case evxLongOff:
          Serial.println("BP0 LONG OFF");
          break;
      }
      break;

    case evInChar:
      //DV_println(Events.cParam);
      switch (Events.cParam) {
        case '1': delay(10); break;
        case '2': delay(100); break;
        case '3': delay(1000); break;
        case '4': delay(10000); break;
      }
      break;



    case evInString:
 
      if (Events.strPtrParam->equals("LEDON")) {
        Led0.setOn(true);
        T_println("LED ON");
      }

      if (Events.strPtrParam->equals("LEDOFF")) {
        Led0.setOn(false);
        T_println("LED OFF");
      }
      break;
  }
}