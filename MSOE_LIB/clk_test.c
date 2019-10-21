/*
 * clk_test.c
 *
 *  Created on: Jun 5, 2017
 *      Author: Tim
 */

#include <stdio.h>
#include "msp432.h"
#include "msoe_lib_clk.h"

int main(void){
	int foo;

	// setup P4.1 as ACLK output
	// setup P4.2 as HSMCLK output
	// setup P4.3 as MCLK output
	// setup P7.0 as SMCLK output
	P4->DIR |= 0x1C;
	P4->SEL0 |= 0x1C;
	P4->SEL1 &= ~0x1C;
	P7->DIR |= 0x01;
	P7->SEL0 |= 0x01;
	P7->SEL1 &= ~0x01;

    printf("Status:\n");

	// initialize to 48MHZ
	foo = Clock_Init_48MHz();
	printf("return status: %i\n", foo);

	// reset to 6MHz so we can see on the analog discovery 2
	foo = Clock_48MHz_Divide(8);
	printf("return status: %i\n", foo);

	while(1){
		;
	}
	return 0;
}
