#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "T6Protocol.h"
#include "can.h"
#include "main.h"

#define T6_BROADCAST_ADDRESS    0xFF
#define T6_NULL_ADDRESS         0x00

// Global Variables
uint8_t MACAddress[MAC_ADDRESS_LENGTH] = MAC_ADDRESS;
uint8_t CAN_Address;
CAN_Message_t RxMessage;
CAN_Message_t TxMessage;
T6_Flags_t T6StatusFlags;

// Function Prototypes
void T6Protocol_RespondToSampleRequest(uint8_t reqAddr);

bool CompareMACAddress(uint8_t *OtherMAC) {
    int i;
    
    for(i = 0; (i < MAC_ADDRESS_LENGTH) && MACAddress[i] == OtherMAC[i]; i++);
    
    if(i == MAC_ADDRESS_LENGTH) {
        return true;
    }
    else {
        return false;
    }
}

void T6Protocol_Init(uint8_t dipState, bool hasCo2) {
    uint8_t index;
    
    // Build Network Discovery Message
    TxMessage.DestinationAddress = T6_BROADCAST_ADDRESS;
    TxMessage.SourceAddress = T6_NULL_ADDRESS;
    TxMessage.Command = T6CMD_DISCOVERY;
    TxMessage.Data[0] = dipState;
    TxMessage.Data[1] = (uint8_t)hasCo2;
    for(index = 0; index < MAC_ADDRESS_LENGTH; index++) {
        TxMessage.Data[index + 2] = MACAddress[index];
    }
    TxMessage.DataLength = 8;
    
    CAN_Init(T6_CAN_BITRATE);
    T6StatusFlags.HasRequestedAddress = true;
    T6StatusFlags.HasClaimedAddress = false;
    CAN_EnqueueMessage(&TxMessage);
}

void T6Protocol_AppPoll() {
    // If there is no message to process, just return
    if(CAN_RXQueueIsEmpty()) return;
    
    // Otherwise, get the message and process it
    CAN_DequeueMessage(&RxMessage);
    
    switch(RxMessage.Command) {
        case T6CMD_ASSIGN_ADDRESS:
            // Check if this is our new address
            if(CompareMACAddress(RxMessage.Data) && T6StatusFlags.HasRequestedAddress) {
                CAN_Address = RxMessage.Data[6];
                T6StatusFlags.HasClaimedAddress = true;
                T6StatusFlags.HasRequestedAddress = false;
            }
            break;
        case T6CMD_SAMPLE_REQUEST:
            // Call user defined Sample Request callback to get
            // data packet to send
            if(T6StatusFlags.HasClaimedAddress && (RxMessage.DestinationAddress == T6_BROADCAST_ADDRESS)) {
                T6Protocol_RespondToSampleRequest(RxMessage.SourceAddress);
            }
            break;
        case T6CMD_SENSOR_UPDATE:
        case T6CMD_DISCOVERY:
        case T6CMD_ERROR:
            // All of these messages are processed by the host controller,
            // so just ignore these messages
        default:
            // Message not recognized to throw it away
            break;
    }
}

static float pressure, temperature, humidity, co2;
void T6Protocol_RespondToSampleRequest(uint8_t reqAddr) {
    uint16_t util;
    
    // Read the temperature humidity sensor
    HIH8000_ReadHumidityTemperature(&temperature, &humidity);
            
    // Read the pressure sensor
    pressure = Adafruit_MPL3115A2_getPressure();
    
    // Sensor Readings Message
    TxMessage.DestinationAddress = reqAddr;
    TxMessage.SourceAddress = CAN_Address;
    TxMessage.Command = T6CMD_SENSOR_UPDATE;
    
    // Temperature Sample
    util = (uint16_t)(temperature * 10);
    TxMessage.Data[0] = (util >> 8) & 0x00FF;
    TxMessage.Data[1] = util & 0x00FF;
    
    // Humidity Sample
    TxMessage.Data[2] = (uint8_t)humidity;
    
    // Pressure Sample
    util = (uint16_t)(pressure * 10);
    TxMessage.Data[3] = (util >> 8) & 0x00FF;
    TxMessage.Data[4] = (util) & 0x00FF;
    
    TxMessage.DataLength = 5;
    
    CAN_EnqueueMessage(&TxMessage);
}
