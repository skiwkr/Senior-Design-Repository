/*
 * msoe_lib_clk.h
 *
 *  Created on: Feb 18, 2017
 *      Author: johnsontimoj
 *
 *      Rev:    0.1     8/8/17      Added wait states for flash to
 *                                  support production silicon (red boards)
 *
 */
////////////////////////////////////////////
//
// Clock System Routines
//
////////////////////////////////////////////
#ifndef __MSOE_LIB_CLK_H__
#define __MSOE_LIB_CLK_H__
//
// Includes
// None required

////////////////////////////////////////////////////////////////////
//
// Clock_Init_48MHz
//
////////////////////////////////////////////////////////////////////
//
// The launchpad has a 48MHz crystal available
//
// To operate at 48MHz the processor must be operating at
// its highest core voltage - VCORE1
//
// To get to the highest core voltage we can use the LDO
// or a DC-DC converter. We will use the LDO.
//
// Enable the 48MHz crystal - HFXTCLK
// Enable REFOCLK at 38KHz
//
// Set master clock (MCLK) to HFXTCLK with divide by 1 - 48MHz
// Set high speed sub-system clock (HSMCLK) to HFXTCLK with divide by 2 - 24MHz (max allowed)
// Set low speed sub-system clock (SMCLK) to HFXTCLK with divide by 4 - 12MHz (max allowed)
// Set the auxiliary clock (ACLK) to REFOCLK - 32KHz
// Set the backup clock (BCLK) to REFOCLK - 32KHz
//
// Inputs: none
// Outputs: none
// Returns:	0 for success
//		1 for Power mode change failure
//		2 for Active mode failure
//		3 for VCORE1 node failure
//		4 for Power mode change failure
//		5 for HFXT clock stable failure
//		6 for "some" clock not working
//
int Clock_Init_48MHz(void);

////////////////////////////////////////////////////////////////////
//
// Clock_48MHz_Divide
//
////////////////////////////////////////////////////////////////////
//
// This routine modifies the output clock frequencies
// for the HFXT clock source (48MHz)
// Divide values from 1 to 128 in powers of 2
//
// This routine assumes the 48MHz clock has already been set up
// using Clock_Init_48MHz()
//
// Inputs: Divide value
// Outputs: none
// Returns: 0 for success
//          1 for Power mode change failure
//          2 for "some" clock not working
//
int Clock_48MHz_Divide(uint8_t divider);

/////////////////////////////////////////////////////////////////////////
#endif // __MSOE_LIB_CLK_C__

