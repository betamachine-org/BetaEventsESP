#include "TimeLib.h"
#include "evHandlerWifi.h"
#include <ArduinoOTA.h>
#include <Arduino_JSON.h>
#include <ESP8266mDNS.h>
//#include "bNodesTools.h"


//#define NO_DEBUG

//#define DEBUG_ESP_HTTP_SERVER
//#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

//#include <Arduino_JSON.h>



//evHandlerWifi::evHandlerWifi(){};
const byte postInitDelay = 10;  //delay post init en secondes
const int defaultSyncInterval = 6 * 60 * 60;

/****** wifiTimeProvider
if wifi is connected it will try a syncr on an intenational API to get time
   it will get local time + timeZone (offset from UTC in hours)
   it will send an evTimeChanged (delta in sec) if needed
if not it will try to grab time from RTC memory 
if RTC time not here it will try from timestamp of history file who is adjusted every hour
if no time it will fail 

if you need another type of syncr you should use setime()
syncInterval should be set around 6 hours


*****************************/


extern time_t getWWWTime();
time_t wifiTimeProvider();


savedRTCmemory_t savedRTCmemory;

//call back to give time for littleFS
// just return current
time_t myTimeCallback() {
  return bNode.currentTime;  // UNIX timestamp
};



void evHandlerWifi::begin() {
  enableWiFiAtBootTime();  // mendatory for autoconnect WiFi with ESP8266 kernel 3.0
  //  normaly not needed

  if (WiFi.getMode() != WIFI_STA) {
    Serial.println(F("!!! Force WiFi to STA mode !!!"));
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin();
    //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
  MyLittleFS.begin();
  LittleFS.setTimeCallback(myTimeCallback);
  DT_println("wifiTimeProvider started");
  setSyncProvider(wifiTimeProvider);  //this will call wifiTimeProvider() immediatly;
  /*
 // System de fichier
  // il gere
  if (!MyLittleFS.begin()) {
    Serial.println(F("erreur MyLittleFS"));
    helperReset();
    //fatalError(3);
  }
  */
  DT_println("evHandlerWifi started");
}

void evHandlerWifi::handle() {
  //Serial.print(".");
  MDNS.update();
  ArduinoOTA.handle();
  switch (Events.code) {

    case evInit:

      Serial.println("Push post init");
      Events.delayedPushSeconds(postInitDelay, evPostInit);

      break;

    case evPostInit:
      Serial.println("push OTA (post init)");
      Events.delayedPushSeconds(1, evOta, evxOn);

      break;
    case evWifi:
      {
        if (Events.ext == evxTimeZoneChanged) {
          DTV_println("evxTimeZoneChanged", Events.iParam);
          writeHisto("TimeZoneChanged", String(Events.iParam));
        }
        if (Events.ext == evxTimestampChanged) {
          DTV_println("evxTimestampChanged", Events.iParam);
          writeHisto("TimestampChanged", String(Events.iParam));
          //timeSyncrCnt=0;
        }
      }
      break;

    case ev1Hz:
      {

        // check for connection to local WiFi  1 fois par seconde c'est suffisant
        uint8_t aStatus = WiFi.status();
        //DV_println(aStatus);
        if (status != aStatus) {
          status = aStatus;
          DTV_println("WIFI:", status);
          //    WL_IDLE_STATUS      = 0,
          //    WL_NO_SSID_AVAIL    = 1,
          //    WL_SCAN_COMPLETED   = 2,
          //    WL_CONNECTED        = 3,
          //    WL_CONNECT_FAILED   = 4,
          //    WL_CONNECTION_LOST  = 5,
          //    WL_DISCONNECTED     = 6
          //    7: WL_AP_LISTENING
          //    8: WL_AP_CONNECTED

          if (connected != (status == WL_CONNECTED)) {
            connected = not connected;
            Events.push(evWifi, (connected) ? evxWifiOn : evxWifiOff, status);
            DTV_println("WIFI:", connected ? F("Connected") : F("lost"));
            // writeHisto(connected ? F("wifi Connected") : F("wifi lost"), WiFi.SSID());
          }
        }
        /*
        // save current time in RTC memory
        currentTime = now();
        savedRTCmemory.actualTimestamp = currentTime;  // save time in RTC memory
        saveRTCmemory();

        // If we are not connected we warn the user every 30 seconds that we need to update credential


        if (!WiFiConnected && second() % 30 == 15) {
          // every 30 sec
          Serial.print(F("module non connecté au Wifi local "));
          DV_println(WiFi.SSID());
          Serial.println(F("taper WIFI= pour configurer le Wifi"));
        }

        // au chagement de mois a partir 7H25 on envois le mail (un essais par heure)
        if (WiFiConnected && currentMonth != month() && hour() > 7 && minute() == 25 && second() == 0) {
          if (sendHistoTo(mailSendTo)) {
            if (currentMonth > 0) eraseHisto();
            currentMonth = month();
            writeHisto(F("Mail send ok"), mailSendTo);
          } else {
            writeHisto(F("Mail erreur"), mailSendTo);
          }
        }
        */
      }
      break;

    case evOta:
      {
        switch (Events.ext) {

          case evxOff:
            {



              DT_println("Stop OTA");
              //myUdp.broadcastInfo("Stop OTA");
              ArduinoOTA.end();
              writeHisto(F("Stop OTA"), hdlWifi.nodeName);
              // but restart MDNS
              MDNS.begin(hdlWifi.nodeName);
              //MDNS.addService("http", "tcp", 80);
            }
            break;

          case evxOn:
            {
              // start OTA
              ArduinoOTA.setHostname(hdlWifi.nodeName.c_str());
              ArduinoOTA.begin(true);                                        //MDNS is handled in main loop
              Events.delayedPushMillis(1000L * 15 * 60, evOta, evxWifiOff);  // stop OTA dans 15 Min

              ////MDNS.update();
              DTV_print("OTA on ", nodeName);
              DTV_println(" started.  SSID", WiFi.SSID());
              writeHisto(F("Start OTA"), hdlWifi.nodeName);

              //end start OTA
            }
        }
        //MDNS.update();
      }
      break;
  }
}
// warning about  use of timelib.h
//  now()
//     might call the getTimeFunction  unless a setSyncInterval(largetime) was done just before and largetime big enough to hold elapsed time of prevous call to now() !!!!
//     so never call now() in getTimeFunction or time sensitive job

//  setTime(time_t T)
//     assume that it is exactly 00.000  seconds so you might be little late if you got  time form an elapsed second
//     timeStatus()  is timeSet    and next syncTime is update

// timeStatus_t timeStatus()
//      !!! timeStatus()  call now() !!!!
//     so never call timeStatus() in getTimeFunction or time sensitive job  !!!!


//  adjustTIme(long adjustment)
//     just add adjustment to internal sysTime and nothing else
//     so it can be use to do a settime without changing timeStatus

//  setSyncProvider( getExternalTime getTimeFunction)
//     will call now() and force a call to getTimeFunction() !!!!
//     so never call setSyncProvider() in getTimeFunction or time sensitive job  !!!!

//  setSyncInterval(time_t interval)
//     just set nextSyncTime to  sysTime + interval and nothing else






// very tricky way to get an aproximate time from ESP harware at boot with timeStatus set to timeNeedsSync
// on first call it will get aproximate time from RTC memory of Flash and do an syncInterval(0) !!!
// on second call it will return 0 and do an syncInterval(300) so timeStatus is set to timeNeedsSync
// subsequents call will try to get time from  the web if ok syncInterval(1*3600) if not syncInterval(5*60)
time_t wifiTimeProvider() {

  time_t result = 0;

  //
  // recuperation de l'heure dans la static ram de l'ESP uniquement si time not set
  static uint8_t callCnt = 0;
  if (callCnt == 0) {

    hdlWifi.coldBoot = !getRTCMemory();  // si RTC memory est vide c'est un coldboot
    DV_println(hdlWifi.coldBoot);
    result = savedRTCmemory.actualTimestamp;
    DV_println(result);
    if (hdlWifi.coldBoot) {
      // si l'heure n'est pas presente
      // c'est un cold boot
      // on recupere l'heure du fichier Histo par defaut
      result = jobGetHistoTime() + 10;
      //savedRTCmemory.actualTimestamp = result;
      DTV_println("Got time from histo", niceDisplayTime(result, true));
    } else {
      DTV_println("Got time from RTC", niceDisplayTime(result, true));
    }
    setSyncInterval(0);
    callCnt = 1;
    return (result);
  }
  if (callCnt == 1) {
    setSyncInterval(5*60);
    callCnt = 2;
    setSyncInterval(5); //will try in 5 sec 
    return (0);  // to force time need syncr
  }
  if (callCnt == 2) {
    setSyncInterval(5*60);
    callCnt = 3;
  }
  result = getFullwwwTime();
  DTV_println("Got time from getWWWtime", niceDisplayTime(result, true));
  return (result);
}

// get full time with a standard web server
#include <ESP8266HTTPClient.h>
#define LOCATION "Europe/Paris"  // adjust to yout location
#define URI "worldtimeapi.org/api/timezone/"

time_t getFullwwwTime() {
  // connect to an internet API to
  // check WWW connection
  // get unix time
  // get timezone
  //http://worldtimeapi.org/api/timezone/Europe/Paris

  String urlServer = F("http://" URI LOCATION);
  DV_println(urlServer);
  WiFiClient aWiFI;  // Wificlient doit etre declaré avant HTTPClient
  HTTPClient aHttp;  //Declare an object of class HTTPClient
  aHttp.setTimeout(100);
  aHttp.begin(aWiFI, urlServer);  //Specify request destination

  int httpCode = aHttp.GET();  //Send the request
  if (httpCode != 200) {
    DTV_println(F("got an error in http.GET() "), httpCode);
    aHttp.end();  //Close connection
    return (0);
  }

  String answer = aHttp.getString();  //Get the request response payload
  //DV_print(helperFreeRam() + 1);
  aHttp.end();  //Close connection (restore 22K of ram)
  //DV_println(answer);  //Print the response payload
  //  23:02:48.615 -> answer => '{"abbreviation":"CET","client_ip":"82.66.229.100","datetime":"2024-03-15T23:02:48.596866+01:00","day_of_week":5,"day_of_year":75,"dst":false,"dst_from":null,"dst_offset":0,"dst_until":null,"raw_offset":3600,"timezone":"Europe/Paris","unixtime":1710540168,"utc_datetime":"2024-03-15T22:02:48.596866+00:00","utc_offset":"+01:00","week_number":11}'
  if (!answer.startsWith(F("{\"abbreviation\":"))) return (0);
  //DV_println(helperFreeRam());
  JSONVar aJson = JSON.parse(answer);
  //DV_println(helperFreeRam());
  int atimezone = (const double)aJson["raw_offset"];
  atimezone += (const double)aJson["dst_offset"];
  time_t unixtime = (const long)aJson["unixtime"] + atimezone;
  atimezone = atimezone / -3600;
  V_println(atimezone);

  V_println(unixtime);
  hdlWifi.clockDelta = unixtime - bNode.currentTime;
  if (hdlWifi.clockDelta) {
    Events.push(evWifi, evxTimestampChanged, hdlWifi.clockDelta);
  }
  if (bNode.timeZone != atimezone) {
    //hdlWifi .timeZone = atimezone;
    bNode.timeZone = atimezone;
    //hdlWifi.timeZoneChanged = true;
    Events.push(evWifi, evxTimeZoneChanged, atimezone);
  }

  DV_println(hdlWifi.clockDelta);
  hdlWifi.clockLastTry = unixtime;
  return (unixtime);
}
