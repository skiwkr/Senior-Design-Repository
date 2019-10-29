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
 * Name: void pin_setup(void)
 * Description: Sets up the pin output for use
 * Inputs: NA
 * Output: NA
 */
//========================================================================================================//
void pin_setup(void){
// input pin - A0
//
    //Setting all ports as outputs so that they aren't floating inputs

    P1->DIR |= 0xFF;  P1->OUT = 0;
    P2->DIR |= 0xFF;  P2->OUT = 0;
    P3->DIR |= 0xFF;  P3->OUT = 0;
    P4->DIR |= 0xFF;  P4->OUT = 0;
    P5->DIR |= 0xFF;  P5->OUT = 0;
    P6->DIR |= 0xFF;  P6->OUT = 0;
    P7->DIR |= 0xFF;  P7->OUT = 0;
    P8->DIR |= 0xFF;  P8->OUT = 0;
    P9->DIR |= 0xFF;  P9->OUT = 0;
    P10->DIR |= 0xFF; P10->OUT = 0;

    P7->DIR |=  BIT7;
    P5->DIR &= ~BIT4;
    P2->DIR |=  0b111;
    P2->OUT &= ~0b111;

    // Setting used pins to corresponding values
    // ADC functionality, no longer using
    //P5->SEL0 |= 0x04; // '11' pin functionality
    //P5->SEL1 |= 0x04;
    P5->DIR |= BIT5; // output for P5.5

    // Timer
    P7->SEL1 &= ~BIT7; //Change pin functionality to TIMERA1
    P7->SEL0 |= BIT7;  //A1.1 output pin
    P7->DIR |= BIT7;   //Actual Output

    // Input pin for IR receiver
    P6->SEL0 &= ~BIT0;
    P6->SEL1 &= ~BIT0;

    P6->DIR &= ~BIT(1); //Sets P6 BIT1 to an input
    P6->IES |= (BIT1);             // Falling edge interrupt maybe
    P6->IE |= BIT1;                 //Interrupt Enable
    NVIC->ISER[1] |= BIT(PORT6_IRQn-32); //enable for 6 interrupt

    return;
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
 * Name: void NVIC_setup(void)
 * Description: Sets up the NVIC for interrupt use on ADC14
 * Inputs: NA
 * Output: NA
 */
//========================================================================================================//
void NVIC_setup(void){
// setup NVIC
// Enable ADC
// Note: ADC is INTISR(24)
NVIC->IP[24] |= 0x20; // Set a priority
NVIC->ISER[0] |= 0x01000000; // ISER0 starts at 0
return;
}

//========================================================================================================//
/*
 * Name: void initTimer(void)
 * Description: creates a timer that will count every second
 * Inputs: NA
 * Output: NA
 */
//========================================================================================================//
void initTimer(void){
    //Set up SMCLK (12,000,000), Set mode to UpDown, Clear timer, Set ID bit to 1
    TIMER_A1->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR |TIMER_A_CTL_ID__8;
    //Set EX Bit to 8 to have the N value = 8
    TIMER_A1->EX0 |= TIMER_A_EX0_IDEX__8;

    //Set output mode to toggle and enable interrupt
    TIMER_A1->CCTL[1] |= TIMER_A_CCTLN_OUTMOD_4 | TIMER_A_CCTLN_CCIE;

    //Set Top value 1Hz square wave
    TIMER_A1-> CCR[0] = 46875;
    TIMER_A1-> CCR[1] = 46875;

    NVIC->ISER[0] |= 0b100000000000; //Interrupt Enable
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
