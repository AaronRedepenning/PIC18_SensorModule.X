#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "T6Protocol.h"
#include "can.h"

#define T6_BROADCAST_ADDRESS    0xFF
#define T6_DEFAULT_ADDRESS      0x00

// Global Variables
uint8_t MACAddress[MAC_ADDRESS_LENGTH] = MAC_ADDRESS;
uint8_t CAN_Address;
CAN_Message_t RxMessage;
T6_Flags_t T6StatusFlags;

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

void T6Protocol_Init(uint8_t dipState, DeviceType dType) {
    CAN_Message_t DiscoverMsg;
    DiscoverMsg.DestinationAddress = T6_BROADCAST_ADDRESS;
    DiscoverMsg.SourceAddress = T6_DEFAULT_ADDRESS;
    DiscoverMsg.Command = T6_CMD_DISCOVER;
    DiscoverMsg.Data[0] = dipState;
    DiscoverMsg.Data[1] = dType;
    for(uint8_t i = 0; i < MAC_ADDRESS_LENGTH; i++) {
        DiscoverMsg.Data[i + 2] = MACAddress[i];
    }
    DiscoverMsg.DataLength = 8;
    
    CAN_Init(T6_CAN_BITRATE);
    
    // Enqueue Discovery message to be sent
    CAN_EnqueueMessage(&DiscoverMsg);
    T6StatusFlags.HasClaimedAddress = false;
    T6StatusFlags.HasRequestedAddress = true;
}

void T6Protocol_AppPoll() {
    if(!CAN_RXQueueIsEmpty()) {
        CAN_DequeueMessage(&RxMessage);
        
        switch(RxMessage.Command) {
            case T6_CMD_DISCOVER: {
                // Don't do anything for discovery message. This is only for
                // the host controller (server) to respond to
            }
            case T6_CMD_PROVIDE_ADDRESS: {
                // This could be our requested address. Check if we requested an
                // address and this provided address is for this device
                if(T6StatusFlags.HasRequestedAddress && CompareMACAddress(RxMessage.Data)) {
                    T6StatusFlags.HasClaimedAddress = true;
                    T6StatusFlags.HasRequestedAddress = false;
                    CAN_Address = RxMessage.Data[6];
                }
            }
            case T6_CMD_SAMPLE: {
                // Sensors Sample was requested, to send this use the callback
            }
            default: {
                
            }
        }
    }
}
