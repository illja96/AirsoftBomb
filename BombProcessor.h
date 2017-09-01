#ifndef _BOMBPROCESSOR_h
#define _BOMBPROCESSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Key.h>

class BombProcessor
{
private:
	bool isDebug;

	LiquidCrystal lcd = LiquidCrystal(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	Keypad keypad = Keypad(NULL, NULL, NULL, NULL, NULL);

	byte PrevStageKeyPin, NextStageKeyPin;

	byte BuzzerPin;
	int BuzzerKeypadFrequency, BuzzerErrorFrequency, BuzzerArmFrequency, BuzzerDefaultFrequency;
	int BuzzerKeypadDuration, BuzzerErrorDuration, BuzzerArmDuration, BuzzerDefaultDuration;

	unsigned long BombDelayInMillis;

	byte CodeLenght;
	char Code[16];
public:
	BombProcessor();
	void Init(byte prevStageKeyPin, byte nextStageKeyPin, byte buzzerPin, bool isDebug = false);
	void InitLCD(byte LCDColumns, byte LCDRows,
		byte LCDPinRS, byte LCDPinRW, byte LCDPinEnable,
		byte LCDPinD4, byte LCDPinD5, byte LCDPinD6, byte LCDPinD7);
	void InitKeypad();
	void Process();

private:
	int Authors();
	int SetBombDelay();
	int SetCodeLenght();
	int SetCode();
	int Arm();
	int Defuse();
	int Deactiveted();
	int Exploded();

	char GetKey();
	inline bool GetPrevStageKeyState();
	inline bool GetNextStageKeyState();

	void GetBombTime(const unsigned long bombTime, byte& hours, byte& minutes, byte& seconds);

	void DrawBombTime(const char timeDelayChars[6], char printChars[16]);
	void DrawBombTime(const byte hours, const byte minutes, const byte seconds, char printChars[16]);

	void DrawCodeLenght(const char codeLenghtChars[2], char printChars[16]);
	void DrawCode(const char codeChars[16], char printChars[16]);
	void DrawCode(const char codeChars[16], const byte decodedCodeLenght, char printChars[16]);

	unsigned long GetArmBuzzerDelay(const unsigned long timeToDetonateLeftInMillis);
	unsigned long LinearInterpolation(unsigned long x1, unsigned long fx1, unsigned long x2, unsigned long fx2, unsigned long x);
};

#endif