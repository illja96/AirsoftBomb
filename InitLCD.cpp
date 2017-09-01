#include "InitLCD.h"

InitLCD::InitLCD() {}

LiquidCrystal InitLCD::Init(byte LCDColumns, byte LCDRows,
	byte LCDPinRS, byte LCDPinEnable,
	byte LCDPinD4, byte LCDPinD5, byte LCDPinD6, byte LCDPinD7)
{
	LiquidCrystal lcd = LiquidCrystal(LCDPinRS, LCDPinEnable, LCDPinD4, LCDPinD5, LCDPinD6, LCDPinD7);
	lcd.begin(LCDRows, LCDColumns);
	lcd.clear();

	return lcd;
}

LiquidCrystal InitLCD::Init(byte LCDColumns, byte LCDRows,
	byte LCDPinRS, byte LCDPinRW, byte LCDPinEnable,
	byte LCDPinD4, byte LCDPinD5, byte LCDPinD6, byte LCDPinD7)
{
	LiquidCrystal lcd = LiquidCrystal(LCDPinRS, LCDPinRW, LCDPinEnable, LCDPinD4, LCDPinD5, LCDPinD6, LCDPinD7);
	lcd.begin(LCDRows, LCDColumns);
	lcd.clear();

	return lcd;
}