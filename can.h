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
    typedef enum CAN_Msg_Priority {
        LEVEL0 = 0, // Lowest Priority
        LEVEL1 = 1,
        LEVEL2 = 2,
        LEVEL3 = 3  // Highest Priority
    } CAN_MSG_PRIORITY;
    
    typedef struct {
        uint32_t MessageID; // CAN Identifier Code
        bool EID;           // Extended Identifier
        bool RTR;           // Remote Transmit Request
        uint8_t data[8];    // Up to 8 bytes of data
        uint8_t DLC;        // Data Length Code
        CAN_MSG_PRIORITY priority; // Message priority level
    } CAN_Message_t;
    
    // Public API
    void CAN_Init(long bitrate, bool loopback);
    void CAN_DequeueMessage(CAN_Message_t *msg);
    void CAN_EnqueueMessage(CAN_Message_t *msg);
    bool CAN_TXQueueIsEmpty();
    bool CAN_RXQueueIsEmpty();
    void CAN_ISR();


#ifdef	__cplusplus
}
#endif

#endif	/* CAN_H */

