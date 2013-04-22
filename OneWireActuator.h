/*
 * OneWireActuator.h
 *
 * Created: 21/04/2013 02:43:11
 *  Author: mat
 * 
 * Implements an actuator using the one-wire 2413 device. (2450 may be added later.)
 */ 


#pragma once

#include "Actuator.h"
#include "ds2413.h"

/**
 * An actuator that operates by communicating with a DS2413 device.
 */
class OneWireActuator : public Actuator
{
public:	
	void init(OneWire* bus, DeviceAddress address, pio_t pio, bool invert=true) {
		this->invert = invert;
		
		device.init(bus, address);
	}
	
	void setActive(bool active) {
		device.channelWrite(pio, active^invert);
	}		
			
private:
	DS2413 device;
	pio_t pio;
	bool invert;
};

