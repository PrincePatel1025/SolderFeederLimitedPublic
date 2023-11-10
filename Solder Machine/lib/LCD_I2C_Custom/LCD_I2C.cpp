// Based on the work by DFRobot

#include "LCD_I2C.h"
#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#define printIIC(args) Wire.write(args)
inline size_t LCDI2C::write(uint8_t value)
{
	send(value, Rs);
	return 1;
}

#else
#include "WProgram.h"

#define printIIC(args) Wire.send(args)
inline void LCDI2C::write(uint8_t value)
{
	send(value, Rs);
}

#endif
#include "Wire.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LCDI2C::LCDI2C() {}

LCDI2C::LCDI2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows)
{
	_Addr = lcd_Addr;
	_cols = lcd_cols;
	_rows = lcd_rows;
	_backlightval = LCD_NOBACKLIGHT;

	totalXLength = _cols;
	totalYLength = _rows;
}

void LCDI2C::init()
{
	init_priv();
}

void LCDI2C::init_priv()
{
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin(_cols, _rows);
	clear();
}

void LCDI2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	if (lines > 1)
	{
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1))
	{
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
	delay(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval); // reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	// put the LCD into 4 bit mode
	//  this is according to the hitachi HD44780 datasheet
	//  figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4);
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	home();
}

/********** high level commands, for the user! */
void LCDI2C::clear()
{
	command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	delayMicroseconds(2000);   // this command takes a long time!
}

void LCDI2C::home()
{
	command(LCD_RETURNHOME); // set cursor position to zero
	delayMicroseconds(2000); // this command takes a long time!
}

void LCDI2C::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	if (row > _numlines)
	{
		row = _numlines - 1; // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCDI2C::noDisplay()
{
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDI2C::display()
{
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCDI2C::noCursor()
{
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDI2C::cursor()
{
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCDI2C::noBlink()
{
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDI2C::blink()
{
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCDI2C::scrollDisplayLeft(void)
{
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCDI2C::scrollDisplayRight(void)
{
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCDI2C::leftToRight(void)
{
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCDI2C::rightToLeft(void)
{
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCDI2C::autoscroll(void)
{
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCDI2C::noAutoscroll(void)
{
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDI2C::createChar(uint8_t location, uint8_t charmap[])
{
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i = 0; i < 8; i++)
	{
		write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void LCDI2C::noBacklight(void)
{
	_backlightval = LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LCDI2C::backlight(void)
{
	_backlightval = LCD_BACKLIGHT;
	expanderWrite(0);
}

/*********** mid level commands, for sending data/cmds */

inline void LCDI2C::command(uint8_t value)
{
	send(value, 0);
}

/************ low level data pushing commands **********/

// write either command or data
void LCDI2C::send(uint8_t value, uint8_t mode)
{
	uint8_t highnib = value & 0xf0;
	uint8_t lownib = (value << 4) & 0xf0;
	write4bits((highnib) | mode);
	write4bits((lownib) | mode);
}

void LCDI2C::write4bits(uint8_t value)
{
	expanderWrite(value);
	pulseEnable(value);
}

void LCDI2C::expanderWrite(uint8_t _data)
{
	Wire.beginTransmission(_Addr);
	printIIC((int)(_data) | _backlightval);
	Wire.endTransmission();
}

void LCDI2C::pulseEnable(uint8_t _data)
{
	expanderWrite(_data | En); // En high
	delayMicroseconds(1);	   // enable pulse must be >450ns

	expanderWrite(_data & ~En); // En low
	delayMicroseconds(50);		// commands need > 37us to settle
}

// Alias functions

void LCDI2C::cursor_on()
{
	cursor();
}

void LCDI2C::cursor_off()
{
	noCursor();
}

void LCDI2C::blink_on()
{
	blink();
}

void LCDI2C::blink_off()
{
	noBlink();
}

void LCDI2C::load_custom_character(uint8_t char_num, uint8_t *rows)
{
	createChar(char_num, rows);
}

void LCDI2C::setBacklight(uint8_t new_val)
{
	if (new_val)
	{
		backlight(); // turn backlight on
	}
	else
	{
		noBacklight(); // turn backlight off
	}
}

void LCDI2C::printstr(const char c[])
{
	// This function is not identical to the function used for "real" I2C displays
	// it's here so the user sketch doesn't have to be changed
	Print::print(c);
}

// !Custom API's

void LCDI2C::cPrint(uint8_t xCursorLocation, uint8_t yCursorLocation, String message)
{
	setCursor(xCursorLocation, yCursorLocation);
	Print::print(message);
}
void LCDI2C::cPrint(uint8_t xCursorLocation, uint8_t yCursorLocation, int8_t message)
{
	setCursor(xCursorLocation, yCursorLocation);
	Print::print(message);
}
void LCDI2C::cPrint(uint8_t xCursorLocation, uint8_t yCursorLocation, float message)
{
	setCursor(xCursorLocation, yCursorLocation);
	Print::print(message);
}
void LCDI2C::cPrint(uint8_t xCursorLocation, uint8_t yCursorLocation, long message)
{
	setCursor(xCursorLocation, yCursorLocation);
	Print::print(message);
}
void LCDI2C::cPrint(uint8_t xCursorLocation, uint8_t yCursorLocation, unsigned long message)
{
	setCursor(xCursorLocation, yCursorLocation);
	Print::print(message);
}

void LCDI2C::leftShift(const String message, const uint8_t maxMessageLength, const uint8_t startLocationX, const uint8_t startLocationY)
{
	if (((startLocationX + maxMessageLength) <= totalXLength) && (startLocationY <= totalYLength))
	{

		byte messageLength = message.length();

		if (messageLength <= maxMessageLength)
		{
			byte spaces = maxMessageLength - messageLength;
			byte cursorLocation = startLocationX + messageLength - 1;

			setCursor(((startLocationX)-1), (startLocationY - 1));
			Print::print(message);

			setCursor(cursorLocation, (startLocationY - 1));
			for (byte i = (byte)0; i < spaces; i++)
			{
				Print::print(" ");
			}
		}
	}
}
void LCDI2C::leftShift(const String message, const uint8_t startLocationY)
{
	leftShift(message, message.length(), 1, startLocationY);
}
void LCDI2C::leftShift(const int message, const uint8_t maxMessageLength, const uint8_t startLocationX, const uint8_t startLocationY)
{
	String stringMessage = String(message);
	leftShift(stringMessage, maxMessageLength, startLocationX, startLocationY);
}
void LCDI2C::leftShift(const int message, const uint8_t startLocationY)
{
	String stringMessage = String(message);
	leftShift(stringMessage, stringMessage.length(), 1, startLocationY);
}

void LCDI2C::rightShift(const String message, const uint8_t maxMessageLength, const uint8_t endLocationX, const uint8_t startLocationY)
{
	byte messageLength = message.length();

	if ((endLocationX <= 20) && (messageLength <= 20) && (startLocationY <= 4))
	{
		if (((int)(endLocationX - maxMessageLength) >= 0) && (messageLength <= maxMessageLength))
		{
			int spaces = maxMessageLength - messageLength;

			setCursor(((endLocationX - maxMessageLength)), (startLocationY)-1);
			for (int i = 0; i < spaces; i++)
			{
				Print::print(" ");
			}

			Print::print(message);
		}
	}
}
void LCDI2C::rightShift(const String message, const uint8_t startLocationY)
{
	rightShift(message, message.length(), 20, startLocationY);
}
void LCDI2C::rightShift(const int message, const uint8_t maxMessageLength, const uint8_t endLocationX, const uint8_t startLocationY)
{
	String stringMessage = String(message);
	rightShift(stringMessage, maxMessageLength, endLocationX, startLocationY);
}
void LCDI2C::rightShift(const int message, const uint8_t startLocationY)
{
	String stringMessage = String(message);
	leftShift(stringMessage, stringMessage.length(), 20, startLocationY);
}

void LCDI2C::centerStage(const String message, const uint8_t startLocationX, const uint8_t endLocationX, const uint8_t startLocationY)
{
	int messageLength = message.length();
	int totalPrintLength = (endLocationX - startLocationX) + 1;
	int cursorStartLocation = 1;
	int rightZeros = 0;
	int leftZeros = 0;
	int startLocationRightZeros = 0;
	int startLocationLeftZeros = 0;

	String newMessage = message;

	if (messageLength <= totalPrintLength)
	{

		if ((totalPrintLength % 2 == 0))
		{
			if (messageLength % 2 != 0)
			{
				newMessage = message + " ";
				messageLength++;
			}

			cursorStartLocation = ((totalPrintLength - messageLength) / 2) + startLocationX;
			startLocationRightZeros = startLocationX;
			startLocationLeftZeros = (cursorStartLocation + messageLength);
			rightZeros = ((totalPrintLength - messageLength) / 2);
			leftZeros = rightZeros;
		}

		else if (totalPrintLength % 2 != 0)
		{
			if (messageLength % 2 == 0)
			{
				newMessage = message + " ";
				messageLength++;
			}

			cursorStartLocation = ((totalPrintLength - messageLength) / 2) + startLocationX;
			startLocationRightZeros = startLocationX;
			startLocationLeftZeros = (cursorStartLocation + messageLength);
			rightZeros = ((totalPrintLength - messageLength) / 2);
			leftZeros = rightZeros;
		}

		setCursor(cursorStartLocation - 1, startLocationY - 1);
		Print::print(newMessage);

		setCursor(startLocationRightZeros - 1, startLocationY - 1);
		for (int i = 0; i < rightZeros; i++)
		{
			Print::print(" ");
		}

		setCursor(startLocationLeftZeros - 1, startLocationY - 1);
		for (int i = 0; i < leftZeros; i++)
		{
			Print::print(" ");
		}
	}
}

void LCDI2C::centerStage(const int message, const uint8_t startLocationX, const uint8_t endLocationX, const uint8_t startLocationY)
{
	String stringMessage = String(message);
	centerStage(stringMessage, startLocationX, endLocationX, startLocationY);
}

void LCDI2C::centerStage(const String message, const uint8_t startLocationY)
{
	centerStage(message, 1, 20, startLocationY);
}

void LCDI2C::centerStage(const int message, const uint8_t startLocationY)
{
	String stringMessage = String(message);
	centerStage(stringMessage, 1, 20, startLocationY);
}

void LCDI2C::clean(const int startXLocation, const int endXLocation, const int startYLocation)
{
	setCursor(startXLocation - 1, startYLocation - 1);

	for (int i = 0; i < (endXLocation - startXLocation) + 1; i++)
	{
		print(" ");
	}
}
void LCDI2C::clean(const int startYLocation)
{
	clean(1, 20, startYLocation);
}

#ifdef Debug
void leftShift(const String message, const unsigned int maxMessageLength, const unsigned int startLocationX, const unsigned int startLocationY, char code)
{

	if (((startLocationX + maxMessageLength) <= totalXLength) && (startLocationY <= totalYLength))
	{

		byte messageLength = message.length();

		if (messageLength <= maxMessageLength)
		{
			byte spaces = maxMessageLength - messageLength;
			byte cursorLocation = startLocationX + messageLength - 1;

			lcd->setCursor(((startLocationX - 1)), (startLocationY - 1));
			lcd->print(message);

			lcd->setCursor(cursorLocation, (startLocationY - 1));
			for (byte i = 0; i < spaces; i++)
			{
				lcd->print(" ");
			}
		}

		else
		{
			lcd->setCursor(0, (startLocationY - 1));
			lcd->print(code);
			lcd->print("001");
		}
	}
}
#endif
