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


// Function Prototypes
void adc_setup(void);
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
    adc_setup();
    initTimer();
    NVIC_setup();

    // Need to enable interrupts before program starts
    _enable_interrupts();

    // Start sampling/conversion
    ADC14->CTL0 |= 0x00000001; // start conversion

    //Local Variables

    while(1){ // Main while loop

        //Low power code will be here eventually
        //For now, do nothing because yeet.
    }
}

//========================================================================================================//
/*
 * Name: void adc_setup(void)
 * Description: Sets up the ADC for use with ADC14
 * Inputs: NA
 * Output: NA
 */
//========================================================================================================//
void adc_setup(void){
    // using - A3 = P5.2
    //
    // keep enable low while making changes
    //
    // ctrl0
    // /4 sc_bit timer no_inv /1 mod rptS x xxxx 16x cont xx on xx enb scb
    // 01 000 1 0 000 000 10 0 0000 0010 1 00 1 00 0 0
    ADC14->CTL0 = 0x44040290;
    // ctrl1
    // xxxx no_sel x mem5 xxxxxxxxxx 12b unsigned on reg
    // 0000 000000 0 00101 0000000000 10 0 0 00
    ADC14->CTL1 = 0x00050020;
    // mctl[5] - since using mem5
    // xxxx xxxx xxxx xxxx x enb diffb x AVCC x xx A3
    // 0000 0000 0000 0000 0 0 0 0 0000 0 00 00011
    ADC14->MCTL[5] = 0x00000003;
    // ier0 - enable interrupts on channel 5
    // 0000 0000 0000 0000 0000 0000 0010 0000
    ADC14->IER0 |= 0x00000020;
    // all others default
    // set enable
    ADC14->CTL0 |= 0x00000002;
    return;
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
    P5->DIR |= 0xFF;
    P6->DIR |= 0xFF;  P6->OUT = 0;
    P7->DIR |= 0xFF;  P7->OUT = 0;
    P8->DIR |= 0xFF;  P8->OUT = 0;
    P10->DIR |= 0xFF; P10->OUT = 0;

    P7->DIR |=  BIT7;
    P5->DIR &= ~BIT4;
    P2->DIR |=  0b111;
    P2->OUT &= ~0b111;

    // Setting used pins to corresponding values

    P5->SEL0 |= 0x04; // '11' pin functionality
    P5->SEL1 |= 0x04;
    P5->DIR |= BIT5; // output for P5.5

    // Timer
    P7->SEL1 &= ~BIT7; //Change pin functionality to TIMERA1
    P7->SEL0 |= BIT7;  //A1.1 output pin
    P7->DIR |= BIT7;   //Actual Output

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
void ADC14_IRQHandler(void){
    int val; // tmp variable - really not necessary
    val = ADC14->MEM[5];
    ADC14->CLRIFGR0 |= 0x00000020; // clr flg
    // need to put print in ISR otherwise it never
    // gets a chance to run
    if(val <= 300 ){
        // increment the number of revolutions that were measured.
        Revolutions++;
    }
    else{
        //Do Nothing
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
    RPM   = Revolutions*60*pi*Diameter; // calculates RPM
    Speed = RPM*60/5280;                // calculates Speed

    //call Zach's function to send the speed to the display
    //SendToDisplay(Speed);

    //Reset the revolutions variable
    Revolutions = 0;

    //Clear Flag
    clear = TIMER_A1->IV;
}
