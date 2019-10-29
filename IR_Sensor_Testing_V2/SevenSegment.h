/*
 * SevenSegment.c
 *
 *  Created on: Oct 24, 2019
 *      Author: seefeldzd
 */
#include "stdio.h"

int NumToBin[10] = {0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10011000};

void InitPins(void);
void SendToDisplay(float Speed){
    int DecimalDigit = (Speed - (int)Speed)*10;
    int TensDigit = (int)(Speed/10);
    int OnesDigit = ((int)Speed)%10;

    //printf("Input:%f, Tens:%i, Ones:%i, Decimal:%i\n", Speed, TensDigit, OnesDigit, DecimalDigit);
    P9->OUT = NumToBin[TensDigit];
    P7->OUT = NumToBin[OnesDigit];
    P4->OUT = NumToBin[DecimalDigit];
}
void InitPins(void) {

}
