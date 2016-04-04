/* 
 * File:   can.h
 * Author: aaron
 *
 * Created on March 25, 2016, 8:23 PM
 */

#ifndef CAN_H
#define	CAN_H

#ifdef	__cplusplus
extern "C" {
#endif
    

// Data Types

#define CAN_MSG_LENGTH   5
#define CAN_DATA_LENGTH  8
    
union CAN_MESSAGE_UNION {
    struct {
        uint8_t DestinationAddress;
        uint8_t SourceAddress;
        uint8_t Reserved;
        uint8_t Command;
        unsigned int DataLength         : 4;
        unsigned int RTR                : 4;
        uint8_t Data[CAN_DATA_LENGTH];
    };
    uint8_t Array[CAN_MSG_LENGTH + CAN_DATA_LENGTH];
};
typedef union CAN_MESSAGE_UNION CAN_Message_t;

// Public API
void CAN_Init(long bitrate);
void CAN_DequeueMessage(CAN_Message_t *msg);
void CAN_EnqueueMessage(CAN_Message_t *msg);
bool CAN_TXQueueIsEmpty();
bool CAN_RXQueueIsEmpty();
void CAN_ISR();


#ifdef	__cplusplus
}
#endif

#endif	/* CAN_H */

