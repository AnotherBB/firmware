/*
 * chamberManager.cpp
 *
 * Created: 15/03/2013 02:23:11
 *  Author: mat
 */ 

#include "TempControl.h"
#include "chamber.h"


void Chamber::apply()
{
	state.apply();
}	

void Chamber::retract()
{
	state.retract();
}
