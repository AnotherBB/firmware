/*
 * ExternalTempSensor.h
 *
 * Created: 29/03/2013 04:46:34
 *  Author: mat
 */ 

#pragma once

#include "Brewpi.h"
#include "TempSensor.h"

/**
 * A temp sensor whose value is not read from the device, but set in code.
 * This is used by the simulator.
 */
class ExternalTempSensor : public BasicTempSensor
{
	public:
	ExternalTempSensor(bool connected=false) : _temperature(0), _connected(connected) { }

	void setConnected(bool connected)
	{
		_connected = connected;
	}

	bool isConnected() { return _connected; }

	fixed7_9 init() {
		return read();
	}
	
	fixed7_9 read() {
		if (!isConnected())
			return DEVICE_DISCONNECTED;				
		return _temperature;
	}
	
	void setValue(fixed7_9 newTemp) {
		_temperature = newTemp;		
	}
	
	private:
	fixed7_9 _temperature;
	bool _connected;
};
