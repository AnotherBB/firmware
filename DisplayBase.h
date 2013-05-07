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

#pragma once

#include "brewpi_avr.h"
#include "temperatureFormats.h"

/* Set to 1 to enable virtual functions and polymorphic display. */
#define DISPLAY_POLYMORPHIC 0
#if DISPLAY_POLYMORPHIC 
	#define DISPLAY_METHOD virtual
	#define DISPLAY_METHOD_PURE_VIRTUAL =0
	#define DISPLAY_FIELD
	#define DISPLAY_REF &
	#define DISPLAY_SUPERCLASS : public Display
#else
	#define DISPLAY_METHOD 
	#define DISPLAY_FIELD 
	#define DISPLAY_METHOD_PURE_VIRTUAL {}
	#define DISPLAY_REF
	#define DISPLAY_SUPERCLASS 
#endif

#if DISPLAY_POLYMORPHIC
class Display{
	public:
	Display(){};	
	DISPLAY_METHOD ~Display();
		
	// initializes the lcd display
	DISPLAY_METHOD void init(void) DISPLAY_METHOD_PURE_VIRTUAL;
	
	DISPLAY_METHOD void printAll() DISPLAY_METHOD_PURE_VIRTUAL;
			
	// print all temperatures on the LCD
	DISPLAY_METHOD void printAllTemperatures(void) DISPLAY_METHOD_PURE_VIRTUAL;
	
	// print a temperature
	DISPLAY_METHOD void printTemperature(fixed7_9 temp) DISPLAY_METHOD_PURE_VIRTUAL;
	
	// print " --.-"
	void printUndefinedTemperature(void);

	// print the stationary text on the lcd.
	DISPLAY_METHOD void printStationaryText(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print degree sign + C/F
	DISPLAY_METHOD void printDegreeUnit(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print mode on the right location on the first line, after Mode:
	DISPLAY_METHOD void printMode(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print beer temperature at the right place on the display
	DISPLAY_METHOD void printBeerTemp(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print beer temperature setting at the right place on the display
	DISPLAY_METHOD void printBeerSet(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print fridge temperature at the right place on the display
	DISPLAY_METHOD void printFridgeTemp(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print fridge temperature setting at the right place on the display
	DISPLAY_METHOD void printFridgeSet(void) DISPLAY_METHOD_PURE_VIRTUAL;

	// print the current state on the last line of the LCD
	DISPLAY_METHOD void printState(void) DISPLAY_METHOD_PURE_VIRTUAL;

	DISPLAY_METHOD void printAt(int x, int y, const char* text) DISPLAY_METHOD_PURE_VIRTUAL;
	
	DISPLAY_METHOD void getLine(uint8_t lineNumber, char * buffer) DISPLAY_METHOD_PURE_VIRTUAL;
	
	/*
	 * When true, print content is not sent to the lcd panel, but only buffered.                                                                      
	 */
	DISPLAY_METHOD void setBufferOnly(bool bufferOnly) DISPLAY_METHOD_PURE_VIRTUAL;
	
	DISPLAY_METHOD void resetBacklightTimer() DISPLAY_METHOD_PURE_VIRTUAL;
	
	DISPLAY_METHOD void updateBacklight() DISPLAY_METHOD_PURE_VIRTUAL;
	
};
#endif


class NullDisplay DISPLAY_SUPERCLASS
{
public:	
	// initializes the lcd display
	DISPLAY_METHOD void init(void){}
	
	DISPLAY_METHOD void printAll() {}
	
	// print all temperatures on the LCD
	DISPLAY_METHOD void printAllTemperatures(void){}
	
	// print a temperature
	DISPLAY_METHOD void printTemperature(fixed7_9 temp){}

	// print the stationary text on the lcd.
	DISPLAY_METHOD void printStationaryText(void){}

	// print degree sign + C/F
	DISPLAY_METHOD void printDegreeUnit(void){}

	// print mode on the right location on the first line, after Mode:
	DISPLAY_METHOD void printMode(void){}

	// print beer temperature at the right place on the display
	DISPLAY_METHOD void printBeerTemp(void){}

	// print beer temperature setting at the right place on the display
	DISPLAY_METHOD void printBeerSet(void){}

	// print fridge temperature at the right place on the display
	DISPLAY_METHOD void printFridgeTemp(void){}

	// print fridge temperature setting at the right place on the display
	DISPLAY_METHOD void printFridgeSet(void){}

	// print the current state on the last line of the LCD
	DISPLAY_METHOD void printState(void){}

	DISPLAY_METHOD void printAt(int x, int y, const char* text) {}
		
	DISPLAY_METHOD void getLine(uint8_t lineNumber, char * buffer) {
		for (int i=0; i<20; i++)
			buffer[i] = ' ';
		buffer[20] = 0;
	}
		
	DISPLAY_METHOD void setBufferOnly(bool bufferOnly) { }
		
	DISPLAY_METHOD void resetBacklightTimer() { }
		
	DISPLAY_METHOD void updateBacklight() { }
};
		
	
	