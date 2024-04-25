#pragma once


//This file just create a basic events set for demo
//most of time you will not need it
//just insert EventsManagerESP.h  and create instances of all evHandlers you need
//see basics evHandler in evHandlers.H

//EventManager32 uses ESP8266 kernel 3.0
static_assert(sizeof(time_t) == 8, "This version works with time_t 32bit, please move to ESP8266 kernel 3.0");

// #define D3  0     //!FLASH    BP0
// #define D4  2     //!LED2     LED0


#include "EventsManagerESP.h"
// Events Manager build an instance called "Events" who care about events

//if SERIAL is needed it should be declare first othewise others evHandlers cant print during begin
evHandlerSerial Keyboard;  //Only one instance  Must be named KeyBoard

// instance debugger  (need evHandlerSerial)
evHandlerDebug  Debug;   // display differents debug times lines if you send a "T"<CR> line command


// instance pushButton on D3 (FLASH for ESP8266) named BP0_PIN

// definition GPIO on D3 named BP0 if not already defined
#ifndef BP0_PIN
#define BP0_PIN D3 // (flash) 
#endif

evHandlerButton BP0(evBP0, BP0_PIN);


// instance LED on D4 

//definition GPIO on D4 named Led0 if not already defined
#ifndef LED0_PIN
#define LED0_PIN D4  // led builtin gpio 2
#endif


// setup Led0  blinking 1HZ   
evHandlerLed    Led0(evLed0, LED0_PIN, false , 1);

//Done