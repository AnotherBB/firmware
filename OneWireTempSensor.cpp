/*
 * OneWireTempSensor.cpp
 */ 
#include "Brewpi.h"
#include "OneWireTempSensor.h"
#include "DallasTemperature.h"
#include "OneWire.h"
#include "OneWireDevices.h"
#include "PiLink.h"
#include "Ticks.h"

OneWireTempSensor::~OneWireTempSensor(){
	delete sensor;
};

/**
 * Initializes the temperature sensor.
 * This method is called when the sensor is first created and also any time the sensor reports it's disconnected.
 * If the result is DEVICE_DISCONNECTED then subsequent calls to read() will also return DEVICE_DISCONNECTED.
 * Clients should attempt to re-initialize the sensor by calling init() again. 
 */
fixed7_9 OneWireTempSensor::init(){

	// save address and pinNr for debug messages
	char addressString[17];
	printBytes(sensorAddress, 8, addressString);

	uint8_t pinNr = oneWire->pinNr();

	if (sensor==NULL) {
		sensor = new DallasTemperature(oneWire);
		if (sensor==NULL) {
			logErrorString(ERROR_SRAM_SENSOR, addressString);
			setConnected(false);
			return DEVICE_DISCONNECTED;
		}
	}
	
	// get sensor address - todo this is deprecated and will be phased out. Needed to support revA shields
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A	
	if (!sensorAddress[0]) {
		if (!sensor->getAddress(sensorAddress, 0)) {
			// error no sensor found
			logErrorInt(ERROR_SENSOR_NO_ADDRESS_ON_PIN, pinNr);
			setConnected(false);
			return DEVICE_DISCONNECTED;
		}
		else {
			#if (BREWPI_DEBUG > 0)
			printBytes(sensorAddress, 8, addressString);
			#endif	
		}
	}
#endif

	// This quickly tests if the sensor is connected. Suring the main TempControl loop, we don't want to spend many seconds
	// scanning each sensor since this brings things to a halt.
	if (!sensor->isConnected(sensorAddress)) {
		setConnected(false);
		return DEVICE_DISCONNECTED;		
	}
		
	logDeveloper(PSTR("Fetching initial temperature of sensor %s"), addressString);
	
	sensor->setResolution(sensorAddress, 12);
	sensor->setWaitForConversion(false);
		
	// read initial temperature twice - first read is inaccurate
	fixed7_9 temperature;
	for (int i=0; i<2; i++) {
		temperature = DEVICE_DISCONNECTED;
		lastRequestTime = ticks.seconds();
		while(temperature == DEVICE_DISCONNECTED){
			sensor->requestTemperatures();
			waitForConversion();
			temperature = sensor->getTempRaw(sensorAddress);
			logDeveloper(PSTR("Sensor initial temp read: pin %d %s %d"), this->oneWire->pinNr(), addressString, temperature);
			if(ticks.timeSince(lastRequestTime) > 4) {
				setConnected(false);
				return DEVICE_DISCONNECTED;
			}
		}
	}
	temperature = constrainTemp(temperature+calibrationOffset, ((int) INT_MIN)>>5, ((int) INT_MAX)>>5)<<5; // sensor returns 12 bits with 4 fraction bits. Store with 9 fraction bits		
	logInfoIntStringTemp(INFO_TEMP_SENSOR_INITIALIZED, pinNr, addressString, temperature);
	
	setConnected(true);
	return temperature;
}

void OneWireTempSensor::waitForConversion()
{
	wait.millis(750);
}

void OneWireTempSensor::setConnected(bool connected) {
	if (this->connected==connected)
		return; // state is stays the same
		
	char addressString[17];
	printBytes(sensorAddress, 8, addressString);
	this->connected = connected;
	if(connected){
		logInfoIntString(INFO_TEMP_SENSOR_CONNECTED, this->oneWire->pinNr(), addressString);
	}
	else{
		logWarningIntString(WARNING_TEMP_SENSOR_DISCONNECTED, this->oneWire->pinNr(), addressString);
	}
}

fixed7_9 OneWireTempSensor::read(){
	if (!connected)
		return DEVICE_DISCONNECTED;
	
	if(ticks.timeSince(lastRequestTime) > 5){ // if last request is longer than 5 seconds ago, request again and delay
		sensor->requestTemperatures();
		lastRequestTime = ticks.seconds();
		waitForConversion();
	}
	fixed7_9 temperature = sensor->getTempRaw(sensorAddress);
	if(temperature == DEVICE_DISCONNECTED){
		setConnected(false);
		return DEVICE_DISCONNECTED;
	}
	temperature = constrainTemp(temperature+calibrationOffset, ((int) INT_MIN)>>5, ((int) INT_MAX)>>5)<<5; // sensor returns 12 bits with 4 fraction bits. Store with 9 fraction bits

	// already send request for next read
	sensor->requestTemperatures();
	lastRequestTime = ticks.seconds();
	return temperature;
}
