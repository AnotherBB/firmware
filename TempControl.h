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


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "TempSensor.h"
#include "pins.h"
#include "temperatureFormats.h"

// These two structs are stored in and loaded from EEPROM
struct ControlSettings{
	char mode;
	fixed7_9 beerSetting;
	fixed7_9 fridgeSetting;
	fixed7_9 heatEstimator; // updated automatically by self learning algorithm
	fixed7_9 coolEstimator; // updated automatically by self learning algorithm
};

struct ControlVariables{
	fixed7_9 beerDiff;
	fixed23_9 diffIntegral; // also uses 9 fraction bits, but more integer bits to prevent overflow
	fixed7_9 beerSlope;
	fixed23_9 p;
	fixed23_9 i;
	fixed23_9 d;
	fixed7_9 Kp;
	fixed7_9 Kd;
	fixed7_9 estimatedPeak;
	fixed7_9 negPeakSetting;
	fixed7_9 posPeakSetting;
	fixed7_9 negPeak;
	fixed7_9 posPeak;
};

struct ControlConstants{
	char tempFormat;
	fixed7_9 tempSettingMin;
	fixed7_9 tempSettingMax;	
	fixed7_9 KpHeat;
	fixed7_9 KpCool;
	fixed7_9 Ki;
	fixed7_9 KdCool;
	fixed7_9 KdHeat;
	fixed7_9 iMaxSlope;
	fixed7_9 iMinSlope;
	fixed7_9 iMaxError;
	fixed7_9 idleRangeHigh;
	fixed7_9 idleRangeLow;
	fixed7_9 heatingTargetUpper;
	fixed7_9 heatingTargetLower;
	fixed7_9 coolingTargetUpper;
	fixed7_9 coolingTargetLower;
	uint16_t maxHeatTimeForEstimate; // max time for heat estimate in seconds
	uint16_t maxCoolTimeForEstimate; // max time for heat estimate in seconds
	// filter coefficients are stored as an uint16_t: (a<<8+b)
	uint16_t fridgeFastFilter;	// for display, logging and on-off control
	uint16_t fridgeSlowFilter;	// for peak detection
	uint16_t fridgeSlopeFilter;	// not used in current control algorithm
	uint16_t beerFastFilter;	// for display and logging
	uint16_t beerSlowFilter;	// for on/off control algorithm
	uint16_t beerSlopeFilter;	// for PID calculation
};

#define COOLING_TARGET ((cc.coolingTargetUpper+cc.coolingTargetLower)/2)
#define HEATING_TARGET ((cc.heatingTargetUpper+cc.heatingTargetLower)/2)

#define EEPROM_IS_INITIALIZED_ADDRESS 0
#define EEPROM_CONTROL_SETTINGS_ADDRESS (EEPROM_IS_INITIALIZED_ADDRESS+sizeof(uint8_t))
#define EEPROM_CONTROL_CONSTANTS_ADDRESS (EEPROM_CONTROL_SETTINGS_ADDRESS+sizeof(ControlSettings))

#define	MODE_FRIDGE_CONSTANT 'f'
#define MODE_BEER_CONSTANT 'b'
#define MODE_BEER_PROFILE 'p'
#define MODE_OFF 'o'

enum states{
	COOLING,
	HEATING,
	IDLE,
	STARTUP,
	DOOR_OPEN,
	STATE_OFF
};

// Making all functions and variables static reduces code size.
// There will only be one TempControl object, so it makes sense that they are static.

class TempControl{
	public:
	
	TempControl(){
	};
	~TempControl(){
	};
	
	static void init(void);
	static void reset(void);
	
	static void updateTemperatures(void);
	static void updatePID(void);
	static void updateState(void);
	static void updateOutputs(void);
	static void detectPeaks(void);
	
	static void loadSettings(void);
	static void storeSettings(void);
	static void loadDefaultSettings(void);
	
	static void loadConstants(void);
	static void storeConstants(void);
	static void loadDefaultConstants(void);
	
	static void loadSettingsAndConstants(void);
	
	static unsigned long timeSinceCooling(void);
 	static unsigned long timeSinceHeating(void);
  	static unsigned long timeSinceIdle(void);
	  
	static fixed7_9 getBeerTemp(void);
	static fixed7_9 getBeerSetting(void);
	static void setBeerTemp(int newTemp);
	
	static fixed7_9 getFridgeTemp(void);
	static fixed7_9 getFridgeSetting(void);
	static void setFridgeTemp(int newTemp);
		
	static void setMode(char newMode);
	static char getMode(void);
	static void setState(uint8_t newState);
	static uint8_t getState(void);
		
	public:
	static TempSensor beerSensor;
	static TempSensor fridgeSensor;
	
	// Control parameters
	static ControlConstants cc;
	static ControlSettings cs;
	static ControlVariables cv;
		
	private:
	// keep track of beer setting stored in EEPROM
	static fixed7_9 storedBeerSetting;

	// Timers
	static unsigned long lastIdleTime;
	static unsigned long lastHeatTime;
	static unsigned long lastCoolTime;
	
	// State variables
	static uint8_t state;
	static bool doPosPeakDetect;
	static bool doNegPeakDetect;

	
	static void increaseEstimator(fixed7_9 * estimator, fixed7_9 error);
	static void decreaseEstimator(fixed7_9 * estimator, fixed7_9 error);
};

extern TempControl tempControl;

#endif /* CONTROLLER_H_ */