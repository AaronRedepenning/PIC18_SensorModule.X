/*
 * File:   main.c
 * Author: Aaron Redepenning
 *
 * Created on March 5, 2016, 4:04 PM
 */

#include "main.h"

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

void BoardInit() {
    // Initialize Peripherals
    PORTS_Init();
    I2CMaster_Init();
    
    // Initialize Sensors
    HDC1000_Init(0, DEGREES_F); // Temperature/Humidity
    LPS25HB_Init();             // Pressure
    
    // Initialize CAN and and the T6 Application
    T6Protocol_Init(0x00, THP_SENSOR);
    
    // Enable Global Interrupts
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
