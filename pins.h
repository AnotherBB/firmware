/*
 * Copyright 2012 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
 * 
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PINS_H_
#define PINS_H_

#include "brewpi_avr.h"

// Most pins are only conditionaly defined here, allowing definitions to be provided in config_local.h for 
// local overrides

#define BREWPI_SHIELD_REV_A	1
#define BREWPI_SHIELD_REV_C	2


#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A
#ifndef beerSensorPin
#define beerSensorPin    A5 // OneWire 1
#endif

#ifndef fridgeSensorPin
#define fridgeSensorPin  A4 // OneWire 2
#endif

#ifndef coolingPin
#define coolingPin	6
#endif

#ifndef heatingPin
#define heatingPin	5
#endif

#endif

#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_C

#ifndef oneWirePin
#define oneWirePin A4
#endif

#ifndef actuatorPin1
#define actuatorPin1 2
#endif

#ifndef actuatorPin2
#define actuatorPin2 5
#endif

#ifndef actuatorPin3
#define actuatorPin3 6
#endif

#ifndef actuatorPin4
#define actuatorPin4 A5
#endif

#endif

#define doorPin		4
#define alarmPin	3
#define lcdLatchPin 10

// If you change the interrupt pins, you will also have to review the interrupt vectors of the rotary encoder
#define rotarySwitchPin 7 // INT6
#define rotaryAPin 8 // PCINT4
#define rotaryBPin 9 // PCINT5

// You can use the internal pull-up resistors instead of external ones for the doorPin and the rotary encoder pins
#ifndef USE_INTERNAL_PULL_UP_RESISTORS
#define USE_INTERNAL_PULL_UP_RESISTORS false
#endif

#ifndef LIGHT_AS_HEATER
// If you are using the light bulb of your fridge as a heater, set this define to true
// It will turn on the light when the door is opened
#define LIGHT_AS_HEATER true
#endif



#endif /* PINS_H_ */