/*
 * msoe_lib_delay.h
 *
 *  Created on: aug 9, 2017
 *      Author: johnsontimoj
 *
 *      Version 1.1 - 9/7/2017
 *          Changed the 48MHz delay ms function to use a loop to increase flexibility
 *      Version 1.2 - 7/23/2018
 *          Removed some of the clk cycle added delay to account for small inputs*
 */
#ifndef __MSOE_LIB_DELAY_H__
#define __MSOE_LIB_DELAY_H__
////////////////////////////////////////////
//
// Delay Routines
// us, ms, sec delay functions for 3Mhz and 48Mhz operation
// us, ms, sec delay function for parameterized frequency operation
//
////////////////////////////////////////////
//
// Includes

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
int Delay_48MHz_us(uint32_t val);
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
int Delay_48MHz_ms(uint16_t val);
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
int Delay_48MHz_sec(uint8_t val);
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
int Delay_3MHz_us(uint32_t val);
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
int Delay_3MHz_ms(uint32_t val);
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
int Delay_3MHz_sec(uint32_t val);
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
int Delay_us(uint32_t val, uint32_t freq);
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
int Delay_ms(uint32_t val, uint32_t freq);
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
int Delay_sec(uint32_t val, uint32_t freq);
////////////////////////////////////////////////////////////////////

#endif // __MSOE_LIB_DELAY_H__

