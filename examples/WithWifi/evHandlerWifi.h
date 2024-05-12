/*************************************************
 *************************************************
    handlerWifi.h   validation of lib bNodes and betaEvents32 to deal nicely with events programing with Arduino
    Copyright 2024 Pierre HENRY net23@frdev.com All - right reserved.

  This file is part of bNodes.

    bNodes is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    bNodes is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with betaEvents.  If not, see <https://www.gnu.org/licenses/lglp.txt>.

  History

   works with betaEvents32 3.0  and bNodes

    V1.0  16/03/2024
    - cleanup to remove Wifi from .ino   work only with STA mode
    give evxConnected  end evxDisconnected  
    TODO: deal with AP mode
    TODO: deal with lowpowermode 
    TODO: deal with ESP32
    Ajout de la gestion de la synchro externe de l'heure pout timeLib  (setSyncProvider)

    

     *************************************************/
#pragma once
#include <Arduino.h>
#include <EventsManagerESP.h>


typedef enum { evxWifiOff,
               evxWifiOn,
               evxWifiErr,
               evxTimeZoneChanged,
               evxTimestampChanged } exWifi_t;

class evHandlerWifi : public evHandler {
public:
  evHandlerWifi(){};
  virtual void begin() override;
  //virtual byte get() override;
  virtual void handle() override;  //check wifi.status every second to push evxxxx
  bool connected = false;
  uint8_t status = 0;       //enum { evxWifiOff, evxWifiOn, evxWifiErr } exWifi_t;
  String nodeName = "???";  // nom de  la device (a configurer avec NODE=)"
  bool coldBoot = true;
  //bool timeZoneChanged = false;
  int clockDelta = 0;
  int clockLastTry = 0;
  //int timeZone = 0;
private:
};
time_t getFullwwwTime();
extern evHandlerWifi hdlWifi;
