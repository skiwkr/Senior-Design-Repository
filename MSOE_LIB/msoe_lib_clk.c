/*
 * msoe_lib_clk.c
 *
 *  Created on: Feb 18, 2017
 *      Author: johnsontimoj
 *
 *      Rev:    0.1     8/8/17      Added wait states for flash to
 *                                  support production silicon (red boards)
 */
#ifndef __MSOE_LIB_CLK_C__
#define __MSOE_LIB_CLK_C__
////////////////////////////////////////////
//
// Clock System Routines
//
////////////////////////////////////////////
//
// Includes
#include <stdint.h>
#include "msp432.h"
//
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
//      and
// The FLASH memory must be set to have 1 wait state
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
int Clock_Init_48MHz(void){
	// Local Variables
	// These values are used to timeout the transitions
	uint32_t PMR_fail = 100000;
	uint32_t AM_LDO_VCORE1_fail = 500000;
	uint32_t HFXT_fail = 100000;
	uint32_t CLK_fail = 100000;

	// Power mode cannot be changed until the Power Control Module (PCM)
	// is not active. Status is held in PCMCTL1 register bit 8 (PMR_BUSY)
	// Power mode can be changed when this bit is zero (idle)
	// Wait for  PMR to be idle
	while(PCM->CTL1 & 0x00000100){
		PMR_fail--;
		if(PMR_fail == 0)		// Attempt Failed - no changes made - return 1
			return 1;
	} // end while

	// The Power Control Module requires a special value (Key) to be written to
	// to change the power mode. Default mode does not support 48MHz operation.
	// Key is 0x695A_xxxx and loads into PCMCTL0
	//
	// Simultaneously set the PCM to active mode, LDO Vcore 1 (highest voltage)
	// to support 48MHz operation - PCMCTL0, bit 0 = high
	PCM->CTL0 = (PCM->CTL0 & ~0xFFFF000F) |   0x695A0000   |   0x00000001;
		//      Force 0's in key       force 1's in key   force mode to 1

	// There is a flag to indicate transition to active mode is NOT valid
	// PCMIFG bit 2 (AM_INVALID_TR_IFG)
	if(PCM->IFG & 0x00000004){
		PCM->CLRIFG = 0x00000004;		// Attempt failed - clear flag and return 2
        return 2;
	} // end if

	// Check the actual mode, and wait for AM LDO VCORE1 to be set
	// Status is in PCMCTL0, AM_LDO_VCORE1 mode is 0x01 on bits 13:8
	while((PCM->CTL0 & 0x00003F00) != 0x00000100){
		AM_LDO_VCORE1_fail--;
		if(AM_LDO_VCORE1_fail == 0)
			return 3;				// Attempt failed - return 3
	} // end while

	// Power mode change is complete
	// Clear the change key to prevent unintended changes
	PCM->CTL1 &= ~0xFFFF0000;

	// Clocks cannot be changed until the Power Control Module (PCM)
	// is not active. Status is held in PCMCTL1 register bit 8 (PMR_BUSY)
	// Clocks can be changed when this bit is zero (idle)
	// Wait for  PMR to be idle
	while(PCM->CTL1 & 0x00000100){
		PMR_fail--;
		if(PMR_fail == 0)		// Attempt Failed - return 4
			return 4;
	} // end while

	//  Use one wait state for flash to run at 48 MHz
	//  The wait state value is held in the FLCTL->BANKxRDCTL register
	//  in bits 15-12 for both bank 0 and bank 1
	//  Wait state = 1 requires a code of 0001 (0x1) in bits 15-12
	FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(0x0000F000)) | 0x00001000;
	FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(0x0000F000)) | 0x00001000;

	// The crystal oscillator pins default to HFXT mode but the oscillator is not active
	// until these pins are configured
	// Port J, bits 2,3 - set to select mode 01
	PJ->SEL0 |= 0x0C;
	PJ->SEL1 &= ~0x0C;

	// The Clock Source module requires a special value (Key) to be written to
	// to change the clock source. Default DCO mode does not support 48MHz operation.
	// Key is 0x695A (top bits are reserved) and loads into CSKEY
	CS->KEY = 0x695A;

	// 48MHz operation requires the following in CSCTL2
	//
	// HFXTBYPASS to be cleared (bit 25) - not bypassing the crystal
	// HFXTFREQ set to 110 (bits 22:20) - 40-48 MHz
	// HFXT_EN set to 1 (bit 24)
	// HFXTDRIVE set to 1 (bit 16) - based on HFXTDRIVE value
	// xxxx xx01 x110 xxx1 xxxx xxxx xxxx xxxx
	CS->CTL2 = (CS->CTL2 & ~0x02100000) | 0x01610000;
			//       force 0s			force 1s

	// Check to make sure HFXT mode is stable
	// The interrupt flag is set if there is a fault (not stable)
	// The flag will not clear itself so you need to clear the flag
	// and re-check - flag:CSIFG bit 1 (HFXTIFG), clearFlag:CSCLRIFG bit 1 (CLR_HFXTIFG)
	while(CS->IFG & 0x00000002){
		CS->CLRIFG = 0x00000002;
		HFXT_fail--;
		if(HFXT_fail == 0)		// Attempt Failed - return 5
			return 5;
	} // end while

	// Configure the system clocks (CSCTL1)
	//
	// MCLK --> HFXTCLK - bits 2:0 = 101
	// SMCLK/HSMCLK --> HFXTCLK - bits 6:4 = 101
	// ACLK --> REFOCLK - bits 10:8 = 010
	// BCLK --> REFOCLK - bit 12 = 1
	// MCLK /1 - bits 18:16 = 000
	// HSMCLK /2 - bits 22:20 = 001  (max allowed is 24MHz)
	// ACLK /1 - bits 26:24 = 000
	// SMCLK /4 - bits 30:28 = 010	 (max allowed is 12MHz
	// x010 x000 x001 x000 x001 x010 x101 x101
	CS->CTL1 = 0x20101255;		// direct write
//	CS->CTL1 = 0x20131255;		// direct write

	// Configure the system clocks
	//
	// CSCTL2, CSCTL3, CSCLKEN
	// Use default values

	// Clock mode change is complete
	// Clear the change key to prevent unintended changes
	CS->KEY = 0;

	// Make sure all clocks are working properly
	// Use the Status register
	// HFXT_ON, MCLK_ON, SMCLK_ON, HSMCLK_ON, ACLK_ON
	// MCLK_READY, HSMCLK_READY, SMCLK_READY, ACLK_READY
	// xxxx 1111 xxxx 1111 xxxx xxxx xxxx x1xx
	// normally would test: 0x0F020004
	//     but with no other peripherals enabled, only MCLK is on, test: 0x0F020004
	while((CS->STAT & 0x0F020004) != 0x0F020004){
		CLK_fail--;
		if(CLK_fail == 0)		// Attempt Failed - return 6
			return 6;			// some clock not working
	}
	//

	return 0;				// change completed
} // end Clock_Init_48MHz

////////////////////////////////////////////////////////////////////

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
// Returns:	0 for success
//			1 for Power mode change failure
//			2 for "some" clock not working
//
int Clock_48MHz_Divide(uint8_t divider){
	// Local Variables
	// These values are used to timeout the transitions
	uint32_t PMR_fail = 100000;
	uint32_t CLK_fail = 100000;

	// Clocks cannot be changed until the Power Control Module (PCM)
	// is not active. Status is held in PCMCTL1 register bit 8 (PMR_BUSY)
	// Clocks can be changed when this bit is zero (idle)
	// Wait for  PMR to be idle
	while(PCM->CTL1 & 0x00000100){
		PMR_fail--;
		if(PMR_fail == 0)		// Attempt Failed - return 1
			return 1;
	} // end while

	// The Clock Source module requires a special value (Key) to be written to
	// to change the clock source. Default DCO mode does not support 48MHz operation.
	// Key is 0x695A (top bits are reserved) and loads into CSKEY
	CS->KEY = 0x695A;

	// Configure the clock dividers (CSCTL1)
	//
	// MCLK /(divider) - bits 18:16
	// HSMCLK /(divider * 2) - bits 22:20  (max allowed is 24MHz)
	// ACLK  - bits 26:24
	// SMCLK /(divider * 4) - bits 30:28	 (max allowed is 12MHz)
	// x010 x000 x001 x000 x000 x010 x101 x101
	switch(divider){
	case 1:							// Mclk - HSMCLK - ACLK - SMCLK
		CS->CTL1 = 0x20100255;		// 48M-24M-32K-12M
		break;
	case 2:
		CS->CTL1 = 0x30210255;		// 24M-12M-32K-6M
		break;
	case 4:
		CS->CTL1 = 0x40320255;		// 12M-6M-32K-3M
		break;
	case 8:
		CS->CTL1 = 0x50430255;		// 6M-3M-32K-1.5M
		break;
	case 16:
		CS->CTL1 = 0x60540255;		// 3M-1.5M-32K-750K
		break;
	case 32:
		CS->CTL1 = 0x70650255;		// 1.5M-750M-32K-375K
		break;
	case 64:
		CS->CTL1 = 0x70760255;		// 750K-375K-32K-375K
		break;
	case 128:
		CS->CTL1 = 0x70770255;		// 375K-375K-32K-375K
		break;
	default:
		CS->CTL1 = 0x20100255;		// 48M-24M-32K-12M
	}

	// Clock mode change is complete
	// Clear the change key to prevent unintended changes
	CS->KEY = 0;

	// Make sure all clocks are working properly
	// Use the Status register
	// HFXT_ON, MCLK_ON, SMCLK_ON, HSMCLK_ON, ACLK_ON
	// MCLK_READY, HSMCLK_READY, SMCLK_READY, ACLK_READY
	// xxxx 1111 xxxx 1111 xxxx xxxx xxxx x1xx
	// normally would test: 0x0F020004
	//     but with no other peripherals enabled, only MCLK is on, test: 0x0F020004
	while((CS->STAT & 0x0F020004) != 0x0F020004){
		CLK_fail--;
		if(CLK_fail == 0)		// Attempt Failed - return 2
			return 2;			// some clock not working
	}
	//

	return 0;				// change completed
} // end Clock_48MHz_Divide

/////////////////////////////////////////////////////////////////////////
#endif // __MSOE_LIB_CLK_C__

