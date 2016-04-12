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

#define BRGCON1_1MBIT           0x00
#define BRGCON2_1MBIT           0xA8
#define BRGCON3_1MBIT           0x01
#define BRGCON1_625kBIT         0x01
#define BRGCON2_625kBIT         0x98
#define BRGCON3_625kBIT         0x01
#define BRGCON1_500kBIT         0x01
#define BRGCON2_500kBIT         0xA8
#define BRGCON3_500kBIT         0x01

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

CAN_Flags_t CanFlags;

void CAN_SetBitrate(long bitrate);
void CAN_SetMode(uint8_t mode);
void CAN_SetAddressFilter(uint8_t address);
void CAN_TransmitMessages();
void CAN_ReceiveMessages();
bool CAN_SendOneMessage(CAN_Message_t *msg);
bool CAN_ReadOneMessage(CAN_Message_t *msg);

/*********************************************************************
CAN_Init

This Function initializes the CAN bus with a specified bit rate. Once
 the CAN bus is set up it will transmit a network Discovery command
 to get a CAN address from the Host Controller.

Parameters:	bitrate - The bitrate the CAN bus is using
            
Return:		None
*********************************************************************/
void CAN_Init(long bitrate)
{
    // Initialize Transmit and Receive Queues
    RXHead = 0;
    RXTail = 0;
    RXQueueCount = 0;
    TXHead = 0;
    TXTail = 0;
    TXQueueCount = 0;
    
    // Force the CAN peripheral into configuration mode
	CAN_SetMode(CAN_CONFIG_MODE);
	
	// Set the desired bit rate
	CAN_SetBitrate(bitrate);
	
	// Place the module into enhanced FIFO mode
	ECANCON |= ECAN_SET_FIFO_MODE;
    
    // Disable filters and receive all messages in RX buffers
	RXB0CONbits.RXM1 = RXB1CONbits.RXM1 = 1;
    B0CONbits.RXM1 = B1CONbits.RXM1 = B2CONbits.RXM1 = B3CONbits.RXM1 = 1;
    B4CONbits.RXM1 = B5CONbits.RXM1 = 1;
    
    // All programmable buffers are configured as RX buffers
    BSEL0 = 0xFC;
    
    // Disable all filters
    RXFCON0 = 0;
    RXFCON1 = 0;
    SDFLC = 0;
    
    // No Masks
    MSEL0 = MSEL1 = MSEL2 = MSEL3 = 0xFF;
    
    // Configure ECAN module to use crystal oscillator
    CIOCON = 0x01;
    
    // Clear all interrupt flags
    PIR5 = 0;
    
    // Enable CAN interrupts on RX and TX
    CanFlags.TXIntsEnabled = false;
    PIE5bits.RXBnIE = 1;    // RX Interrupt Enable
    PIE5bits.IRXIE = 1;     // Error Message Interrupt Enable
    TXBIEbits.TX0IE = 1;    // TX Interrupt Enable
    BIE0 = 0xFF;            // RX Interrupt Enable for Programmable Buffers
    
    // Put ECAN module in normal mode to start receiving/transmitting messages
    CAN_SetMode(CAN_NORMAL_MODE);
    //CAN_SetMode(CAN_LOOPBACK_MODE);
}

/*********************************************************************
CAN_SendOneMessage

This Function sends a single message over the CAN bus. It gets called
 by CAN_TransmitMessages(), and is passes a single CAN message to send.

Parameters:	msg   - pointer to the CAN message to send  
Return:		true  - Send was a success
            false - Send was a failure
*********************************************************************/
#define ENABLE_EID      0x08
#define MAPPED_SIDH     RXB0SIDH
#define MAPPED_CONbits  RXB0CONbits
#define MAPPED_TREQ     RXRTRRO

bool CAN_SendOneMessage(CAN_Message_t *msg) {
    unsigned char *pReg;
    uint8_t loop;
    
    // Enable Extended IDs and Check Message Length
    msg->Reserved = ENABLE_EID;
    if(msg->DataLength > 8) {
        msg->DataLength = 8;
    }
    msg->RTR = 0; // Not used
    
    // Ensure last TX has completed before sending another
    while(MAPPED_CONbits.MAPPED_TREQ);
    
    // Load data into the TX buffers
    pReg = &MAPPED_SIDH;
    for(loop = 0; loop < CAN_MSG_LENGTH + msg->DataLength; loop++, pReg++) {
        *pReg = msg->Array[loop];
    }
    
    // Send Message
    MAPPED_CONbits.MAPPED_TREQ = 1;
    
    return true;
}

/*********************************************************************
CAN_ReadOneMessage

This Function reads a single message from a recieve buffer. It will be
 called by CAN_RecieveMessages().

Parameters:	msg     - pointer to CAN message object to fill message with
Return:		true    - read was a success
            false   - read was a failure
*********************************************************************/

bool CAN_ReadOneMessage(CAN_Message_t *msg) {
    unsigned char *pReg;
    unsigned char loop;
    
    // Read data from mapped receive buffers
    pReg = &MAPPED_SIDH;
    for(loop = 0; loop < CAN_MSG_LENGTH; loop++, pReg++) {
        msg->Array[loop] = *pReg;
    }
    if(msg->DataLength > 8) {
        msg->DataLength = 8;
    }
    for(loop = 0; loop < msg->DataLength; loop++, pReg++) {
        msg->Data[loop] = *pReg;
    }
    
    // Clear buffer full indicator
    MAPPED_CONbits.RXFUL = 0;
    
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
    if(!CanFlags.TXIntsEnabled) {
        CanFlags.TXIntsEnabled = true;
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
static uint8_t lastTXBufferUsed = 0;

void CAN_TransmitMessages() {
    // Remove Oldest Message from TXQueue
    if(TXQueueCount > 0) {
        // ToDo: Support Sending From other TX Buffer
        
        // Set the Window Address Bits to the Message Buffer
        ECANCON = ECAN_SET_FIFO_MODE | 0x03; // Only Support sending with RX0 currently
        
        CAN_SendOneMessage(&TXQueue[TXHead]);
        TXHead++;
        if(TXHead >= TX_QUEUE_SIZE) {
            TXHead = 0;
        }
        TXQueueCount--;
    }
    else {
        // TXQueue is Empty
        CanFlags.TXIntsEnabled = false;
        PIE5bits.TXBnIE = 0;
        PIR5bits.TXBnIF = 0;
        return;
    }
}

/*********************************************************************
CAN_ReceiveMessages

This Function ...

Parameters:	None
Return:		None
*********************************************************************/
#define FIFOEMPTY_MASK   0x80
#define WINDOW_RX_BUFFER  0x10
#define FIFO_POINTER_MASK 0x0F
void CAN_ReceiveMessages() {
    
    while(COMSTAT & FIFOEMPTY_MASK) {
        // Set the Window Address Bits to the Message Buffer
        ECANCON = ECAN_SET_FIFO_MODE | WINDOW_RX_BUFFER | (CANCON & FIFO_POINTER_MASK);
        
        // Read the message
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
        
        COMSTAT &= ~FIFOEMPTY_MASK;
        COMSTAT &= ~FIFOEMPTY_MASK;
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
        case 1000000: { // 1 MBit/s
            BRGCON1 = BRGCON1_1MBIT;
            BRGCON2 = BRGCON2_1MBIT;
            BRGCON3 = BRGCON3_1MBIT;
            break;
        }
        case 625000: { // 625kBit/s
            BRGCON1 = BRGCON1_625kBIT;
            BRGCON2 = BRGCON2_625kBIT;
            BRGCON3 = BRGCON3_625kBIT;
            break;
        }
        case 500000: { // 500kBit/s
            BRGCON1 = BRGCON1_500kBIT;
            BRGCON2 = BRGCON2_500kBIT;
            BRGCON3 = BRGCON3_500kBIT;
            break;
        }
        default: { // Default = 500 kBit/s
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