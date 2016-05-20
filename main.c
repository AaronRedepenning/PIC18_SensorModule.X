/*
 * File:   main.c
 * Author: Aaron Redepenning
 *
 * Created on March 5, 2016, 4:04 PM
 */

#include "main.h"

////////////////////////////////////////////////////////////////////////////////
// Global Variables
////////////////////////////////////////////////////////////////////////////////

// Interrupt Handler for CAN Messages
void interrupt InterruptVector( void )
{
    INTCONbits.GIE = 0;
    if(PIE5 != 0x00) {
        CAN_ISR();
    }
    INTCONbits.GIE = 1;
    return;
}

// System and Board Functions
void GlobalInterruptEnable() {
    // Enable Interrupts Globally
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}

// Returns the state of the DIP switches for height configuration
uint8_t getDipState() {
    return (PORTB & 0x1F);
}

//
void BoardInit() {
    // Initialize Peripherals
    PORTS_Init();
    I2CMaster_Init();
    
    // 2 seconds delay for I2C bus to setup
    for(int i = 0; i < 100; i++) {
        __delay_ms(20);
    }
    
    // Initialize the pressure sensor
    HIH8000_Init();
    Adafruit_MPL3115A2_Init();
    
    // Power on delay for 5 seconds
    for(int i = 0; i < 500; i++) {
        __delay_ms(20);
    }
    
    // Initialize CAN and and the T6 Application
    uint8_t dip = getDipState();
    T6Protocol_Init(dip, false);
    
    // Enable Global Interrupts - required for CAN communications
    GlobalInterruptEnable();
}

// Main Function
void main(void) {
    BoardInit();

    while(1) {
        T6Protocol_AppPoll();
    }
    
    return;
}
