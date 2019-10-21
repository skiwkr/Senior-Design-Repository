/*
 * msoe_lib_misc.c
 *
 *  Created on: aug 15, 2017
 *      Author: johnsontimoj
 */
#ifndef __MSOE_LIB_MISC_C__
#define __MSOE_LIB_MISC_C__
////////////////////////////////////
//
// Miscellaneous code for MSOE library
//
//////////////////////////////////////
#include <stdio.h>
#include "msp432.h"

/////////////////////////////////////
//
// Set_ports_to_out(void)
//
// This code sets all the ports to outputs to prevent
// floating inputs
//
// Call this BEFORE any other code that modifies ports
//
//////////////////////////////////////
int Set_ports_to_out(void){
    P1->DIR |= 0xFF;
    P1->OUT = 0;
    P2->DIR |= 0xFF;
    P2->OUT = 0;
    P3->DIR |= 0xFF;
    P3->OUT = 0;
    P4->DIR |= 0xFF;
    P4->OUT = 0;
    P5->DIR |= 0xFF;
    P5->OUT = 0;
    P6->DIR |= 0xFF;
    P6->OUT = 0;
    P7->DIR |= 0xFF;
    P7->OUT = 0;
    P8->DIR |= 0xFF;
    P8->OUT = 0;
    P9->DIR |= 0xFF;
    P9->OUT = 0;
    P10->DIR |= 0xFF;
    P10->OUT = 0;
    return 0;
}

/////////////////////////////////////
//
// Stop_watchdog(void)
//
// This code turns off the watchdog timer
//
//////////////////////////////////////
int Stop_watchdog(void){
    WDT_A->CTL = 0x5A00 | 0x0080;    // password | timer stop
    return 0;
}

/////////////////////////////////////////////////////

#endif // __MSOE_LIB_MISC_C__
