/*
 * msoe_lib_delay.c
 *
 *  Created on: aug 9, 2017
 *      Author: johnsontimoj
 *
 *      Version 1.1 - 9/7/2017
 *          Changed the 48MHz delay ms function to use a loop to increase flexibility
 *      Version 1.2 - 7/23/2018
 *          Removed some of the clk cycle added delay to account for small inputs
 */
#ifndef __MSOE_LIB_DELAY_C__
#define __MSOE_LIB_DELAY_C__
////////////////////////////////////////////
//
// Delay Routines
// us, ms, sec delay functions for 3Mhz and 48Mhz operation
// us, ms, sec delay function for parameterized frequency operation
//
////////////////////////////////////////////
//
// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "msp432.h"
//
////////////////////////////////////////////////////////////////////
//
// Delay_48MHz_us
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = 349,524
// maximum delay ~0.349s
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to 48MHz
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in us
// Outputs: none
//
int Delay_48MHz_us(uint32_t val){

	// input checking
    // max input is 349,524
    if (val > 349524){
        printf("\nDelay_48MHz_us delay out of bounds %i\n", val);
        exit(1);
    }

    // Local Variables
	// These values are used to timeout the transitions
	uint32_t delay_cnt;

	// Calculate the required number of clock cycles
	// 48MHz --> 20.8333ns/clk
	// # of clocks per us = 1e-6 * 48e6 = 48
	// required number of clocks = val * clks/us
	// Characterization has shown approx 50 clks used for overhead @48MHz
	//     subtract this from the delay count
	// removed the 50 clock correction to provide small delay values 7/23/18 tj
	delay_cnt = val * 48;

	// Setup the systick timer
	SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
	SysTick->LOAD = delay_cnt;
	SysTick->VAL = 0x0;

	// Wait
    while(!(SysTick->CTRL & 0x00010000))
	    ;

	// Done waiting
	return 0;
} // end Delay_48MHz_us

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_48MHz_ms
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = 65,535
// maximum delay 65,535 ms
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to 48MHz
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in ms
// Outputs: none
//
int Delay_48MHz_ms(uint16_t val){

    // input checking
    // max input is 65,535
    if (val > 65535){
        printf("\nDelay_48MHz_ms delay out of bounds %i\n", val);
        exit(1);
    }

    // Local Variables
    // These values are used to timeout the transitions
    uint16_t delay_cnt;
    uint16_t i;

    // Calculate the required number of clock cycles
    // 48MHz --> 20.8333ns/clk
    // # of clocks per 1ms = 1e-3 * 48e6 = 48000
    // required number of clocks = val * clks/1ms
    // Characterization has shown approx 50 clks used for overhead @48MHz
    //     subtract this from the delay count
    // removed the 50 clock correction to provide small delay values 7/23/18 tj
    delay_cnt = 48000;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
    SysTick->VAL = 0x0;

    // Wait
    for(i = 0; i < val; i++){
        SysTick->LOAD = delay_cnt;
        while(!(SysTick->CTRL & 0x00010000))
        ;
    }
    // Done waiting
    return 0;
} // end Delay_48MHz_ms

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_48MHz_sec
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = 21
// maximum delay 21s
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to 48MHz
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in sec
// Outputs: none
//
int Delay_48MHz_sec(uint8_t val){

    // input checking
    // max input is 21
    if (val > 21){
        printf("\nDelay_48MHz_sec delay out of bounds %i\n", val);
        exit(1);
    }

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;
    uint8_t i;

    // Calculate the required number of clock cycles
    // 48MHz --> 20.8333ns/clk
    // # of clocks per 100ms = 1e-1 * 48e6 = 4800000
    // required number of clocks = val * clks/100ms
    // Characterization has shown approx 50 clks used for overhead @48MHz
    //     subtract this from the delay count
    // removed the 50 clock correction to provide small delay values 7/23/18 tj
    delay_cnt = 4800000;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
//    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    for(i = 0; i < val*10; i++){
        SysTick->LOAD = delay_cnt;
        while(!(SysTick->CTRL & 0x00010000))
        ;
    }
    // Done waiting
    return 0;
} // end Delay_48MHz_sec

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_3MHz_us
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = 349,524
// maximum delay ~0.349s
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to 3MHz (default)
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in us
// Outputs: none
//
int Delay_3MHz_us(uint32_t val){
    /////////////////////////////////////
    // minimum input value = 33us
    // maximum input value = 5,529,405
    // maximum delay ~5.592s
    /////////////////////////////////////

    // input checking
    // max input is 5,529,405, min unput is 33
    if (val > 5529405){
        printf("\nDelay_3MHz_us delay out of bounds high %i\n", val);
        exit(1);
    }
    else if (val < 33){
        printf("\nDelay_3MHz_us delay out of bounds low %i\n", val);
        exit(1);
    }

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;

    // Calculate the required number of clock cycles
    // 3MHz --> 0.333us/clk
    // # of clocks per us = 1e-6 * 3e6 = 3
    // required number of clocks = val * clks/us
    // Characterization has shown approx 98 clks used for overhead @3MHz
    //     subtract this from the delay count
    // removed the 98 clock correction to provide small delay values 7/23/18 tj
    delay_cnt = val * 3;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    while(!(SysTick->CTRL & 0x00010000))
        ;

    // Done waiting
    return 0;
} // end Delay_3MHz_us

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_3MHz_ms
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = 5,529
// maximum delay ~5.529s
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to 3MHz (default)
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in ms
// Outputs: none
//
int Delay_3MHz_ms(uint32_t val){
    // input checking
    // max input is 5,529
    if (val > 5529){
        printf("\nDelay_3MHz_ms delay out of bounds %i\n", val);
        exit(1);
    }

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;

    // Calculate the required number of clock cycles
    // 3MHz --> 0.333us/clk
    // # of clocks per ms = 1e-3 * 3e6 = 3000
    // required number of clocks = val * clks/us
    // Characterization has shown approx 98 clks used for overhead @3MHz
    //     subtract this from the delay count
    // removed the 98 clock correction to provide small delay values 7/23/18 tj
    delay_cnt = 3000;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    while(!(SysTick->CTRL & 0x00010000))
        ;

    // Done waiting
    return 0;
} // end Delay_3MHz_ms

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_3MHz_sec
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = 255
// maximum delay 255s
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to 3MHz (default)
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in sec
// Outputs: none
//
int Delay_3MHz_sec(uint32_t val){
    // input checking
    // max input is 255
    if (val > 255){
        printf("\nDelay_3MHz_sec delay out of bounds high %i\n", val);
        exit(1);
    }

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;
    uint8_t i;

    // Calculate the required number of clock cycles
    // Using 1 sec as the base time
    // 3MHz --> 0.333us/clk
    // # of clocks per s = 1 * 3e6 = 3000000
    // required number of clocks = 1s * clks/us
    // Characterization has shown approx 98 clks used for overhead @3MHz
    //     subtract this from the delay count
    // removed the 98 clock correction to provide small delay values 7/23/18 tj
    delay_cnt = 3000000;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
//    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    for(i = 0; i < val; i++){
        SysTick->LOAD = delay_cnt;
        while(!(SysTick->CTRL & 0x00010000))
        ;
    }
    // Done waiting
    return 0;
} // end Delay_3MHz_ms

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_us
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = dependent on frequency
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to a known value
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in us
//          cpu clock frequency
// Outputs: none
//
int Delay_us(uint32_t val, uint32_t freq){

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;

    // Calculate the required number of clock cycles
    // freq  --> freq s/clk
    // # of clocks per us = 1e-6 * freq
    // required number of clocks = val * clks/us
    delay_cnt = val * (freq * 1e-6) ;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    while(!(SysTick->CTRL & 0x00010000))
        ;

    // Done waiting
    return 0;
} // end Delay_us

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_ms
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = dependent on frequency
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to a known value
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in ms
//          cpu clock frequency
// Outputs: none
//
int Delay_ms(uint32_t val, uint32_t freq){

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;

    // Calculate the required number of clock cycles
    // freq  --> freq s/clk
    // # of clocks per ms = 1e-3 * freq
    // required number of clocks = val * clks/ms
    delay_cnt = val * (freq * 1e-3) ;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    while(!(SysTick->CTRL & 0x00010000))
        ;

    // Done waiting
    return 0;
} // end Delay_ms

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Delay_sec
//
////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////
// maximum input value = dependent on frequency
/////////////////////////////////////
//
// This assumes the MSP432 has been programmed to a known value
//
// Uses the systick timer to create a delay where nothing else happens
//      No interrupts
//      CPU clk as the clk source (only real choice)
//
// Set systick control to no interrupts, cpu clock, enable
// Set the reload value register to the desired clock count to create delay
// Continuously read the control register for countflag
// No action on the calibration register
//
// Inputs: delay time in sec
//          cpu clock frequency
// Outputs: none
//
int Delay_sec(uint32_t val, uint32_t freq){

    // Local Variables
    // These values are used to timeout the transitions
    uint32_t delay_cnt;

    // Calculate the required number of clock cycles
    // freq  --> freq s/clk
    // # of clocks per sec =  freq
    // required number of clocks = val * clks/sec
    delay_cnt = val * (freq) ;

    // Setup the systick timer
    SysTick->CTRL = 0x0005;     // set clk to CPU clk (bit 2) and enable (bit 0)
    SysTick->LOAD = delay_cnt;
    SysTick->VAL = 0x0;

    // Wait
    while(!(SysTick->CTRL & 0x00010000))
        ;

    // Done waiting
    return 0;
} // end Delay_sec

////////////////////////////////////////////////////////////////////

#endif // __MSOE_LIB_DELAY_C__

