/*
 * delay_test.c
 *
 *  Created on: aug 9, 2017
 *      Author: johnsontimoj
 *
 *      Version 1.1 - 9/7/2017
 *          Changed the 48MHz delay ms test to use longer delay values
 *      Version 1.2 - 7/23/2018
 *          Added bounds checking for delay inputs
 *          Removed calibration code (commented out for now)
 */

#include <stdio.h>
#include "msp432.h"
#include "msoe_lib_delay.h"
#include "msoe_lib_clk.h"

int main(void){
//	int foo;
	int i;

	// setup P6.1 as LED output
	P6->DIR |= 0x02;
	P6->SEL0 &= ~0x02;
	P6->SEL1 &= ~0x02;
	P6->OUT = 0x00; // use whole port - LAZY METHOD

	///////////////////////////////////////////////////
	//
	// Input checking test at 48MHz
	// must be commented out when not testing
	//
	//////////////////////////////////////////////////
//    Delay_48MHz_us(349524);
//    printf("349524 works");
//    Delay_48MHz_us(349525);
//    printf("349525 works");
//    Delay_48MHz_ms(65535);
//	  printf("65535 works");
//	  Delay_48MHz_ms(65536);
//	  printf("65536 works");
//    Delay_48MHz_sec(21);
//    printf("21 works");
//    Delay_48MHz_sec(22);
//    printf("22 works");

    ///////////////////////////////////////////////////
    //
    // Input checking test at 48MHz
    // must be commented out when not testing
    //
    //////////////////////////////////////////////////
//    Delay_3MHz_us(5529405);
//    printf("5529405 works");
//    Delay_3MHz_us(5529406);
//    printf("5529406 works");
//    Delay_3MHz_us(33);
//    printf("33 works");
//    Delay_3MHz_us(32);
//    printf("32 works");
//    Delay_3MHz_ms(5529);
//    printf("5529 works");
//    Delay_3MHz_ms(5530);
//    printf("5530 works");
//    Delay_3MHz_sec(255);
//    printf("255 works");
//    Delay_3MHz_sec(256);
//    printf("256 works");


//	///////////////////////////////////////////////////
//    //
//    // Delay tests at 3MHz
//    //
//    //////////////////////////////////////////////////
	for(i = 0; i < 10; i++){
	    Delay_3MHz_us(1000000);
	    P6->OUT = ~P6->OUT;
	}
	for(i = 0; i < 10; i++){
      Delay_3MHz_ms(200);
      P6->OUT = ~P6->OUT;
	}
	for(i = 0; i < 5; i++){
      Delay_3MHz_sec(5);
      P6->OUT = ~P6->OUT;
	}

///////////////////////////////////////////////////
//
// Generic Delay tests at 3MHz
//
//////////////////////////////////////////////////
//  for(i = 0; i < 10; i++){
//      Delay_us(1000000, 3000000);
//      P6->OUT = ~P6->OUT;
//  }
//  for(i = 0; i < 10; i++){
//      Delay_ms(200, 3000000);
//      P6->OUT = ~P6->OUT;
//  }
//  for(i = 0; i < 5; i++){
//      Delay_sec(5, 3000000);
//      P6->OUT = ~P6->OUT;
//  }

/////////////////////////////////////
//
// Delay tests at 48MHz
//
/////////////////////////////////////
//  initialize to 48MHZ
//	foo = Clock_Init_48MHz();
//
//  for(i = 0; i < 10; i++){
//      Delay_48MHz_us(300000);
//      P6->OUT = ~P6->OUT;
//  }
//  for(i = 0; i < 20; i++){
//      Delay_48MHz_ms(5000);
//      P6->OUT = ~P6->OUT;
//  }
//  for(i = 0; i < 5; i++){
//      Delay_48MHz_sec(5);
//      P6->OUT = ~P6->OUT;
//  }

///////////////////////////////////////////////////
//
// Generic Delay tests at 48MHz
//
//////////////////////////////////////////////////
//  initialize to 48MHZ
//  foo = Clock_Init_48MHz();
//
//  for(i = 0; i < 10; i++){
//      Delay_us(300000, 48e6);
//      P6->OUT = ~P6->OUT;
//  }
//  for(i = 0; i < 10; i++){
//      Delay_ms(200, 48e6);
//      P6->OUT = ~P6->OUT;
//  }
//
//    Can't delay for 1 sec at 48MHz without looping
//
    return 0;
}
