#ifndef _INITLCD_h
#define _INITLCD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <LiquidCrystal.h>

class InitLCD
{
public:
	InitLCD();

	LiquidCrystal Init(byte LCDColumns, byte LCDRows,
		byte LCDPinRS, byte LCDPinEnable,
		byte LCDPinD4, byte LCDPinD5, byte LCDPinD6, byte LCDPinD7);

	LiquidCrystal Init(byte LCDColumns, byte LCDRows,
		byte LCDPinRS, byte LCDPinRW, byte LCDPinEnable,
		byte LCDPinD4, byte LCDPinD5, byte LCDPinD6, byte LCDPinD7);
};

extern InitLCD initLCD;

#endif