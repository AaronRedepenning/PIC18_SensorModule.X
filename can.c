#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "can.h"

// Internal Defenitions
#define CAN_CONFIG_MODE         0x80
#define CAN_NORMAL_MODE         0x00
#define CAN_LOOPBACK_MODE       0x40

#define ECAN_SET_LEGACY_MODE    0x00
#define ECAN_SET_ENHANCED_MODE  0x40
#define ECAN_SET_FIFO_MODE      0xA0

#define BRGCON1_1MBIT           0x01
#define BRGCON2_1MBIT           0
#define BRGCON3_1MBIT           0

#define RX_QUEUE_SIZE           10
#define TX_QUEUE_SIZE           10

// Global Variables
CAN_Message_t   OneMessage;

uint8_t         RXHead;
uint8_t         RXTail;
uint8_t         RXQueueCount;
CAN_Message_t   RXQueue[RX_QUEUE_SIZE];

uint8_t         TXHead;
uint8_t         TXTail;
uint8_t         TXQueueCount;
CAN_Message_t   TXQueue[TX_QUEUE_SIZE];

bool TXIntsEnabled;


void CAN_SetBitrate(long bitrate);
void CAN_SetMode(uint8_t mode);
void CAN_SetAddressFilter(uint8_t address);
void CAN_TransmitMessages();
void CAN_ReceiveMessages();
bool CAN_SendOneMessage(CAN_Message_t *msg);
bool CAN_ReadOneMessage(CAN_Message_t *msg);

/*********************************************************************
CAN_Init

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_Init(long bitrate, bool loopback)
{
    // Initialize Queues
    RXHead = 0;
    RXTail = 0;
    RXQueueCount = 0;
    TXHead = 0;
    TXTail = 0;
    TXQueueCount = 0;
    
    // Force the CAN peripherial into Config mode
	CAN_SetMode(CAN_CONFIG_MODE);
	
	// Set the bitrate, currently set to 625kHz
	CAN_SetBitrate(bitrate);
	
	// Place the module in enhanced legacy mode
	ECANCON |= ECAN_SET_ENHANCED_MODE;
    
    // Disable filters, accept all messages
	RXB0CONbits.RXM1 = 1;
    RXB0CONbits.RXM1 = 1;
    
    // All additional buffers placed in transmit mode
    BSEL0 = 0b11111100;
    
    RXFCON0 = 0;
    RXFCON1 = 0;
    SDFLC = 0;
    
    MSEL0 = MSEL1 = MSEL2 = MSEL3 = 0xFF;
    
    CIOCON = 0b01100001;
    
    PIE5 = 0;
    
    if(loopback) {
        CAN_SetMode(CAN_LOOPBACK_MODE);
    }
    else {
        CAN_SetMode(CAN_NORMAL_MODE);
    }
    
    // Set Interrupt Bits
    TXIntsEnabled = false;
    PIE5bits.RXBnIE = 1;
    PIE5bits.IRXIE = 1;
    TXBIEbits.TX0IE = 1;
    BIE0bits.RXB0IE = 1;
}

/*********************************************************************
CAN_SendOneMessage

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
bool CAN_SendOneMessage(CAN_Message_t *msg)
{
    if(msg == NULL) return false;
    // CAN cannot send more than 8 bytes of data
    if(msg->DLC > 8) return false; 
    
    // Load Message Data
    TXB0D0 = msg->data[0];
    TXB0D1 = msg->data[1];
    TXB0D2 = msg->data[2];
    TXB0D3 = msg->data[3];
    TXB0D4 = msg->data[4];
    TXB0D5 = msg->data[5];
    TXB0D6 = msg->data[6];
    TXB0D7 = msg->data[7];
    
    if(msg->EID) {
        TXB0SIDH = (msg->MessageID >> 21);
        TXB0SIDL |= (msg->MessageID & 0x1C0000) >> 13;
        TXB0SIDL |= (msg->MessageID & 0x30000) >> 16;
        TXB0EIDH = (msg->MessageID >> 8);
        TXB0EIDL = (msg->MessageID);
        TXB0SIDLbits.EXIDE = 1;
    }
    else { // Standard CAN ID - 11 bits
        TXB0SIDH = (msg->MessageID >> 3);
        TXB0SIDLbits.SID = (msg->MessageID & 0x07);
        TXB0SIDLbits.EXIDE = 0;
    }
    
    // Remote Transmit Request Bit (RTR)
    TXB0DLCbits.TXRTR = msg->RTR ? 1 : 0;
    
    //Data Length Code Bits (DLC)
    TXB0DLCbits.DLC = msg->DLC; // Send 1 byte of data
    
    TXB0CONbits.TXPRI = msg->priority; // Set Message Priority
    TXB0CONbits.TXREQ = 1; // Request CAN Transmission
    
    while(TXB0CONbits.TXREQ) // Wait for transmission to complete
    { 
        
    } 
    // TODO: Enable intuerupts and callbacks on data TX and RX
    
    return true;
}

/*********************************************************************
CAN_ReadOneMessage

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
bool CAN_ReadOneMessage(CAN_Message_t *msg)
{
    // Check if there is a message to be read
    if(!RXB0CONbits.RXFUL) return false;
    
    if(msg == NULL) return false;
    
    if(RXB0SIDLbits.EID) { // Extended CAN ID - 29 bits
        msg->EID = true;
        msg->MessageID = RXB0SIDH;
        msg->MessageID = msg->MessageID << 5;
        msg->MessageID |= (RXB0SIDL & 0xE0) >> 3;
        msg->MessageID |= (RXB0SIDL & 0x03);
        msg->MessageID = (msg->MessageID << 8) | RXB0EIDH;
        msg->MessageID = (msg->MessageID << 8) | RXB0EIDL;
        // TODO: Finish getting EID from registers
    }
    else {  // Standard CAN ID - 11 bits
        msg->EID = false;
        msg->MessageID = RXB0SIDH;
        msg->MessageID = msg->MessageID << 3;
        msg->MessageID |= (RXB0SIDL & 0xE0) >> 5;
    }
    
    // Read Data Length Code
    msg->DLC = RXB0DLCbits.DLC;
    
    //Read All Data Registers
    msg->data[0] = RXB0D0;
    msg->data[1] = RXB0D1;
    msg->data[2] = RXB0D2;
    msg->data[3] = RXB0D3;
    msg->data[4] = RXB0D4;
    msg->data[5] = RXB0D5;
    msg->data[6] = RXB0D6;
    msg->data[7] = RXB0D7;
    
    // Check if Remote Transmit Request (RTR)
    msg->RTR = RXB0DLCbits.RXRTR ? true : false;
    
    // Clear full bit since data has been read
    RXB0CONbits.RXFUL = 0;
    
    // Message Priority N/A
    msg->priority = 0;
    
    return true;
}

/*********************************************************************
CAN_EnqueueMessage

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_EnqueueMessage(CAN_Message_t *msg)
{
    // Disable Interrupts while working with Queue
    PIE5bits.TXBnIE = 0;
    PIR5bits.TXBnIF = 0;
    
    if(TXQueueCount < TX_QUEUE_SIZE) {
        TXQueue[TXTail] = *msg;
        TXTail++;
        if(TXTail >= TX_QUEUE_SIZE) {
            TXTail = 0;
        }
        TXQueueCount++;
    }
    else {
        // Queue is Full
    }
    // Enable Interrupts
    PIE5bits.TXBnIE = 1;
    if(!TXIntsEnabled) {
        TXIntsEnabled = true;
        PIR5bits.TXBnIF = 1; // Throw Interrupt to TX CAN Message
    }
}

/*********************************************************************
CAN_DequeueMessage

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_DequeueMessage(CAN_Message_t *msg)
{
    // Disable Interrupts
    PIE5bits.RXBnIE = 0;
    
    if(RXQueueCount == 0) {
        // Queue is empty
    }
    else {
        // Remove Oldest Message
        *msg = RXQueue[RXHead];
        RXHead++;
        if(RXHead >= RX_QUEUE_SIZE) {
            RXHead = 0;
        }
        RXQueueCount--;
    }
    // Enable Interrupts
    PIE5bits.RXBnIE = 1;
}

/*********************************************************************
CAN_TXQueueIsEmpty

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
bool CAN_TXQueueIsEmpty() {
    return TXQueueCount == 0 ? true : false;
}

/*********************************************************************
CAN_RXQueueIsEmpty

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
bool CAN_RXQueueIsEmpty() {
    return RXQueueCount == 0 ? true : false;
}

/*********************************************************************
CAN_ISR

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_ISR(void) {
    if(PIR5bits.IRXIF) { // CAN Bus Error Message Received
        PIR5bits.IRXIF = 0;
        // Not Supported
    }
    if(PIR5bits.WAKIF) { // Bus Wake Interrupt
        PIR5bits.WAKIF = 0;
        // Not Supported
    }
    if(PIR5bits.ERRIF) { // CAN Module Error Interrupt
        PIR5bits.ERRIF = 0;
        // Not Supported
    }
    if(PIR5bits.TXBnIF) { // Transmit Interrupt
        PIR5bits.TXBnIF = 0;
        CAN_TransmitMessages();
    }
    if(PIR5bits.RXBnIF) { // Receive Interrupt
        PIR5bits.RXBnIF = 0;
        CAN_ReceiveMessages();
    }
}

/*********************************************************************
CAN_TransmitMessages

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_TransmitMessages() {
    // Remove Oldest Message from TXQueue
    if(TXQueueCount > 0) {
        CAN_SendOneMessage(&TXQueue[TXHead]);
        TXHead++;
        if(TXHead >= TX_QUEUE_SIZE) {
            TXHead = 0;
        }
        TXQueueCount--;
    }
    else {
        // TXQueue is Empty
        TXIntsEnabled = false;
        PIE5bits.TXBnIE = 0;
        PIE5bits.TXBnIE = 0;
        return;
    }
}

/*********************************************************************
CAN_ReceiveMessages

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_ReceiveMessages() {
    // Get the message
    CAN_ReadOneMessage(&OneMessage);
    
    // Place it in the Queue
    if(TXQueueCount < RX_QUEUE_SIZE) {
        RXQueue[RXTail] = OneMessage;
        RXTail++;
        if(RXTail >= RX_QUEUE_SIZE) {
            RXTail = 0;
        }
        RXQueueCount++;
    }
    else {
        // Message Dropped
    }
}

/*********************************************************************
CAN_SetBitrate

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
void CAN_SetBitrate(long bitrate)
{
    switch(bitrate)
    {
        case 625000: // 625kBit
        {
            BRGCON1 = BRGCON1_1MBIT;
            BRGCON2 = BRGCON2_1MBIT;
            BRGCON3 = BRGCON3_1MBIT;
            break;
        }
        default:
        {
            BRGCON1 = BRGCON1_1MBIT;
            BRGCON2 = BRGCON2_1MBIT;
            BRGCON3 = BRGCON3_1MBIT;
            break;
        }
    }
}

/*********************************************************************
SetECANMode

This routine sets the ECAN module to a specific mode.  It requests the
mode, and then waits until the mode is actually set.

Parameters:	unsigned char	ECAN module mode.  Must be either
							ECAN_CONFIG_MODE or ECAN_NORMAL_MODE
Return:		None
*********************************************************************/
void CAN_SetMode(uint8_t mode)
{
    CANCON = mode;
    while((CANSTAT & 0xE0) != mode);
}