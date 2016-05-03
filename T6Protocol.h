/* 
 * File:   T6Protocol.h
 * Author: Aaron Redepenning
 *
 * Created on April 11, 2016, 7:06 PM
 */

////////////////////////////////////////////////////////////////////////////////
// Global Defines, types, data
////////////////////////////////////////////////////////////////////////////////

#ifndef T6PROTOCOL_H
#define	T6PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// MAC Address
#define MAC_ADDRESS_LENGTH  6
#define MAC_ADDRESS         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// Bit-rates
#define BITRATE_500KHZ      500000
#define T6_CAN_BITRATE      BITRATE_500KHZ
    
// T6 Command Set
#define T6CMD_DISCOVERY         'D'
#define T6CMD_ASSIGN_ADDRESS    'A'
#define T6CMD_SAMPLE_REQUEST    'S'
#define T6CMD_SENSOR_UPDATE     'U'
#define T6CMD_ERROR             'E'

struct T6_STATUS_FLAGS {
    bool HasRequestedAddress;
    bool HasClaimedAddress;
};
typedef struct T6_STATUS_FLAGS T6_Flags_t;

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

void T6Protocol_Init(uint8_t dipStatus, bool hasCo2);
void T6Protocol_AppPoll();

#ifdef	__cplusplus
}
#endif

#endif	/* T6PROTOCOL_H */

