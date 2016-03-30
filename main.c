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
    // Build Message Header
    msg->MessageID = CAN_RESPONSE;
    msg->EID = false;
    msg->RTR = false;
    msg->priority = LEVEL0;
    
    // Read Sensors
    HDC1000_ReadTempHumidity(&temperature, &humidity);
    pressure = LPS25HB_ReadPressureMillibars();
    
    // Build Data Packet
    uint16_t tmp = (uint16_t)(temperature * 10.0);
    msg->data[0] = tmp >> 8;    // Temperature High Bits
    msg->data[1] = tmp;         // Temperature Low Bits
    msg->data[2] = (uint8_t)(humidity);    // Humidity Bits
    tmp = (uint16_t)(pressure * 10.0);
    msg->data[3] = tmp >> 8;    // Pressure High Bits
    msg->data[4] = tmp;         // Pressure Low Bits
    
    // Set Message Length Field (DLC)
    msg->DLC = 5;
}

#ifdef TESTING
void ParseCanMessage(CAN_Message_t *msg) {
    uint16_t tmp;
    tmp = msg->data[0];                 // Temperature High Bits
    tmp = (tmp << 8) | msg->data[1];    // Temperature Low Bits
    can_temperature = tmp / 10.0;
    can_humidity = msg->data[2];
    tmp = msg->data[3];
    tmp = (tmp << 8) | msg->data[4];
    can_pressure = tmp / 10.0;
}

void BuildRequestMessage(CAN_Message_t *msg) {
    msg->MessageID = CAN_REQUEST;
    msg->priority = LEVEL0;
    msg->RTR = false;
    msg->EID = false;
    msg->DLC = 0;
}
#endif

void BoardInit() {
    // Initialize Peripherals
    PORTS_Init();
    I2CMaster_Init();
#ifdef TESTING
    CAN_Init(0, true); // CAN in Loopback Mode
#else
    CAN_Init(0, false);
#endif
    
    // Initialize Devices
    HDC1000_Init(0, DEGREES_F);
    LPS25HB_Init();
    
    // Enable Global Interrupts
    GlobalInterruptEnable();
}

// Main Function
void main(void) {
    
    BoardInit();
    
#ifdef TESTING
    // Send a CAN Request
    BuildRequestMessage(&sMessage);
    CAN_EnqueueMessage(&sMessage);
#endif
    
    while(1) {
        if(!CAN_RXQueueIsEmpty()) {
            CAN_DequeueMessage(&rMessage);
            if(rMessage.MessageID == CAN_REQUEST) { // Sensor Readings are Requested
                // Take Sensor Readings and Build CAN Message to Send
                BuildCanMessage(&sMessage);
                
                // Enqueue CAN Message to Be Sent
                CAN_EnqueueMessage(&sMessage);
            }
#ifdef TESTING
            // If we are in Testing mode, CAN Messages are received by this device too
            else if(rMessage.MessageID == CAN_RESPONSE) { // Sensor Readings are Requested
                ParseCanMessage(&rMessage);
            }
#endif
        }
    }
    
    return;
}
