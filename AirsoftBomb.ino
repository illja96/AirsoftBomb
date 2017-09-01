#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Key.h>

#include "BombProcessor.h"
#include "InitLCD.h"

const byte PrevStageKeyPin = 50, NextStageKeyPin = 52;
const byte BuzzerPin = 2;

const byte LCDRows = 2, LCDColumns = 16;
const byte LCDPinRS = 34, LCDPinRW = 32, LCDPinEnable = 30, LCDPinD4 = 28, LCDPinD5 = 26, LCDPinD6 = 24, LCDPinD7 = 22;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	BombProcessor bombProcessor;
	bombProcessor.Init(PrevStageKeyPin, NextStageKeyPin, BuzzerPin);
	bombProcessor.InitLCD(LCDRows, LCDColumns, LCDPinRS, LCDPinRW, LCDPinEnable, LCDPinD4, LCDPinD5, LCDPinD6, LCDPinD7);
	bombProcessor.InitKeypad();

	bombProcessor.Process();
}