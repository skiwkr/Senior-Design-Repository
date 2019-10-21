/*
 * msoe_lib_misc.h
 *
 *  Created on: aug 15, 2017
 *      Author: johnsontimoj
 */
#ifndef __MSOE_LIB_MISC_H__
#define __MSOE_LIB_MISC_H__
////////////////////////////////////
//
// Miscellaneous code for MSOE library
//
//////////////////////////////////////

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
int Set_ports_to_out(void);

/////////////////////////////////////
//
// Stop_watchdog(void)
//
// This code turns off the watchdog timer
//
//////////////////////////////////////
int Stop_watchdog(void);

/////////////////////////////////////////////////////

#endif // __MSOE_LIB_MISC_H__
