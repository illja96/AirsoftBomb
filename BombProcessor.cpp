#include "BombProcessor.h"

const byte keypadRows = 4;
byte keypadRowsPins[keypadRows] = { 45, 43, 41, 39 };

const byte keypadColumns = 4;
byte keypadColumnsPins[keypadColumns] = { 53, 51, 49, 47 };

char keypadKeysChars[keypadRows][keypadColumns] =
{
	{ '1', '2', '3', 'A' },
	{ '4', '5', '6', 'B' },
	{ '7', '8', '9', 'C' },
	{ '*', '0', '#', 'D' }
};

BombProcessor::BombProcessor()
{
	BombDelayInMillis = 0;

	BuzzerKeypadFrequency = 1400;
	BuzzerKeypadDuration = 100;

	BuzzerErrorFrequency = 250;
	BuzzerErrorDuration = 1000;

	BuzzerArmFrequency = 1000;
	BuzzerArmDuration = 500;

	BuzzerDefaultFrequency = 40;
	BuzzerDefaultDuration = 250;

	CodeLenght = 0;
	for (int i = 0; i < 15; i++)
		Code[i] = '\0';
}
void BombProcessor::Init(byte prevStageKeyPin, byte nextStageKeyPin, byte buzzerPin, bool isDebug = false)
{
	this->PrevStageKeyPin = prevStageKeyPin;
	pinMode(PrevStageKeyPin, INPUT_PULLUP);

	this->NextStageKeyPin = nextStageKeyPin;
	pinMode(NextStageKeyPin, INPUT_PULLUP);

	BuzzerPin = buzzerPin;
	pinMode(BuzzerPin, OUTPUT);

	this->isDebug = isDebug;
}

void BombProcessor::InitLCD(byte LCDColumns, byte LCDRows, byte LCDPinRS, byte LCDPinRW, byte LCDPinEnable, byte LCDPinD4, byte LCDPinD5, byte LCDPinD6, byte LCDPinD7)
{
	lcd = LiquidCrystal(LCDPinRS, LCDPinRW, LCDPinEnable, LCDPinD4, LCDPinD5, LCDPinD6, LCDPinD7);
	lcd.begin(LCDRows, LCDColumns);
	lcd.clear();
}
void BombProcessor::InitKeypad()
{
	keypad = Keypad(makeKeymap(keypadKeysChars), keypadRowsPins, keypadColumnsPins, keypadRows, keypadColumns);
}

void BombProcessor::Process()
{
	int stage = 1;

	while (true)
	{
		int isStageState = 0;

		switch (stage)
		{
		case 1:
			isStageState = Authors();
			break;

		case 2:
			isStageState = SetBombDelay();
			break;

		case 3:
			isStageState = SetCodeLenght();
			break;

		case 4:
			isStageState = SetCode();
			break;

		case 5:
			isStageState = Arm();
			break;

		case 6:
			isStageState = Defuse();
			break;

		case 7:
			Deactiveted();
			break;

		case 8:
			Exploded();
			break;
		}

		if (isStageState == 0)
		{
			tone(BuzzerPin, BuzzerErrorFrequency, BuzzerErrorDuration);
			delay(BuzzerErrorDuration * 2);
			tone(BuzzerPin, BuzzerErrorFrequency, BuzzerErrorDuration);
			delay(BuzzerErrorDuration);
		}
		else
		{
			tone(BuzzerPin, BuzzerDefaultFrequency, BuzzerDefaultDuration);
			delay(BuzzerDefaultDuration);
		}

		stage += isStageState;
	}
}

int BombProcessor::Authors()
{
	lcd.clear();
	lcd.print("  Airsoft bomb  ");
	lcd.setCursor(0, 1);
	lcd.print("by Annoying Bork");
	delay(1000);

	return 1;
}

int BombProcessor::SetBombDelay()
{
	bool isStageCanceled = false, isStageComplete = false;

	char timeDelay[6];
	for (int i = 0; i < 6; i++)
		timeDelay[i] = '0';

	char timeLCDString[16];

	if (BombDelayInMillis != 0)
	{
		char TimeChars[2];
		byte hours, minutes, seconds;
		GetBombTime(BombDelayInMillis, hours, minutes, seconds);

		sprintf(TimeChars, "%02d", hours);
		timeDelay[5] = TimeChars[0];
		timeDelay[4] = TimeChars[1];

		sprintf(TimeChars, "%02d", minutes);
		timeDelay[3] = TimeChars[0];
		timeDelay[2] = TimeChars[1];

		sprintf(TimeChars, "%02d", seconds);
		timeDelay[1] = TimeChars[0];
		timeDelay[0] = TimeChars[1];
	}

	DrawBombTime(timeDelay, timeLCDString);

	lcd.clear();
	lcd.print("Enter bomb delay");
	lcd.setCursor(0, 1);
	lcd.print("    HH:MM:SS    ");
	delay(1000);

	lcd.setCursor(0, 1);
	lcd.print(timeLCDString);

	do
	{
		isStageCanceled = GetPrevStageKeyState();
		isStageComplete = GetNextStageKeyState();

		char keypadPressedKeyChar = GetKey();

		if (keypadPressedKeyChar == '\0' || !isDigit(keypadPressedKeyChar))
			continue;

		for (int i = 5; i > 0; i--)
		{
			if (!isDigit(timeDelay[i - 1]))
				timeDelay[i - 1] = '0';

			timeDelay[i] = timeDelay[i - 1];
		}
		timeDelay[0] = keypadPressedKeyChar;

		DrawBombTime(timeDelay, timeLCDString);

		lcd.setCursor(0, 1);
		lcd.print(timeLCDString);

	} while (!isStageCanceled && !isStageComplete);

	if (isStageCanceled)
	{
		lcd.clear();
		lcd.print("  No rollback   ");
		lcd.setCursor(0, 1);
		lcd.print("   available    ");

		return 0;
	}

	byte Hours = ((timeDelay[5] - '0') * 10) + (timeDelay[4] - '0');

	byte Minutes = ((timeDelay[3] - '0') * 10) + (timeDelay[2] - '0');
	if (Minutes > 60)
		Minutes = 60;

	byte Seconds = ((timeDelay[1] - '0') * 10) + (timeDelay[0] - '0');
	if (Seconds > 60)
		Seconds = 60;

	if (Seconds == 0 && Minutes == 0 && Hours == 0)
	{
		lcd.clear();
		lcd.print("  Time must be  ");
		lcd.setCursor(0, 1);
		lcd.print(" more than zero ");

		return 0;
	}

	unsigned long TotalInSeconds = ((unsigned long)Hours * 3600) + ((unsigned long)Minutes * 60) + Seconds;
	this->BombDelayInMillis = TotalInSeconds * 1000;

	if (isStageComplete)
		return 1;
}

int BombProcessor::SetCodeLenght()
{
	bool isStageCanceled = false, isStageComplete = false;

	char newCodeLenght[2] = { '0', '0' };
	char newCodeLenghtLCD[16];

	if (CodeLenght != 0)
	{
		newCodeLenght[0] = (CodeLenght - ((CodeLenght / 10) * 10)) + '0';
		newCodeLenght[1] = (CodeLenght / 10) + '0';
	}

	DrawCodeLenght(newCodeLenght, newCodeLenghtLCD);

	lcd.clear();
	lcd.print("Set code lenght ");
	lcd.setCursor(0, 1);
	lcd.print(newCodeLenghtLCD);

	do
	{
		isStageCanceled = GetPrevStageKeyState();
		isStageComplete = GetNextStageKeyState();

		char keypadPressedKeyChar = GetKey();

		if (keypadPressedKeyChar == '\0' || !isDigit(keypadPressedKeyChar))
			continue;

		newCodeLenght[1] = newCodeLenght[0];
		if (newCodeLenght[1] - '0' > 1)
			newCodeLenght[1] = '1';

		newCodeLenght[0] = keypadPressedKeyChar;
		if (newCodeLenght[1] == '1' && newCodeLenght[0] - '0' > 6)
			newCodeLenght[0] = '6';

		DrawCodeLenght(newCodeLenght, newCodeLenghtLCD);

		lcd.setCursor(0, 1);
		lcd.print(newCodeLenghtLCD);

	} while (!isStageCanceled && !isStageComplete);

	if (isStageCanceled)
		return -1;

	byte tempCodeLenght = ((newCodeLenght[1] - '0') * 10) + (newCodeLenght[0] - '0');
	if (tempCodeLenght == 0)
	{
		lcd.clear();
		lcd.print("Code lenght must");
		lcd.setCursor(0, 1);
		lcd.print(" be more than 0 ");

		return 0;
	}
	CodeLenght = tempCodeLenght;

	if (isStageComplete)
		return 1;
}
int BombProcessor::SetCode()
{
	bool isStageCanceled = false, isStageComplete = false;

	char newBombCode[16];

	char newBombCodeLCD[16];
	if (Code[0] == '\0')
	{
		randomSeed(millis());
		for (int i = 0; i < CodeLenght; i++)
		{
			long newBombCodeRandValue = random(0, 14);

			if (newBombCodeRandValue < 10)
				newBombCode[i] = newBombCodeRandValue + '0';
			else
				newBombCode[i] = (newBombCodeRandValue - 10) + 'A';
		}
	}
	else
	{
		for (int i = 0; i < 15; i++)
			newBombCode[i] = Code[i];
	}

	DrawCode(newBombCode, newBombCodeLCD);

	lcd.clear();
	lcd.print("   Enter code   ");
	lcd.setCursor(0, 1);
	lcd.print(newBombCodeLCD);

	do
	{
		isStageCanceled = GetPrevStageKeyState();
		isStageComplete = GetNextStageKeyState();

		char keypadPressedKeyChar = GetKey();

		if (keypadPressedKeyChar == '\0')
			continue;

		bool isDigit = keypadPressedKeyChar >= '0' && keypadPressedKeyChar <= '9';
		bool isChar = keypadPressedKeyChar >= 'A' && keypadPressedKeyChar <= 'D';

		if (!isDigit && !isChar)
			continue;

		for (int i = 15; i > 0; i--)
			newBombCode[i] = newBombCode[i - 1];
		newBombCode[0] = keypadPressedKeyChar;

		DrawCode(newBombCode, newBombCodeLCD);

		lcd.setCursor(0, 1);
		lcd.print(newBombCodeLCD);

	} while (!isStageCanceled && !isStageComplete);

	if (isStageCanceled)
		return -1;

	for (int i = 0; i < 15; i++)
		Code[i] = newBombCode[i];

	if (isStageComplete)
		return 1;
}

int BombProcessor::Arm()
{
	bool isStageCanceled = false, isStageComplete = false;

	lcd.clear();
	lcd.print("Press next key  ");
	lcd.setCursor(0, 1);
	lcd.print("to arm the bomb ");

	do
	{
		isStageCanceled = GetPrevStageKeyState();
		isStageComplete = GetNextStageKeyState();

	} while (!isStageCanceled && !isStageComplete);

	if (isStageCanceled)
		return -1;

	lcd.clear();
	lcd.print("   Bomb armed   ");

	if (isStageComplete)
		return 1;
}
int BombProcessor::Defuse()
{
	bool isStageCanceled = false, isStageComplete = false;

	unsigned long bombStartTimeInMillis = millis();
	unsigned long bombDetonateTimeInMillis = millis() + BombDelayInMillis;
	unsigned long bombTimeToDetonateLeftInMillis = bombDetonateTimeInMillis - millis();

	unsigned long lastLCDUpdate = millis() - 1 * 1000;

	unsigned long beepDelay = GetArmBuzzerDelay(bombTimeToDetonateLeftInMillis);
	unsigned long lastBeep = millis() - beepDelay;

	char bombCodeLCD[16];
	byte decodedCodeLenght = 0;

	unsigned long lastKeypadPress = millis() - 5 * 1000;

	char bombTimeLCD[16];
	byte bombTimeHours, bombTimeMinutes, bombTimeSeconds;

	lcd.clear();

	do
	{
		isStageCanceled = bombDetonateTimeInMillis < millis();
		isStageComplete = CodeLenght == decodedCodeLenght;

		char keypadPressedKeyChar = GetKey();

		if (keypadPressedKeyChar != '\0')
			lastKeypadPress = millis();

		bool isTimer = lastKeypadPress + 5 * 1000 < millis();

		if (isTimer)
		{
			unsigned long bombTimeToDetonateLeftInMillis = bombDetonateTimeInMillis - millis();

			bool isTimeToUpdate = lastLCDUpdate + 1 * 1000 < millis();
			if (isTimeToUpdate)
			{
				GetBombTime(bombTimeToDetonateLeftInMillis, bombTimeHours, bombTimeMinutes, bombTimeSeconds);
				DrawBombTime(bombTimeHours, bombTimeMinutes, bombTimeSeconds, bombTimeLCD);

				lcd.setCursor(0, 0);
				lcd.print("   Bomb armed   ");
				lcd.setCursor(0, 1);
				lcd.print(bombTimeLCD);

				lastLCDUpdate = millis();
			}

			bool isTimeToBeep = lastBeep + beepDelay < millis();
			if (isTimeToBeep)
			{
				tone(BuzzerPin, BuzzerArmFrequency, BuzzerArmFrequency);
				beepDelay = GetArmBuzzerDelay(bombTimeToDetonateLeftInMillis);
				lastBeep = millis();
			}
		}
		else
		{
			if (keypadPressedKeyChar == '\0')
				continue;

			if (keypadPressedKeyChar == Code[decodedCodeLenght])
				decodedCodeLenght++;
			else
				decodedCodeLenght = 0;

			Serial.print("AFTER keypadPressedKeyChar = ");
			Serial.println(keypadPressedKeyChar);

			Serial.print("Code[decodedCodeLenght] = ");
			Serial.println(Code[decodedCodeLenght]);

			Serial.print("decodedCodeLenght = ");
			Serial.println(decodedCodeLenght);

			DrawCode(Code, decodedCodeLenght, bombCodeLCD);

			lcd.setCursor(0, 0);
			lcd.print("   Enter code   ");
			lcd.setCursor(0, 1);
			lcd.print(bombCodeLCD);
		}

	} while (!isStageCanceled && !isStageComplete);

	if (isStageComplete)
		return 1;

	if (isStageCanceled)
		return 2;
}

int BombProcessor::Deactiveted()
{
	while (true)
	{
		lcd.clear();
		lcd.print("      Bomb      ");
		lcd.setCursor(0, 1);
		lcd.print("  deactiveted   ");
		delay(500);

		lcd.clear();
		delay(500);
	}
}
int BombProcessor::Exploded()
{
	while (true)
	{
		tone(BuzzerPin, BuzzerArmFrequency, BuzzerArmDuration * 3);

		lcd.clear();
		lcd.print("      Bomb      ");
		lcd.setCursor(0, 1);
		lcd.print("    exploded    ");
		delay(500);

		lcd.clear();
		delay(500);
	}
}

char BombProcessor::GetKey()
{
	char keypadPressedKeyChar = NO_KEY;

	keypadPressedKeyChar = keypad.getKey();

	/*if (keypadPressedKeyChar == NO_KEY && isDebug && Serial.available())
	{
	keypadPressedKeyChar = Serial.read();
	Serial.flush();
	}*/

	if (keypadPressedKeyChar != NO_KEY)
	{
		tone(BuzzerPin, BuzzerKeypadFrequency, BuzzerKeypadDuration);
		Serial.print("keypadPressedKeyChar = ");
		Serial.println(keypadPressedKeyChar);
	}

	return keypadPressedKeyChar;
}
inline bool BombProcessor::GetPrevStageKeyState()
{
	return digitalRead(PrevStageKeyPin) != HIGH;
}
inline bool BombProcessor::GetNextStageKeyState()
{
	return digitalRead(NextStageKeyPin) != HIGH;
}

void BombProcessor::GetBombTime(const unsigned long bombTime, byte& hours, byte& minutes, byte& seconds)
{
	char TimeChars[2];

	hours = bombTime / 3600000;

	minutes = (bombTime - (hours * 3600000)) / 60000;

	seconds = (bombTime - (hours * 3600000) - (minutes * 60000)) / 1000;
}

void BombProcessor::DrawBombTime(const byte hours, const byte minutes, const byte seconds, char printChars[16])
{
	char TimeChars[2];
	char timeDelay[6];

	sprintf(TimeChars, "%02d", hours);
	timeDelay[5] = TimeChars[0];
	timeDelay[4] = TimeChars[1];

	sprintf(TimeChars, "%02d", minutes);
	timeDelay[3] = TimeChars[0];
	timeDelay[2] = TimeChars[1];

	sprintf(TimeChars, "%02d", seconds);
	timeDelay[1] = TimeChars[0];
	timeDelay[0] = TimeChars[1];

	return DrawBombTime(timeDelay, printChars);
}
void BombProcessor::DrawBombTime(const char timeDelayChars[6], char printChars[16])
{
	printChars[0] = ' ';
	printChars[1] = ' ';
	printChars[2] = ' ';
	printChars[3] = ' ';
	printChars[4] = timeDelayChars[5];
	printChars[5] = timeDelayChars[4];
	printChars[6] = ':';
	printChars[7] = timeDelayChars[3];
	printChars[8] = timeDelayChars[2];
	printChars[9] = ':';
	printChars[10] = timeDelayChars[1];
	printChars[11] = timeDelayChars[0];
	printChars[12] = ' ';
	printChars[13] = ' ';
	printChars[14] = ' ';
	printChars[15] = '\0';
}

void BombProcessor::DrawCodeLenght(const char codeLenghtChars[2], char printChars[16])
{
	printChars[0] = ' ';
	printChars[1] = ' ';
	printChars[2] = ' ';
	printChars[3] = ' ';
	printChars[4] = ' ';
	printChars[5] = ' ';
	printChars[6] = ' ';
	printChars[7] = codeLenghtChars[1];
	printChars[8] = codeLenghtChars[0];
	printChars[9] = ' ';
	printChars[10] = ' ';
	printChars[11] = ' ';
	printChars[12] = ' ';
	printChars[13] = ' ';
	printChars[14] = ' ';
	printChars[15] = '\0';
}
void BombProcessor::DrawCode(const char codeChars[16], char printChars[16])
{
	byte offset = (16 - CodeLenght) / 2;

	for (int i = 0, j = 0; i < 16; i++)
	{
		if (i < offset || i > 16 - (offset + 1))
		{
			printChars[i] = ' ';
			continue;
		}

		printChars[i] = codeChars[CodeLenght - 1 - j];
		j++;
	}
}
void BombProcessor::DrawCode(const char codeChars[16], const byte decodedCodeLenght, char printChars[16])
{
	char newCodeChars[16];

	for (int i = 0; i < 16; i++)
	{
		if (i >= decodedCodeLenght)
			newCodeChars[i] = '*';
		else
			newCodeChars[i] = codeChars[i];
	}

	DrawCode(newCodeChars, printChars);
}

unsigned long BombProcessor::GetArmBuzzerDelay(const unsigned long timeToDetonateLeftInMillis)
{
	const unsigned long oneSecondInMillis = 1000;
	const unsigned long oneAndHalfSecondInMillis = 1.5 * oneSecondInMillis;
	const unsigned long threeSecondInMillis = 3 * oneSecondInMillis;
	const unsigned long tenSecondsInMillis = 10 * oneSecondInMillis;
	const unsigned long oneMinuteInMillis = 60 * oneSecondInMillis;
	const unsigned long tenMinutesInMillis = 10 * oneMinuteInMillis;

	if (timeToDetonateLeftInMillis <= oneMinuteInMillis)
		return LinearInterpolation(0, oneSecondInMillis, oneMinuteInMillis, oneAndHalfSecondInMillis, timeToDetonateLeftInMillis);

	if (timeToDetonateLeftInMillis <= tenMinutesInMillis)
		return LinearInterpolation(oneMinuteInMillis, oneAndHalfSecondInMillis, tenMinutesInMillis, tenSecondsInMillis, timeToDetonateLeftInMillis);

	return tenSecondsInMillis;
}
unsigned long BombProcessor::LinearInterpolation(unsigned long x1, unsigned long fx1, unsigned long x2, unsigned long fx2, unsigned long x)
{
	if (x1 > x2)
	{
		unsigned long temp = x1;
		x1 = x2;
		x2 = temp;

		temp = fx1;
		fx1 = fx2;
		fx2 = temp;
	}

	float x1InSeconds = float(x1) / 1000;
	float fx1InSeconds = float(fx1) / 1000;
	float x2InSeconds = float(x2) / 1000;
	float fx2InSeconds = float(fx2) / 1000;
	float xInSeconds = float(x) / 1000;
	float fxInSeconds = fx1InSeconds + (fx2InSeconds - fx1InSeconds) * (xInSeconds - x1InSeconds) / (x2InSeconds - x1InSeconds);

	return fxInSeconds * 1000;
}