/* 
 * File:   T6Protocol.h
 * Author: aaron
 *
 * Created on April 11, 2016, 7:06 PM
 */

#ifndef T6PROTOCOL_H
#define	T6PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAC_ADDRESS_LENGTH  6
#define MAC_ADDRESS         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define BITRATE_500KHZ      500000

#define T6_CAN_BITRATE      BITRATE_500KHZ
    
// Command Set
#define T6_CMD_DISCOVER             'D'
#define T6_CMD_PROVIDE_ADDRESS      'A'
#define T6_CMD_SAMPLE               'S'

enum DEVICE_TYPE {
    INVALID     = 0x00,
    THP_SENSOR  = 0x01,
    THPC_SENSOR = 0x02
};
typedef enum DEVICE_TYPE DeviceType;

struct T6_STATUS_FLAGS {
    bool HasRequestedAddress;
    bool HasClaimedAddress;
};
typedef struct T6_STATUS_FLAGS T6_Flags_t;

void T6Protocol_Init(uint8_t dipStatus, DeviceType dType);
void T6Protocol_AppPoll();
    
// Callback for Sensor Sample Message

#ifdef	__cplusplus
}
#endif

#endif	/* T6PROTOCOL_H */

