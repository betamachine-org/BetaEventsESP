


// Version evenementielle de l'exemple de la lib standard OneDrive
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// https://github.com/milesburton/Arduino-Temperature-Control-Library
// Attention une oullup de 4,7K est necessaire (sauf sur D4 la led fait office de pull up)  
#pragma once
#include <Arduino.h>


#define MAXDS18x20 250  // no specific limit (254 max) in this vestion but about 1 sec to get one
// technicaly 30 should be a limit



enum tevxDs : uint8_t { evxDsStart,
                        evxDsSearch,
                        evxDsRead,
                        evxDsLast,
                        evxDsError };

#include <EventsManagerESP.h>
#include <OneWire.h>

class evHandlerDS18b20 : private evHandler, OneWire {
public:
  evHandlerDS18b20(const uint8_t evCode, const uint8_t aPinNumber, const uint16_t aDelai = 10L*1000)
    : OneWire(aPinNumber), evCode(evCode), pinNumber(aPinNumber), delai(aDelai){};  //
  virtual void begin() override;
  virtual void handle() override;
  float getTemperature() {
    return (float)raw / 16.0;
    ;
  }
  
  uint8_t getNumberOfDevices();
  uint8_t current;
  uint8_t error;

private:

  uint8_t evCode;
  uint8_t pinNumber;
  uint32_t delai;
  uint8_t addr[8];
  uint8_t type_s;
  int16_t raw;  // value
};
