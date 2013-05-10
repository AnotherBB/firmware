/*
 * DeviceManager.h
 *
 * Created: 21/04/2013 23:53:36
 *  Author: mat
 */ 


#pragma once

#include "brewpi_avr.h"

#include "Actuator.h"
#include "Sensor.h"
#include "TempSensor.h"
#include "OneWireDevices.h"
#include "pins.h"

/**
 * A user has freedom to connect various devices to the arduino, either via extending the oneWire bus, or by assigning to specific pins, e.g. actuators, switch sensors.
 * Rather than make this compile-time, the configuration is stored at runtime. 
 * Also, the availability of various sensors will change. E.g. it's possible to have a fridge constant mode without a beer sensor.
 *
 * Since the data has to be persisted to EEPROM, references to the actual uses of the devices have to be encoded.  This is the function of the deviceID.
 */

class DeviceConfig;

typedef int8_t device_slot_t;
inline bool isDefinedSlot(device_slot_t s) { return s>=0; }
const device_slot_t MAX_DEVICE_SLOT = 40;
const device_slot_t INVALID_SLOT = -1;

/*
 * Describes the logical function of each device. 
 */
enum DeviceFunction { 
	DEVICE_NONE = 0,														// used as a sentry to mark end of list
	// chamber devices
	DEVICE_CHAMBER_DOOR=1,													// switch sensor
	DEVICE_CHAMBER_HEAT=2, DEVICE_CHAMBER_COOL=3, DEVICE_CHAMBER_LIGHT=4,	// actuator	
	DEVICE_CHAMBER_TEMP=5, DEVICE_CHAMBER_ROOM_TEMP=6,						// temp sensors
	DEVICE_CHAMBER_FAN=7,						// a fan in the chamber
	DEVICE_CHAMBER_RESERVED1=8,					// reserved for future use	
	// carboy devices
	DEVICE_BEER_FIRST=9,
	DEVICE_BEER_TEMP=DEVICE_BEER_FIRST,									// primary beer temp sensor
	DEVICE_BEER_TEMP2=10,								// secondary beer temp sensor 
	DEVICE_BEER_HEAT=11, DEVICE_BEER_COOL=12,				// individual actuators
	DEVICE_BEER_SG=13,									// SG sensor
	DEVICE_BEER_RESERVED1=14, DEVICE_BEER_RESERVED2=15,	// reserved	
	DEVICE_MAX=16
};

/**
 * Describes where the device is most closely associated.
 */
enum DeviceOwner {
	DEVICE_OWNER_NONE=0,	
	DEVICE_OWNER_CHAMBER=1,
	DEVICE_OWNER_BEER=2
};

enum DeviceType {
	DEVICETYPE_NONE=0,
	DEVICETYPE_TEMP_SENSOR=1,		/* BasicTempSensor - OneWire */
	DEVICETYPE_SWITCH_SENSOR=2,		/* SwitchSensor - direct pin */
	DEVICETYPE_SWITCH_ACTUATOR=3	/* Actuator - both direct pin and onewire are supported */	
};

/*
 * The concrete type of the device. 
 */
enum DeviceHardware { 
	DEVICE_HARDWARE_NONE=0,
	DEVICE_HARDWARE_PIN=1,			// a digital pin, either input or output
	DEVICE_HARDWARE_ONEWIRE_TEMP=2,	// a onewire temperature sensor
#if BREWPI_DS2413
	DEVICE_HARDWARE_ONEWIRE_2413=3	// a onewire 2-channel PIO input or output.
#endif	
};

inline bool isAssignable(DeviceType type, DeviceHardware hardware)
{
	return (hardware==DEVICE_HARDWARE_PIN && (type==DEVICETYPE_SWITCH_ACTUATOR || type==DEVICETYPE_SWITCH_SENSOR))
#if BREWPI_DS2413
	|| (hardware==DEVICE_HARDWARE_ONEWIRE_2413 && type==DEVICETYPE_SWITCH_ACTUATOR)
#endif	
	|| (hardware==DEVICE_HARDWARE_ONEWIRE_TEMP && type==DEVICETYPE_TEMP_SENSOR)
	|| (hardware==DEVICE_HARDWARE_NONE && type==DEVICETYPE_NONE);
}

inline bool isOneWire(DeviceHardware hardware) {	
	return 
#if BREWPI_DS2413
	hardware==DEVICE_HARDWARE_ONEWIRE_2413 || 
#endif	
	hardware==DEVICE_HARDWARE_ONEWIRE_TEMP;
}

inline bool isDigitalPin(DeviceHardware hardware) {
	return hardware==DEVICE_HARDWARE_PIN;
}

/**
 * Determines the class of device for the given DeviceID.
 */
inline DeviceType deviceType(DeviceFunction id) {
	switch (id) {
	case DEVICE_CHAMBER_DOOR:
		return DEVICETYPE_SWITCH_SENSOR;
		
	case DEVICE_CHAMBER_HEAT:
	case DEVICE_CHAMBER_COOL:
	case DEVICE_CHAMBER_LIGHT:
	case DEVICE_CHAMBER_FAN:
	case DEVICE_BEER_HEAT:
	case DEVICE_BEER_COOL:
		return DEVICETYPE_SWITCH_ACTUATOR;
		
	case DEVICE_CHAMBER_TEMP:
	case DEVICE_CHAMBER_ROOM_TEMP:
	case DEVICE_BEER_TEMP:
	case DEVICE_BEER_TEMP2:
		return DEVICETYPE_TEMP_SENSOR;
		
	default: 
		return DEVICETYPE_NONE;
	}
}	

/**
 * Determines where this devices belongs.
 */
inline DeviceOwner deviceOwner(DeviceFunction id) {
	return id==0 ? DEVICE_OWNER_NONE : id>=DEVICE_BEER_FIRST ? DEVICE_OWNER_BEER : DEVICE_OWNER_CHAMBER;
}	

		
/*
 * A union of all device types.
 */	
struct DeviceConfig {
	
	uint8_t chamber;			// 0 means no chamber. 1 is the first chamber.	
	uint8_t beer;				// 0 means no beer, 1 is the first beer
	
	DeviceFunction deviceFunction;			// The function of the device to configure
											// this used to be a union, but now the fields are simply laid out bare
	DeviceHardware deviceHardware;			// flag to indicate the runtime type of device
	struct Hardware {
		uint8_t pinNr;							// the arduino pin nr this device is connected to
		bool invert;							// for actuators/sensors, controls if the signal value is inverted.
		DeviceAddress address;					// for onewire devices, if address[0]==0 then use the first matching device type, otherwise use the device with the specific address
		uint8_t pio;							// pio address
	} hw;
};

/**
 * Provides a single alternative value for a given definition point in a device.
 */
struct DeviceAlternatives {
	enum AlternativeType { 		
		DA_PIN, DA_ADDRESS, DA_PIO, DA_INVERT, DA_BOOLVALUE 
	};
	AlternativeType type;
	union {
		uint8_t pinNr;					// type == DA_PIN
		uint8_t pio;					// type == DA_PIO
		DeviceAddress address;			// type == DA_ADDRESS
		bool invert;					// type == DA_INVERT
		bool boolValue;					// type == DA_BOOLVALUE
	};
	
};


typedef void (*EnumDevicesCallback)(DeviceConfig*, void* pv);
class EnumerateHardware;

struct DeviceOutput
{
	device_slot_t	slot;
	char value[10];
	Print* pp;
};

struct DeviceDisplay {
	int8_t id;		// -1 for all devices, >=0 for specific device	
	int8_t value;	// set value
	int8_t write;	// write value		
	int8_t empty;	// show unused devices when id==-1, default is 0
};

void HandleDeviceDisplay(const char* key, const char* value, void* pv);

/**
 * Reads or writes a value to a device.
 */
void UpdateDeviceState(DeviceDisplay& dd, DeviceConfig& dc, char* val);

class DeviceManager
{
public:
	
	int8_t enumerateActuatorPins(uint8_t offset)
	{
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A
		switch (offset) {
			case 0: return heatingPin;
			case 1: return coolingPin;
			default:
				return -1;
		}
#endif	

#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_C
		switch (offset) {
			case 0: return actuatorPin1;
			case 1: return actuatorPin2;
			case 2: return actuatorPin3;
			case 3: return actuatorPin4;
			default: return -1;
		}
#endif			
		return -1;
	}

	int8_t enumerateSensorPins(uint8_t offset) {
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A || BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_C
		if (offset==0)
			return doorPin;
#endif			
		return -1;
	}
	
	/* Enumerates the 1-wire pins.
	 * 
	 */
	int8_t enumOneWirePins(uint8_t offset)
	{		
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A
		if (offset==0)
			return beerSensorPin;
		if (offset==1)
			return fridgeSensorPin;
#endif
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_C
		if (offset==0)
			return oneWirePin;
#endif
		return -1;								
	}

	static void setupUnconfiguredDevices();
	
	/*
	 * Determines if the given device config is complete. 
	 */
	static bool firstUndefinedAlternative(DeviceConfig& config, DeviceAlternatives& alternatives);
	
	
	/**
	 * Creates and Installs a device from the given device config.
	 * /return true if a device was installed. false if the config is not complete.
	 */
	static void installDevice(DeviceConfig& config);
	
	static void uninstallDevice(DeviceConfig& config);
	
	static void parseDeviceDefinition(Stream& p);
	static void printDevice(device_slot_t slot, DeviceConfig& config, const char* value, Print& p);
		
	/**
	 * Iterate over the defined devices.
	 * Caller first calls with deviceIndex 0. If the return value is true, config is filled out with the 
	 * config for the device. The caller can then increment deviceIndex and try again.
	 */
	static bool allDevices(DeviceConfig& config, uint8_t deviceIndex);

	static bool isDeviceValid(DeviceConfig& config, DeviceConfig& original, uint8_t deviceIndex);

	/**
	 * read hardware spec from stream and output matching devices
	 */
	static void enumerateHardware(Stream& p);
	
	static bool enumDevice(DeviceDisplay& dd, DeviceConfig& dc, uint8_t idx);

	static void listDevices(Stream& p);
	
private:
	
	static void enumerateOneWireDevices(EnumerateHardware& h, EnumDevicesCallback f, DeviceOutput& output);	
	static void enumeratePinDevices(EnumerateHardware& h, EnumDevicesCallback callback, DeviceOutput& output);
	static void OutputEnumeratedDevices(DeviceConfig* config, void* pv);
	static void handleEnumeratedDevice(DeviceConfig& config, EnumerateHardware& h, EnumDevicesCallback callback, DeviceOutput& out);
	static void readTempSensorValue(DeviceConfig::Hardware hw, char* out);
	
	static OneWire* oneWireBus(uint8_t pin);
	static void* createDevice(DeviceConfig& config, DeviceType dc);
	
	static void beginDeviceOutput() { firstDeviceOutput = true; }
	
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_A	
	static OneWire beerSensorBus;
	static OneWire fridgeSensorBus;	
#endif	
#if BREWPI_STATIC_CONFIG==BREWPI_SHIELD_REV_C
	static OneWire primaryOneWireBus;	
#endif
	static bool firstDeviceOutput;
};


extern DeviceManager deviceManager;
