/*
 * File:   main.c
 * Author: Aaron Redepenning
 *
 * Created on March 5, 2016, 4:04 PM
 */

#include "main.h"

// Defines
#define TESTING
#define CAN_REQUEST     0xFF
#define CAN_RESPONSE    0xAA

// Global Variables
#ifdef TESTING
static float can_temperature = 0;
static float can_humidity = 0;
static float can_pressure = 0;
#endif

static float temperature = 0;
static float humidity = 0;
static float pressure = 0;

static CAN_Message_t rMessage;
static CAN_Message_t sMessage;

// Interrupt Handlers
void interrupt InterruptVector( void )
{
    INTCONbits.GIE = 0;
    if(PIE5 != 0x00) {
        CAN_ISR();
    }
    INTCONbits.GIE = 1;
    return;
}

// Global Functions
void GlobalInterruptEnable() {
    // Enable Interrupts Globally
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}

void BuildCanMessage(CAN_Message_t *msg) {
    uint16_t tmp;
    
    // Build Message Header
    msg->DestinationAddress = 0x00;
    msg->SourceAddress = 0x01;
    msg->Command = 0x1A;
    
    // Read Sensors
    HDC1000_ReadTempHumidity(&temperature, &humidity);
    pressure = LPS25HB_ReadPressureMillibars();
    
    // Build Data Packet
    tmp = temperature * 10.0;
    msg->Data[0] = tmp >> 8;    // Temperature High Bits
    msg->Data[1] = tmp;         // Temperature Low Bits
    msg->Data[2] = humidity;    // All Humidity Bits
    tmp = pressure * 10.0;
    msg->Data[3] = tmp >> 8;    // Pressure High Bits
    msg->Data[4] = tmp;         // Pressure Low Bits
    
    // Set Data Length
    msg->DataLength = 5;
}

void BuildNetworkMessage(CAN_Message_t *msg) {
    msg->DestinationAddress = 0x00;
    msg->SourceAddress = 0x01;
    msg->Command = 0x01; // I'm here message
    msg->DataLength = 0;
}

void BoardInit() {
    // Initialize Peripherals
    PORTS_Init();
    I2CMaster_Init();
    CAN_Init(500000); // CAN in Normal Mode, 500kBit/s
    
    // Initialize Devices
    HDC1000_Init(0, DEGREES_F);
    LPS25HB_Init();
    
    // Enable Global Interrupts
    GlobalInterruptEnable();
}

// Main Function
void main(void) {
    
    BoardInit();
    
    // Network Connect Message
    BuildNetworkMessage(&sMessage);
    CAN_EnqueueMessage(&sMessage);
    
    while(1) {
        
        if(!CAN_RXQueueIsEmpty()) {
            CAN_DequeueMessage(&rMessage);
            // TODO: Check Message Contents
            BuildCanMessage(&sMessage);
            CAN_EnqueueMessage(&sMessage);  
        }
    }
    
    return;
}
