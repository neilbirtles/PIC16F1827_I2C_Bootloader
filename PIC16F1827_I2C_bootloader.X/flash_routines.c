/*********************************************************************
* FileName:        flash_routines.c
* Dependencies:    See INCLUDES section below
* Processor:       
* Compiler:        
* Company:         Microchip Technology, Inc.
*
* Software License Agreement:
*
* The software supplied herewith by Microchip Technology Incorporated
* (the "Company") for its PICmicro® Microcontroller is intended and
* supplied to you, the Company's customer, for use solely and
* exclusively on Microchip PICmicro Microcontroller products. The
* software is owned by the Company and/or its supplier, and is
* protected under applicable copyright laws. All rights are reserved.
* Any use in violation of the foregoing restrictions may subject the
* user to criminal sanctions under applicable laws, as well as to
* civil liability for the breach of the terms and conditions of this
* license.
*
* THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*********************************************************************
* File Description:
*
* Change History:
* Author               Cristian Toma
********************************************************************/

//#include <htc.h>
//#include <pic16F1937.h>
#include<xc.h>

//****************************************************************
//  FLASH MEMORY READ
//  needs 16 bit address pointer in address
//  returns 14 bit value from selected address
//
//****************************************************************
unsigned int flash_memory_read (unsigned int address)
{

	EEADRL=((address)&0xff);
	EEADRH=((address)>>8);	
	CFGS = 0;					// access FLASH program, not config
	LWLO = 0;					// only load latches	

	EEPGD = 1;
	RD = 1;
    asm("NOP");
    asm("NOP");
	return ( (EEDATH)<<8 | (EEDATL) ) ;
}	
//****************************************************************
//  FLASH MEMORY WRITE
//  needs 16 bit address pointer in address, 16 bit data pointer
//
//****************************************************************

void flash_memory_write (unsigned int address, unsigned char *data )
{
		unsigned char wdi;
		
		EECON1 = 0;
	
		EEADRL=((address)&0xff);	// load address
		EEADRH=((address)>>8);		// load address
	
		for (wdi=0;wdi<14;wdi+=2)
		{
			EEDATH = data[wdi];
			EEDATL = data[wdi+1];
		
			EEPGD = 1;					// access program space FLASH memory
			CFGS = 0;					// access FLASH program, not config
			WREN = 1;					// allow program/erase
			LWLO = 1;					// only load latches
			EECON2 = 0x55;
			EECON2 = 0xAA;
			
			
			WR = 1;						// set WR to begin write
            asm("NOP");
            asm("NOP");
			
			EEADR++;
		}	

		EEDATH = data[14];
		EEDATL = data[15];
		EEPGD = 1;					// access program space FLASH memory
		CFGS = 0;					// access FLASH program, not config
		WREN = 1;					// allow program/erase
		
		LWLO = 0;					// this time start write
		EECON2 = 0x55;				
		EECON2 = 0xAA;				
		WR = 1;						// set WR to begin write
		asm("NOP");
		asm("NOP");
        
		WREN = 0;					// disallow program/erase
		
}
//****************************************************************
//  FLASH MEMORY ERASE
//  Program memory can only be erased by rows. 
//  A row consists of 32 words where the EEADRL<4:0> = 0000.
//
//****************************************************************	
void flash_memory_erase (unsigned int address)
{
		EEADRL=((address)&0xff);	// load address
		EEADRH=((address)>>8);		// load address
		CFGS = 0;					// access FLASH program, not config
		WREN = 1;					// allow program/erase		
		EEPGD = 1;					// access program space FLASH memory
		FREE = 1;					// perform an erase on next WR command, cleared by hardware
		EECON2 = 0x55;				// required sequence
		EECON2 = 0xAA;				// required sequence
		WR = 1;						// set WR to begin erase cycle
		WREN = 0;					// disallow program/erase		
}	
