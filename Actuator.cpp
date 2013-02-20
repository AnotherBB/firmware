/*
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
 *
 * Actuator.cpp
 *
 * Created: 19/02/2013 18:23:38
 *  Author: mat
 */ 

#include <Arduino.h>
#include "pins.h"
#include "Actuator.h"

DigitalPinActuator::DigitalPinActuator(int _pin, boolean _inverse, boolean _initiallyActive)
	 : pin(_pin), inverse(_inverse)
{	
	pinMode(pin, OUTPUT);
	setActive(_initiallyActive);	
}
	
void DigitalPinActuator::setActive(boolean active)
{
	this->active = active;
	digitalWrite(pin, active ? HIGH : LOW);
}
	
boolean DigitalPinActuator::isActive()
{
	return active;						
}		
	
