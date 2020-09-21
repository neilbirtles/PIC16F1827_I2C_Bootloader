/*********************************************************************
* FileName:        main.c
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

#include "main.h"
#include "i2c_slave.h"
#include "flash_routines.h"
#include <pic.h>

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection->INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
#pragma config PWRTE = OFF    // Power-up Timer Enable->PWRT disabled
#pragma config MCLRE = ON    // MCLR Pin Function Select->MCLR/VPP pin function is MCLR
#pragma config CP = OFF    // Flash Program Memory Code Protection->Program memory code protection is disabled
#pragma config CPD = OFF    // Data Memory Code Protection->Data memory code protection is disabled
#pragma config BOREN = ON    // Brown-out Reset Enable->Brown-out Reset enabled
#pragma config CLKOUTEN = OFF    // Clock Out Enable->CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#pragma config IESO = ON    // Internal/External Switchover->Internal/External Switchover mode is enabled
#pragma config FCMEN = ON    // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor is enabled

// CONFIG2
#pragma config WRT = OFF    // Flash Memory Self-Write Protection->Write protection off
#pragma config PLLEN = OFF    // PLL Enable->4x PLL disabled
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable->Stack Overflow or Underflow will cause a Reset
#pragma config BORV = LO    // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LVP = ON    // Low-Voltage Programming Enable->Low-voltage programming enabled

//the following line needs to be added to a target program to allow the bootloader
//to detect that 
//const unsigned char app_loaded __at(0xFFF) = 0x55;

void SYSTEM_Initialize(void)
{

    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
}

void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATA = 0x02;
    LATB = 0x00;

    /**
    TRISx registers
    */
    TRISA = 0xFD;
    TRISB = 0xFF;

    /**
    ANSELx registers
    */
    ANSELB = 0x08;
    ANSELA = 0x19;

    /**
    WPUx registers
    */
    WPUB = 0xC0;
    WPUA = 0x00;
    OPTION_REGbits.nWPUEN = 0;

    /**
    APFCONx registers
    */
    APFCON0 = 0x00;
    APFCON1 = 0x00;
   
    // Enable IOCI interrupt 
    INTCONbits.IOCIE = 1; 
    
}

void OSCILLATOR_Initialize(void)
{
    // SCS FOSC; SPLLEN disabled; IRCF 8MHz_HF; 
    OSCCON = 0x70;
    // TUN 0; 
    OSCTUNE = 0x00;
    // SBOREN disabled; 
    BORCON = 0x00;
}

void WDT_Initialize(void)
{
    // WDTPS 1:65536; SWDTEN OFF; 
    WDTCON = 0x16;
}


void __interrupt() INTERRUPT_InterruptManager (void)
{
    
}

unsigned char flash_buffer[Device_Prog_Mem_Write_Latches*2];
unsigned char i2c_wd_address;
unsigned char i2c_index;
unsigned char i2c_status;
unsigned int counter = 0;
unsigned int program_loaded_indicator = 0x3455;
unsigned int program_loaded_location = 0xFFF;

ADDRESS	flash_addr_pointer;

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    // If start voltage cal is held low at reset, then force bootloader mode


App:
	
    // main program loop
    while (1)
    {
        __delay_ms(200);
        LED_Toggle();			
    }
}
/**
 End of File
*/