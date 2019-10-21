/*
 * main.c
 *
 *  Created on: Oct 7, 2019
 *      Author: blockjm
 */
//Testing some sexy ass changes

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

void main(void){

    Clock_Init_48MHz();
    //Board Setup
    WDT_A->CTL =0x5A80; // Stop WatchDog timer

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

    // Diameter = 146mm or 0.479003 ft
    float Diameter = 0.479003;
    float Speed;


    while(1){ // Main while loop


     //Run timer to get revolutions per 10 seconds

     //Do math to get linear speed

     //Convert to mph

     //Send to Bluetooth IC


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
    return;
}

//========================================================================================================//
/*
 * Name: Interrupt handler
 * Description: handles the interrupt
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
    P5->OUT |= BIT5;
}
else{
    P5->OUT &= ~BIT5;
}

return;
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
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UPDOWN | TIMER_A_CTL_CLR |TIMER_A_CTL_ID__1;
    //Set EX Bit to 1 to have the N value = 1 since we want the biggest value
    TIMER_A0->EX0 |= TIMER_A_EX0_IDEX__1;
    TIMER_A0->CCTL[1] |= 0xC0; //Set output mode to toggle/set
    TIMER_A0-> CCR[0] = 3427;  //Set Top value
}

