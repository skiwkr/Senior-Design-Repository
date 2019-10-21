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
#ifndef __MSOE_LIB_LCD_C__
#define __MSOE_LIB_LCD_C__
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
//
// CODE
//
#include <stdio.h>
#include <float.h>
#include "msp432.h"
#include "msoe_lib_lcd.h"


///////////   Message write routines   ///////////////////////////
//
// 1) Wait for the SPI TX buffer to be empty indicating any previous transmit has completed
// 2) Set the D/C GPIO for Data or Command
// 3) Load the message into the SPI TX buffer - transmit starts as soon as the register is loaded
// note - TX only - no RX message to read
//
void static LCD_Data_WR(uint8_t data){
  while((EUSCI_A1->IFG & 0x0002) == 0x0000) // Wait for Tx Buffer to be empty
      ;
  P1->OUT |= 0x80;                          // Set D to 1 (data)
  EUSCI_A1->TXBUF = data;                   // load message and start transmit
  while((EUSCI_A1->IFG & 0x0002) == 0x0000) // Wait for Tx Buffer to be empty
        ;
}
void static LCD_Command_WR(uint8_t data){
  while((EUSCI_A1->IFG & 0x0002) == 0x0000) // Wait for Tx Buffer to be empty
      ;
  P1->OUT &= ~0x80;                         // Set D to 0 (command)
  EUSCI_A1->TXBUF = data;                   // load message and start transmit
  while((EUSCI_A1->IFG & 0x0002) == 0x0000) // Wait for Tx Buffer to be empty
        ;
}

///////////   Configuration routines   ///////////////////////////
//
// Port Configuration
//
void static LCD_Port_Config(void){
    // SPI and LCD port mapping
    // P3.3 – MOSI, P3.1 – SCLK, P3.0 – CS
    // P3.5 – RST, P3.4 – D/C

    P2->SEL0 |= 0x0B;       // 1x11 xxxx    P2.0, P2.1, and P2.3 as eUSCI (01) mode
    P2->SEL1 &= ~0x0B;      // automatically set to outputs

    P1->SEL0 &= ~0xC0;      // xxxx 00xx     P1.6 and P1.7 as GPIO (00) mode
    P1->SEL1 &= ~0xC0;

    P1->DIR |= 0xC0;        // xxxx 11xx     P1.6 and P1.7 outputs
}
//
// SPI Configuration
//
// P2.3 – MOSI, P2.1 – SCLK, P2.0 – CS
// see MSP432 documentation for SPI registers and configuration
//
void static LCD_SPI_Config(void){
    EUSCI_A1->CTLW0 = 0xAD83;   // 1010 ckph=1, ckpl=0, MSB first, 8 bit data
                                // 1101 master, active low enable, synchronous
                                // 10xx clock=SMCLK
                                // xx11 CS active, SW reset activated
    EUSCI_A1->MCTLW = 0;        // no modulation for SPI
    // With a 48MHz HFXTCLK clock – set SCLK to 4MHz for PCD8544
    EUSCI_A1->BRW = 0x03;       // 48MHz/4 -> 16MHz SMCLK /4 -> 4MHz SCLK
    EUSCI_A1->IE &= ~0x0003;    // disable interrupt creation
    EUSCI_A1->CTLW0 &= ~0x0001; // release SW reset
}
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
void LCD_Config(void){
    LCD_SPI_Config();
    LCD_Port_Config();

    P1->OUT &= ~0x40;           // reset LCD -  Active low
    int8_t i;
    for(i=0; i<4; i++)          // delay for approx 20 clock cycles
        ;
    P1->OUT |= 0x40;            // clear reset -  Active low

    // LCD needs to access the extended instruction set to program
    // the temp coef, bias system and VOP
    LCD_Command_WR(0x21);       // Function Set - 0010 0 PD V H
                                //     001 - active, horizontal entry mode, extended instruction set
    LCD_Command_WR(0x04);       // Temp coef - 0000 01 tc1 tc0
                                //     choose baseline - 00
    LCD_Command_WR(0x14);       // Bias subsystem - 0001 0 B2 B1 B0
                                //     4 is recommended in the spec
    LCD_Command_WR(0xB1);       // Set VOP - 1 op6-op0
                                //     larger numbers -> brighter display
    // LCD needs to access the regular instruction set to program
    // the LCD mode, x,y.data
    LCD_Command_WR(0x20);       // Function Set - 0010 0 PD V H
                                //     000 - active, horizontal entry mode, regular instruction set
    LCD_Command_WR(0x0C);       // Display control - 0000 1 D 0 E
                                //     DE=10 -> normal mode
    LCD_clear();                // Clear display
}

///////////   Location routines   ///////////////////////////
//
// Character X-Y location
//
// x,y are character locations (0-11 for x, 0-5 for y)
// Each character is 7 pixels (columns) wide so must multiply Column position by 7
//
void LCD_goto_xy(uint8_t x, uint8_t y){
    LCD_Command_WR(0x80 | (x * 7));     // 1 x6 x5 x4 x3 x2 x1 x0 : sets X location
    LCD_Command_WR(0x40 | y);           // 0100 0 y2 y1 y0 : sets Y location
}
//
// Home
//
// Set location to 0,0
void LCD_home(void){
    LCD_goto_xy(0,0);
}
//
// Character row location
//
// Set new character row value - keep existing column location
// Range is 0 to 5
void LCD_row(uint8_t row){
    LCD_Command_WR(0x40 | row);         // 0100 0 y2 y1 y0 : sets Y location
}
//
// Character column location
//
// Set new character column value - keep existing row location
// Range is 0 to 11
void LCD_col(uint8_t col){
    // Each character is 7 pixels (columns) wide so must multiply Column position by 7
    LCD_Command_WR(0x80 | (col * 7));   // 1 x6 x5 x4 x3 x2 x1 x0 : sets X location
}

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
void LCD_print_char(char val){
    LCD_Data_WR(0x00);      // First character column - blank

    int8_t i;
    for(i=0; i<5; i++)                      // cycle through the 5 character columns in the array
        LCD_Data_WR(ASCII[val - 0x20][i]);  // correct for 0x20 index offset

    LCD_Data_WR(0x00);      // Last character column - blank
}
//
// Display string
//
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_print_str(char *str_ptr){
    while(*str_ptr != 0){                   // check for end of string (ascii value 0)
        LCD_print_char((char)(*str_ptr));   // print the current dereferenced value
    str_ptr++;                              // advance to the next character
    } // end while
}
//
// Clear display
//
// Cycle through each 8-bit column location and write all blanks
// 6 banks x 84 columns = 504 8-bit column locations
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_clear(void){
    LCD_goto_xy(0,0);
    uint16_t i;
    for(i=0; i<503; i++)
        LCD_Data_WR(0x00);
}
//
// BMP display
//
// Display a 84x48 BMP file converted to an array

// Create an excel file with 84x48 cells
// fill any cells desired - then copy the area into Microsoft paint
// Save as a bmp file from Paint ...

// Use Microsoft Paint to convert any image to an appropriately sized bitmap file
// Use the Cypress program Bitmap_to_C.exe program to convert the bitmap to an array
// Include the array in your project

// 6 banks x 84 columns = 504 8-bit column locations
// The LCD controller automatically handles the wrap-around at the end of a row
void LCD_print_bmpArray(const char *bmpArray_ptr){
    LCD_goto_xy(0,0);
    uint16_t i;
    for(i=0; i<503; i++)
        LCD_Data_WR(bmpArray_ptr[i]);
}

///////////   Big character display routines   ///////////////////////////
//
// Display big character
//
// Cypress big characters are 8 pixels wide
// Cypress big characters are 18 pixels tall
// Display characters add 1 column of pixels either side of the Cypress character
// Characters are stored in the 3-D array ASCII_BIG and indexed (offset) by 0x20
// The RAM stores the data (1 byte) as a column 8 bits tall with the LSB at the top
// when in Horizontal addressing mode
// Since the characters span more than 1 row (bank) explicit control of the
// character location is required since there is no read capability on the controller
// This also means no wrapping
// Character indices are Row:0-2, Column:0-7
//
void LCD_print_bigchar (uint8_t x, uint8_t y, char val ){
    int8_t i;
    LCD_Command_WR(0x80 | (x * 10));        // set x/y location for upper half
    LCD_Command_WR(0x40 | (y * 2));
    LCD_Data_WR(0x00);                  // pad with 1 pixel space
    for(i=0; i<8; i++)                  // cycle through 8 character pixels
        LCD_Data_WR(ASCII_BIG[val - 0x20][i][0]);// correct offset and access upper row
    LCD_Data_WR(0x00);                  // pad with 1 pixel space
    LCD_Command_WR(0x80 | (x * 10));        // set x/y location for lower half
    LCD_Command_WR(0x40 | (y * 2 + 1));
    LCD_Data_WR(0x00);                  // pad with 2 pixel space
    for(i=0; i<8; i++)                  // cycle through 8 character pixels
        LCD_Data_WR(ASCII_BIG[val - 0x20][i][1]);// correct offset and access lower row
    LCD_Data_WR(0x00);                  // pad with 2 pixel space
}
//
// Display big string
//
void LCD_print_bigstr(uint8_t x, uint8_t y, char *str_ptr){
    while(*str_ptr != 0){                   // check for end of string (ascii value 0)
        if(x >= 8){                         // manage wrapping
            x=0;
            y++;
        }
        if(y == 3)
            y = 0;
        LCD_print_bigchar(x, y, (char)(*str_ptr));  // print the current dereferenced value
        str_ptr++;                              // advance to the next character
        x++;                                    // advance to next character location
    } // end while
}

///////////   Number display routines   ///////////////////////////
//
// Display binary
//
// Bitwise AND the value with each binary value then logical AND with 1
// Offset by 0x30 to convert to ASCII
void LCD_print_bin8(uint8_t val){
    int8_t i;
    for(i=8; i>0; i--)
        LCD_print_char((char)(val & (1 << (i-1)) && 1) + 0x30);
}
void LCD_print_bin16(uint16_t val){
    int i;
    for(i=16; i>0; i--)
        LCD_print_char((char)(val & (1 << (i-1)) && 1) + 0x30);
}
//
// Display 8 bit Hex
//
// Break the value into two nibbles
// check for 0-9 vs A-F and add appropriate ASCII offsets
// Output each nibble
void LCD_print_hex8(uint8_t val){
    uint8_t val_tmp;
    LCD_print_char('0');
    LCD_print_char('x');
    val_tmp = val >> 4;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = (val & 0x0F);
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
}
//
// Display 16 bit Hex
//
// Break the value into four nibbles
// check for 0-9 vs A-F and add appropriate ASCII offsets
// Output each nibble
void LCD_print_hex16(uint16_t val){
    uint16_t val_tmp;
    LCD_print_char('0');
    LCD_print_char('x');
    val_tmp = val >> 12;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x0FFF;
    val_tmp = val_tmp >> 8;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x00FF;
    val_tmp = val_tmp >> 4;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = (val & 0x0F);
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
}
//
// Display 32 bit Hex
//
// Break the value into eight nibbles
// check for 0-9 vs A-F and add appropriate ASCII offsets
// Output each nibble
void LCD_print_hex32(uint32_t val){
    uint32_t val_tmp;
    LCD_print_char('0');
    LCD_print_char('x');
    val_tmp = val >> 28;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x0FFFFFFF;
    val_tmp = val >> 24;
    printf("%x\n", val_tmp);
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x00FFFFFF;
    val_tmp = val_tmp >> 20;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x000FFFFF;
    val_tmp = val_tmp >> 16;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x0000FFFF;

    val_tmp = val_tmp >> 12;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x00000FFF;
    val_tmp = val_tmp >> 8;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = val & 0x000000FF;
    val_tmp = val_tmp >> 4;
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
    val_tmp = (val & 0x0000000F);
    if(val_tmp < 10)
        LCD_print_char(val_tmp + 0x30);
    else
        LCD_print_char(val_tmp + 0x37);
}
//
// Helper function to avoid the use of the math library
//
// Calculate POSITIVE powers of 10
//
int static pow10(uint8_t exp){
    int8_t i;
    int pow = 1;
    for(i=0; i<exp; i++)
        pow *= 10;
    return pow;
}
//
// Display 3 digit unsigned Decimal
//
// supports 8 bit unsigned values
//
void LCD_print_udec3(uint8_t val){
    int8_t i;
    uint8_t val_tmp = val;
    for(i=2; i>=0; i--){
        if(i == 0 && val == 0){
            LCD_print_char('0');
        }
        else if(val >= (pow10(i))){
            val_tmp = val_tmp / (pow10(i));
            LCD_print_char(val_tmp + 0x30);
        }
        else{
            LCD_print_char(' ');
        }
        val_tmp = val % (int)pow10(i);
    }
}
//
// Display 5 digit unsigned Decimal
//
// supports 16 bit unsigned values
//
void LCD_print_udec5(uint16_t val){
    int8_t i;
    uint16_t val_tmp = val;
    for(i=4; i>=0; i--){
        if(i == 0 && val == 0){
            LCD_print_char('0');
        }
        else if(val >= (pow10(i))){
            val_tmp = val_tmp / (pow10(i));
            LCD_print_char(val_tmp + 0x30);
        }
        else{
            LCD_print_char(' ');
        }
        val_tmp = val % (int)pow10(i);
    }
}
//
// Display 10 digit unsigned Decimal
//
// supports 32 bit unsigned values
//
void LCD_print_udec10(uint32_t val){
    int8_t i;
    uint32_t val_tmp = val;
    for(i=9; i>=0; i--){
        if(i == 0 && val == 0){
            LCD_print_char('0');
        }
        else if(val >= (pow10(i))){
            val_tmp = val_tmp / (pow10(i));
            LCD_print_char(val_tmp + 0x30);
        }
        else{
            LCD_print_char(' ');
        }
        val_tmp = val % (int)pow10(i);
    }
}
//
// Display 3 digit signed Decimal
//
// supports 8 bit signed values
//
void LCD_print_dec3(int8_t val){
    int8_t i;
    int8_t val_tmp =val;
    if(val < 0){
        LCD_print_char('-');
        val *= -1;
        val_tmp *= -1;
    }
    for(i=2; i>=0; i--){
        if(i == 0 && val == 0){
            LCD_print_char('0');
        }
        else if(val >= (pow10(i))){
            val_tmp = val_tmp / (pow10(i));
            LCD_print_char(val_tmp + 0x30);
        }
        else{
            LCD_print_char(' ');
        }
        val_tmp = val % (int)pow10(i);
    }
}
//
// Display 5 digit signed Decimal
//
// supports 16 bit signed values
//
void LCD_print_dec5(int16_t val){
    int8_t i;
    int16_t val_tmp = val;
    if(val < 0){
        LCD_print_char('-');
        val *= -1;
        val_tmp *= -1;
    }
    for(i=4; i>=0; i--){
        if(i == 0 && val == 0){
            LCD_print_char('0');
        }
        else if(val >= (pow10(i))){
            val_tmp = val_tmp / (pow10(i));
            LCD_print_char(val_tmp + 0x30);
        }
        else{
            LCD_print_char(' ');
        }
        val_tmp = val % (int)pow10(i);
    }
}
//
// Display 10 digit signed Decimal
//
// supports 32 bit signed values
//
void LCD_print_dec10(int32_t val){
    int8_t i;
    int32_t val_tmp = val;
    if(val < 0){
        LCD_print_char('-');
        val *= -1;
        val_tmp *= -1;
    }
    for(i=9; i>=0; i--){
        if(i == 0 && val == 0){
            LCD_print_char('0');
        }
        else if(val >= (pow10(i))){
            val_tmp = val_tmp / (pow10(i));
            LCD_print_char(val_tmp + 0x30);
        }
        else{
            LCD_print_char(' ');
        }
        val_tmp = val % (int)pow10(i);
    }
}
//
// Helper function to avoid the use of the math library
//
// Calculate powers of 10 as a float
//
float static pow10f(int8_t exp){
    int8_t i;
    float pow = 1.0;
    if(exp >= 0){
        for(i=0; i<exp; i++)
        pow *= 10;
    }else{
        for(i=0; i>exp; i--)
        pow /= 10;
    }
    return pow;
}
//
// Display float in scientific notation
//
// x.xxxxExx
//
void LCD_print_float(float val){
    int8_t exp = -38;
    uint16_t pow = 10000;
    if(val < 0){                                    // if negative print '-' and make positive
        val *= -1;
        LCD_print_char('-');
    }

    while(val >= pow10f(exp+1))                     // calculate exponent
        exp++;

    val = pow * val * pow10f(0 - exp);              // adjust value to xxxxx.xx form
    val += (pow * FLT_EPSILON * 10);                    // since we know magnitude of number - add epsilon*10 to correct for rounding

    int val_tmp = ((int)val);                       // now in form xxxxx
    uint8_t i;                                      // print 1st digit and decimal point
    val_tmp = val / pow;
    val -= val_tmp * pow;
    pow /= 10;
    LCD_print_char(val_tmp + 0x30);
    LCD_print_char('.');
    for(i=1; i<5; i++){                             // print remaining digits
        val_tmp = val / pow;
        val -= val_tmp * pow;
        pow /= 10;
        LCD_print_char(val_tmp + 0x30);
    }
    LCD_print_char('E');                            // print E for exponent and '-' if negative
    if(exp < 0){
        LCD_print_char('-');
        exp *= -1;
    }
    if(exp > 9)                                     // print exponent
        LCD_print_char((exp / 10) + 0x30);
    LCD_print_char((exp % 10) + 0x30);
}

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
void LCD_contrast(uint8_t val){
    // Calculate the word value
    uint8_t word_val;
    word_val = 30 + val * 0.1 * (60 - 30);  // max word value
                                            // automatically truncated to uint8_t
    word_val |= 0x80;                       // add the leading 1 required for programming

    // LCD needs to access the extended instruction set to program
    // the temp coef, bias system and VOP
    LCD_Command_WR(0x21);       // Function Set - 0010 0 PD V H
                                //     001 - active, horizontal entry mode, extended instruction set
    LCD_Command_WR(word_val);       // Set VOP - 1 op6-op0
                                //     larger numbers -> brighter display
    // LCD needs to access the regular instruction set to program
    // the LCD mode, x,y.data
    LCD_Command_WR(0x20);       // Function Set - 0010 0 PD V H
                                //     000 - active, horizontal entry mode, regular instruction set
    LCD_Command_WR(0x0C);       // Display control - 0000 1 D 0 E
                                //     DE=10 -> normal mode
}

#endif //__MSOE_LIB_LCD_C__
