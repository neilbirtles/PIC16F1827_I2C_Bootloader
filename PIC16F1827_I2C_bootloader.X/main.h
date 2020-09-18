/*********************************************************************
* FileName:        main.h
* Processor:       PIC16F1827
* Compiler:        XC8
*
* Software License Agreement: See Github License file
*
*********************************************************************
* File Description:
*
* Change History:
* 
* Author               Neil Birtles
********************************************************************/


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define _XTAL_FREQ 8000000
#define Device_Prog_Mem_Write_Latches 8
#define Device_Prog_Mem_Erase_Block_Size 32

// get/set LED aliases
#define LED_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define LED_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define LED_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)

// get/set Start_Voltage_Cal aliases
#define Start_Voltage_Cal_GetValue()           PORTBbits.RB6

#define I2C_Addr_Bit_GetValue()           PORTBbits.RB0

#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)


void SYSTEM_Initialize(void);
void OSCILLATOR_Initialize(void);
void WDT_Initialize(void);
void PIN_MANAGER_Initialize (void);

#endif	/* MAIN_H */

