/*
 * msoe_lib_lcd_alt_io.c
 *
 *  Created on: May 22, 2017
 *      Author: johnsontimoj, widder
 *
 *      Rev:    0.1     5/24/17     added operational comments
 *      Rev:    0.2     8/8/17      added code to wait for SPI to complete
 *                                      on Command and Data writes to the LCD
 *                                  Fixed a bug with decimal printing to allow
 *                                      for printing the value 0
 *                                  Added LCD_print_hex32 for printing 32bit hex
 *      Rev:    0.3     9/13/17     Add a function to change the contrast LCD_contrast
 *      Rev:    1.0     1/21/18     new file to change UARTs for students with bad pins
 *                                      uses UART2 - widder
*       Rev:    1.1     7/23/18     name change
 *
 */

#ifndef MSOE_LIB_LCD_ALT_IO_H_
#define MSOE_LIB_LCD_ALT_IO_H_
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
//    display driver signals
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
//      b b111b b
//      b 1bbb1 b       {0x7e, 0x11, 0x11, 0x11, 0x7e}  // A
//      b 1bbb1 b
//      b 1bbb1 b       7e - 1st column with LSB as top pixel - 0111 1110
//      b 11111 b       11 - 2nd column with LSB as top pixel - 0001 0001
//      b 1bbb1 b
//      b 1bbb1 b
//      b bbbbb b
//
////////////////////////////////////////////////////////////////
//
// Nokia 5110 LCD Module / MSP432 Hardware Configuration (pinout)
//        ***  Alternate UART  ***
//
// VCC                      pin 1   3.3V
// GND                      pin 2   GND
// SCE (Slave Select)       pin 3   P2.0    output
// RST                      pin 4   P1.6    output      Active LOW
// D/C (data/commandBar)    pin 5   P1.7    output
// D/N (MOSI)               pin 6   P2.3    output
// SCLK                     pin 7   P2.1    output
// LED (back light)         pin 8   N/C
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
//      NOP             -   do nothing
//          0000 0000
//      Function Set    -   power down control; entry mode; extended instruction set control (H)
//          0010 0 PD V H
//              PD  - power down    :   0 = power on, 1 = power off
//              V   - addressing    :   0 = Horz, 1 = vert (we use Horz)
//              H   - inst set      :   0 = Basic, 1 = Extended
//      Write Data      -   write data to the DDRAM
//          D7 D6 D5 D4 D3 D2 D1 D0
//
// Basic Instructions:
//      Display Control     -   sets display configuration
//          0000 1 D 0 E
//              DE  - 00 = display blank, 01 = normal mode
//                    10 = all on, 11 = inverse video mode
//      Y address of DDRAM  -   sets Y address of DDRAM (bank 0 to 6)
//          0100 0 Y2 Y1 Y0
//      X address of DDRAM  -   sets X address of DDRAM (column 0 to 83)
//          1 X6 X5 X4 X3 X2 X1 X0
//
// Extended Instructions:
//      Temp Control        -   select the temperature control curve
//          0000 01 TC1 TC0
//      Bias System         -   set the bias system value
//          0001 0 BS2 BS1 BS0
//              BS2-BS0 - not well documented but value of 4 recommended in the spec
//      Set Vop             -   select the LCD operating voltage
//          1 Vop6 Vop5 Vop4 Vop3 Vop2 Vop1 Vop0
//              Vop6-Vop0 - not well documented, Sparkfun suggests 0x31 for the 5110
//
//////////////////////////////////////////////////////////////////////////


#endif /* MSOE_LIB_LCD_ALT_IO_H_ */
