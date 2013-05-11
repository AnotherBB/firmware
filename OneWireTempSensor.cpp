/*
 * OneWireTempSensor.cpp
 */ 
#include "brewpi_avr.h"
#include "OneWireTempSensor.h"
#include "DallasTemperature.h"
#include "OneWire.h"
#include "OneWireDevices.h"
#include "PiLink.h"
#include "Ticks.h"

/**
 * Initializes the temperature sensor.
 * This method is called when the sensor is first created and also any time the sensor reports it's disconnected.
 * If the result is DEVICE_DISCONNECTED then subsequent calls to read() will also return DEVICE_DISCONNECTED.
 * Clients should attempt to re-initialize the sensor by calling init() again. 
 */
fixed7_9 OneWireTempSensor::init(){

#if BREWPI_DEBUG	
	// save address and pinNr for debug messages
	char addressString[17];
	printBytes(sensorAddress, 8, addressString);
	uint8_t pinNr = oneWire->pinNr();
#endif	

	if (sensor==NULL) {
		sensor = new DallasTemperature(oneWire);
		if (sensor==NULL) {
			DEBUG_MSG(PSTR("Not enough SRAM for temp sensor %s"), addressString);
			setConnected(false);
			return DEVICE_DISCONNECTED;
		}
	}
	
	// get sensor address - todo this is deprecated and will be phased out. Needed to support revA shields
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A
	const uint8_t pinNr = oneWire->pinNr();
	if (!sensorAddress[0]) {
		if (!sensor->getAddress(sensorAddress, 0)) {
			// error no sensor found
			DEBUG_MSG(PSTR("No address for sensor on pin %d"), pinNr);
			if (connected) {
				// log only on transition from connected to disconnected.				
				piLink.debugMessage(PSTR("Unable to find address for sensor on pin %d"), pinNr);
			}
			setConnected(false);
			return DEVICE_DISCONNECTED;
		}
		else {
#if BREWPI_DEBUG
			printBytes(sensorAddress, 8, addressString);
#endif			
		}
	}
#endif	
	
	DEBUG_MSG(PSTR("fetching initial temperature %d %s"), pinNr, addressString);

	// This quickly tests if the sensor is connected. Suring the main TempControl loop, we don't want to spend many seconds
	// scanning each sensor since this brings things to a halt.
	if (!sensor->isConnected(sensorAddress)) {
		setConnected(false);
		return DEVICE_DISCONNECTED;		
	}
		
	sensor->setResolution(sensorAddress, 12);
	sensor->setWaitForConversion(false);
		
	// read initial temperature twice - first read is inaccurate
	fixed7_9 temperature;
	for (int i=0; i<2; i++) {
		temperature = DEVICE_DISCONNECTED;
		lastRequestTime = ticks.millis();
		while(temperature == DEVICE_DISCONNECTED){
			sensor->requestTemperatures();
			wait.millis(750); // delay 750ms for conversion time		
			temperature = sensor->getTempRaw(sensorAddress);
			DEBUG_MSG(PSTR("Sensor initial temp read: pin %d %s %d"), pinNr, addressString, temperature);
			if((ticks.millis() - lastRequestTime) > 4000) {
				setConnected(false);
				DEBUG_MSG(PSTR("Reporting device disconnected pin %d %s"), pinNr, addressString);
				return DEVICE_DISCONNECTED;
			}
		}
	}
	DEBUG_MSG(PSTR("Sensor final read: pin %d %s %d"), pinNr, addressString, temperature);
	temperature = constrain(temperature+calibrationOffset, ((int) INT_MIN)>>5, ((int) INT_MAX)>>5)<<5; // sensor returns 12 bits with 4 fraction bits. Store with 9 fraction bits		
	DEBUG_MSG(PSTR("Sensor initialized: pin %d %s %d"), pinNr, addressString, temperature);	
	setConnected(true);
	return temperature;
}

void OneWireTempSensor::setConnected(bool connected) {
	if (this->connected==connected)
		return;
		
	char addressString[17];
	printBytes(sensorAddress, 8, addressString);
	this->connected = connected;	
	piLink.debugMessage(PSTR("temp sensor %sconnected %s"), connected ? "" : "dis", addressString);
}

fixed7_9 OneWireTempSensor::read(){
	
	if (!connected)
		return DEVICE_DISCONNECTED;
	
	if((ticks.millis()-lastRequestTime) > 5000){ // if last request is longer than 5 seconds ago, request again and delay
		sensor->requestTemperatures();
		lastRequestTime = ticks.millis();
		wait.millis(750); // wait 750 ms (18B20 max conversion time)
	}
	fixed7_9 temperature = sensor->getTempRaw(sensorAddress);
	if(temperature == DEVICE_DISCONNECTED){
		setConnected(false);
		return DEVICE_DISCONNECTED;
	}
	temperature = constrain(temperature+calibrationOffset, ((int) INT_MIN)>>5, ((int) INT_MAX)>>5)<<5; // sensor returns 12 bits with 4 fraction bits. Store with 9 fraction bits

	// already send request for next read
	sensor->requestTemperatures();
	lastRequestTime = ticks.millis();
	return temperature;
}
