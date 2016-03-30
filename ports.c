#include "main.h"

void PORTS_Init(void)
{
    /* Analog */
    ANCON1 = 0;
    
    /* TRIS */
    TRISA = 0;
    TRISB = 0;
    TRISC = 0x18;
    TRISB2 = OUTPUT;    //CANTX
    TRISB3 = INPUT;     //CANRX
    
    /* LATS */
    LATA = 0;
    LATB = 0;
}
