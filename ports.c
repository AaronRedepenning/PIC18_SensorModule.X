#include "main.h"

void PORTS_Init(void)
{
    /* Analog */
    ANCON1 = 0;
    
    /* TRIS */
    TRISA = 0;
    TRISB = 0;
    TRISC = 0x18;
    TRISB0 = INPUT;     // DIP1
    TRISB1 = INPUT;     // DIP2
    TRISB2 = INPUT;     // DIP3
    TRISB3 = INPUT;     // DIP4
    TRISB4 = INPUT;     // DIP5
    TRISC6 = OUTPUT;    //CANTX
    TRISC7 = INPUT;     //CANRX
    
    /* LATS */
    LATA = 0;
    LATB = 0;
}
