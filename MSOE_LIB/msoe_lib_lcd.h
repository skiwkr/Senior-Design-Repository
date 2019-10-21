/*
 * msoe_lib_lcd.h
 *
 *  Created on: May 22, 2017
 *      Author: johnsontimoj
 *
 *      Rev:    0.1     5/24/17     added operational comments
 *      Rev:    0.2     8/8/17      added code to wait for SPI to complete
 *                                      on Command and Data writes to the LCD
 *                                  Fixed a bug with decimal printing to allow
 *                                      for printing the value 0
 *                                  Added LCD_print_hex32 for printing 32bit hex
 *      Rev:    0.3     9/13/17     Add a function to change the contrast LCD_contrast
 *
 */
#ifndef __MSOE_LIB_LCD_H__
#define __MSOE_LIB_LCD_H__
/////////////////////////////////////////////////////////////////
// Font tables from Sparkfun and Cypress example code
// Function ideas from Sparkfun and Cypress
//////////////////////////////////////////////////////////////////
//
// Driver code for Nokia 5110 48x84 pixel LCD display
//
// This driver code interfaces the MSP432 to the
// Phillips PCD8544 controller on the Nokia 5110 48x84 pixel LCD display.
// It uses the MSP432 eUSCI - SPI interface to communicate to the
// PCD8544.
//
///////////////////////////////////////////////////////////////
//
// Basic Operation
//
// The PCD8544 uses a SPI interface to receive messages. There are two
// types of messages supported by the display driver, Commands and Data.
// Commands are used to configure the Driver (and subsequently the actual LCD).
// Data is written to the on chip Display Data RAM (DDRAM). There is no
// read capability on the display driver.
//
// The display driver provides appropriate signals and voltages to drive
// the LCD. It continuously writes the data from the DDRAM to the LCD at
// a rate fast enough to prevent flicker.
//
// Typical operation involves:
// 1) Configure the MSP432 ports to operate the SPI and additional
//	  display driver signals
// 2) Configure the display driver via the SPI
// 3) Write the desired LCD pixels to the DDRAM on the display driver
//
////////////////////////////////////////////////////////////////
//
// Display configuration
//
// The 48 x 84 pixel array is configured at 6 vertical banks of 8
// pixels each X 84 columns. When data is written to the DDRAM it
// is written to 1-of-6 Y addresses (banks) and 1-of-84 X addresses (columns).
// The data written is an 8 bit word representing the 8 pixels for a single
// bank/column pair with the LSB as the top pixel and the MSB as
// the bottom pixel.
//
// The Sparkfun character set is 7 pixels wide by 8 pixels tall.
// Each full character has 1 column of blank pixels on either side of a 5 pixel character.
// Each full character has 1 row of blank pixels below a 7 pixel character.
// The character Array only contains the 5x8 portion of the character, the
// blank columns must be added separately.
// This configuration matches characters to banks on the display vertically
// and characters to every 7 pixels horizontally.
//
//		b b111b b
//		b 1bbb1 b		{0x7e, 0x11, 0x11, 0x11, 0x7e}	// A
//		b 1bbb1 b
//		b 1bbb1 b		7e - 1st column with LSB as top pixel - 0111 1110
//		b 11111 b		11 - 2nd column with LSB as top pixel - 0001 0001
//		b 1bbb1 b
//		b 1bbb1 b
//		b bbbbb b
//
////////////////////////////////////////////////////////////////
//
// Nokia 5110 LCD Module / MSP432 Hardware Configuration (pinout)
//
// VCC						pin 1	3.3V
// GND        				pin 2	GND
// SCE (Slave Select)   	pin 3	P9.4	output
// RST         				pin 4	P9.3	output		Active LOW
// D/C (data/commandBar)	pin 5	P9.2	output
// D/N (MOSI)   			pin 6 	P9.7	output
// SCLK       				pin 7	P9.5	output
// LED (back light)    		pin 8 	N/C
//
////////////////////////////////////////////////////////////////
//
// PCD8544 Programming
//
// The D/C signal is used to indicate whether SPI transfers are
// Data or Commands - D/C=1 --> Data,  D/C=0 --> Command
//
// There are two instruction sets in command mode: Basic and Extended
// Instructions available in either mode:
//		NOP				-	do nothing
//			0000 0000
//		Function Set	-	power down control; entry mode; extended instruction set control (H)
//			0010 0 PD V H
//				PD	- power down	:	0 = power on, 1 = power off
//				V	- addressing 	:	0 = Horz, 1 = vert (we use Horz)
//				H	- inst set		:	0 = Basic, 1 = Extended
//		Write Data		-	write data to the DDRAM
//			D7 D6 D5 D4 D3 D2 D1 D0
//
// Basic Instructions:
//		Display Control		-	sets display configuration
//			0000 1 D 0 E
//				DE	- 00 = display blank, 01 = normal mode
//					  10 = all on, 11 = inverse video mode
//		Y address of DDRAM	-	sets Y address of DDRAM (bank 0 to 6)
//			0100 0 Y2 Y1 Y0
//		X address of DDRAM	-	sets X address of DDRAM (column 0 to 83)
//			1 X6 X5 X4 X3 X2 X1 X0
//
// Extended Instructions:
//		Temp Control		-	select the temperature control curve
//			0000 01 TC1 TC0
//		Bias System			-	set the bias system value
//			0001 0 BS2 BS1 BS0
//				BS2-BS0 - not well documented but value of 4 recommended in the spec
//		Set Vop				-	select the LCD operating voltage
//			1 Vop6 Vop5 Vop4 Vop3 Vop2 Vop1 Vop0
//				Vop6-Vop0 - not well documented, Sparkfun suggests 0x31 for the 5110
//
//////////////////////////////////////////////////////////////////////////
//
// CODE
//
#include "msoe_lib_lcd_char.h"

///////////   Message write routines   ///////////////////////////
//
// 1) Wait for the SPI TX buffer to be empty indicating any previous transmit has completed
// 2) Set the D/C GPIO for Data or Command
// 3) Load the message into the SPI TX buffer - transmit starts as soon as the register is loaded
// note - TX only - no RX message to read
//
void static LCD_Data_WR(uint8_t data);
void static LCD_Command_WR(uint8_t data);

///////////   Configuration routines   ///////////////////////////
//
// Port Configuration
//
void static LCD_Port_Config(void);

//
// SPI Configuration
//
// P9.7 – MOSI, P9.5 – SCLK, P9.4 – CS
// see MSP432 documentation for SPI registers and configuration
//
void static LCD_SPI_Config(void);

//
// LCD Module Configuration
//
// 1) Configure the SPI
// 2) Configure the Ports
// 3) Reset the LCD
// 4) Turn off LCD and access Extended instructions
// 5) Set temp_coef/bias/Vop
// 6) Turn on LCD and access Basic instructions
// 7) Set display to normal mode - ready to accept Data transfers
// 8) Clear display
//
void LCD_Config(void);

///////////   Location routines   ///////////////////////////
//
// Character X-Y location
//
// x,y are character locations (0-11 for x, 0-5 for y)
// Each character is 7 pixels (columns) wide so must multiply Column position by 7
//
void LCD_goto_xy(uint8_t x, uint8_t y);

//
// Home
//
// Set location to 0,0
void LCD_home(void);

//
// Character row location
//
// Set new character row value - keep existing column location
// Range is 0 to 5
void LCD_row(uint8_t row);

//
// Character column location
//
// Set new character column value - keep existing row location
// Range is 0 to 11
void LCD_col(uint8_t col);

///////////   Display routines   ///////////////////////////
//
// Display character
//
// Sparkfun characters are 5 pixels wide
// Sparkfun characters are 7 pixels tall plus a 1 pixel buffer at the bottom
// Display characters have an additional 1 pixel buffer on each side
// Characters are stored in the array ASCII and indexed (offset) by 0x20
// The RAM stores the data (1 byte) as a column 8 bits tall with the LSB at the top
// when in Horizontal addressing mode
// After completing the write the LCD will be in the next character location
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_print_char(char val);

//
// Display string
//
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_print_str(char *str_ptr);

//
// Clear display
//
// Cycle through each 8-bit column location and write all blanks
// 6 banks x 84 columns = 504 8-bit column locations
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_clear(void);

//
// BMP display
//
// Display a 84x48 BMP file converted to an array
// Use Microsoft Paint to convert any image to an appropriately sized bitmap file
// Use the Cypress program Bitmap_to_C.exe program to convert the bitmap to an array
//
// 6 banks x 84 columns = 504 8-bit column locations
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_print_bmpArray(const char *bmpArray_ptr);

///////////   Big character display routines   ///////////////////////////
//
// Display big character
//
// Cypress big characters are 8 pixels wide
// Cypress big characters are 18 pixels tall
// Display characters add 2 columns of pixels either side of the Cypress character
// Characters are stored in the 3-D array ASCII_BIG and indexed (offset) by 0x20
// The RAM stores the data (1 byte) as a column 8 bits tall with the LSB at the top
// when in Horizontal addressing mode
// Since the characters span more than 1 row (bank) explicit control of the
// character location is required since there is no read capability on the controller
// This also means no wrapping
// Character indices are Row:0-2, Column:0-7
void LCD_print_bigchar (uint8_t x, uint8_t y, char val );

//
// Display big string
//
void LCD_print_bigstr(uint8_t x, uint8_t y, char *str_ptr);

///////////   Number display routines   ///////////////////////////
//
// Display binary
//
// Bitwise AND the value with each binary value then logical AND with 1
// Offset by 0x20 to index the array
void LCD_print_bin8(uint8_t val);
void LCD_print_bin16(uint16_t val);
//
// Display 8 bit Hex
//
void LCD_print_hex8(uint8_t val);
//
// Display 16 bit Hex
//
void LCD_print_hex16(uint16_t val);
//
// Display 32 bit Hex
//
void LCD_print_hex32(uint32_t val);
//
// Helper function to avoid the use of the math library
//
// Calculate powers of 10
//
int static pow10(uint8_t exp);
//
// Display 3 digit unsigned Decimal
//
// supports 8 bit unsigned values
//
void LCD_print_udec3(uint8_t val);
//
// Display 5 digit unsigned Decimal
//
// supports 16 bit unsigned values
//
void LCD_print_udec5(uint16_t val);
//
// Display 10 digit unsigned Decimal
//
// supports 32 bit unsigned values
//
void LCD_print_udec10(uint32_t val);
//
// Display 3 digit signed Decimal
//
// supports 8 bit signed values
//
void LCD_print_dec3(int8_t val);
//
// Display 5 digit signed Decimal
//
// supports 16 bit signed values
//
void LCD_print_dec5(int16_t val);
//
// Display 10 digit signed Decimal
//
// supports 32 bit signed values
//
void LCD_print_dec10(int32_t val);
//
// Helper function to avoid the use of the math library
//
// Calculate powers of 10 as a float
//
float static pow10f(int8_t exp);
//
// Display float in scientific notation
//
// x.xxxxExx
//
void LCD_print_float(float val);

//
// Change the contrast of the display
//
// This function maps an input value of 0 to 10 to word values between min and max
//
// 7 bits set Vop, but we don't know what limits the acceptable
// Vop voltage for this LCD. Limits were selected via trial and error.
//
// min word value is 30 - effectively LCD off
// max word value is 60
//
void LCD_contrast(uint8_t val);

#endif //__MSOE_LIB_LCD_H__
