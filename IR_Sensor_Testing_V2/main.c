/*
 * main.c
 *
 *  Created on: Oct 7, 2019
 *      Author: blockjm
 */

// Includes
#include <stdio.h>
#include "msp432.h"
#include "msoe_lib_all.h"
#include "functions.h"
#include "SevenSegment.h"


// Function Prototypes
void pin_setup(void);
void NVIC_setup(void);
void initTimer(void);

// Global Variables
float Revolutions = 0;
float Speed = 0;
float RPM = 0;
// Diameter = 146mm or 0.479003 ft
float Diameter = 0.479003;
float pi = 3.14159262;
int clear = 0;

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;  //Disable Watchdog Timer
    Clock_Init_48MHz();

    // Setup Function Calls
    pin_setup();
    initTimer();
    NVIC_setup();

    P5->OUT = 0b00000000;
    P6->OUT &= ~BIT1;


    // Need to enable interrupts before program starts
    _enable_interrupts();

    //Local Variables

    while(1){ // Main while loop

        //Low power code will be here eventually
        //For now, do nothing because yeet.

    }
}

//========================================================================================================//
/*
 * Name: Interrupt handler
 * Description: handles the interrupt for the IR sensor
 * Inputs: NA
 * Output: NA
 */
//========================================================================================================//
void PORT6_IRQHandler(void){
    int val; // tmp variable
    val = P6->IV;
    if(val |= 0x04 ){      // if P6.1 is reading a value of 1
        P5->OUT ^= BIT5;
        Revolutions++;     // increment the number of revolutions that were measured.
    }
}

//========================================================================================================//
/*
 * Name: void TA1_N_IRQHandler(void)
 * Description: Every second, we must read in the rps, convert to mph, and send that to the watch
 *              via Bluetooth.
 * Inputs: NA
 * Output: NA
 */
//========================================================================================================//
void TA1_N_IRQHandler(void){
    RPM   = Revolutions*60*pi*Diameter; // calculates RPM in feet/minute
    Speed = RPM*60/5280;                // calculates Speed in MPH

    //call Zach's function to send the speed to the display
    SendToDisplay(Speed);

    //Reset the revolutions variable
    Revolutions = 0;

    //Clear Flag
    clear = TIMER_A1->IV;
}
