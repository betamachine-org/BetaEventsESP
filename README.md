 ***********************************************
    Sketch betaEventsESP.ino   validation of lib betaEvents  to deal nicely with events programing with Arduino
    Copyright 2020 Pierre HENRY net23@frdev.com All - right reserved.


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

  betaEventsESP is a an exemple of stand alone code to understand Betaevents concept
  this version is specifty crafted for 32 bit processor as ESP8266

  actual Sources : https://github.com/net234/BetaEventsESP

  it use lib TimeLib.h  and no specific Hardware except ESP8266

  BetaEvents Object :

  event         structure minimaliste pour gerer un evenement
  evManager     class du noyau evenementiel permetant de gerer les evenements  une seule instance nomée "Events"
  evHandler     class permetant de gerer un interface specifique (pousoir led relai ...) de maniere evenementiel


  Internal Objets
  evHandlerList class permetant de gerer une liste d'evenement


  fichier :
  BetaEventESP.h           modele d'utilisation basic des events (a utiliser pour debuter)
  EventsManagerESP.h/cpp   code du noyau
  evHandlerESP.h/cpp       code de divers handler de base (input output poussoir led relais clavier debug)


History :
Orinal build PH_Event V1.0 P.HENRY  24/02/2020  https://github.com/net234/PH_Events
Various version for Arduino 8Bits  under BetaEvents   https://github.com/net234/BetaEvents
Various version for Arduino 32Bits  under BetaEvents32   https://github.com/net234/BetaEvents32
Full rebuild    for ESP8266    https://github.com/net234/BetaEventsESP   (betaEvents V4.0) 25/04/2024
