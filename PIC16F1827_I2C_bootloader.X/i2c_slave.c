/*********************************************************************
* FileName:        i2c_slave.c
* Dependencies:    See INCLUDES section below
* Processor:       PIC16F1827
* Compiler:        XC8
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
*   Changed files to remove refs to PKSA 
*   Updated for XC8 compiler / PIC16F1827
*   Fixed slave address setting 
*   Allowed flash_memory_write to take advantage of more than 8 write latches
* Author               Cristian Toma, Neil Birtles
********************************************************************/

#include <xc.h>
#include "i2c_slave.h"
#include "main.h"
#include "flash_routines.h"

void I2C_Slave_Init()
{
	//these need to be configured according to the chip being used
    TRISB2 = 1;         //SDA input PIC16F1827
    TRISB5 = 1;         //SCL input PIC16F1827

	SSP2BUF = 0x0;          // clear the buffer
	SSP2STAT = 0x80;		// 100Khz
    //work out the address for this slave based on the state of address pin
    //left shift as only 7 MSB used in SSP2ADD
    if(I2C_Addr_Bit_GetValue())
    {
        SSP2ADD = 0x11 << 1;
    }else
    {
        SSP2ADD = 0x10 << 1;
    }
	//SSP2ADD = SLAVE_ADDR << 1;	//only upper 7 bits are used for the address so shift up one place
	SSP2CON1 = 0x36;		// clear WCOL & SSPOV, enable serial port, enable clock, set Slave mode & 7bit addr	
	SSP2CON3 |= 0b01100000;	// Enable interrupt on detection of Start or Restart conditions, SSPBUF is updated 
                            //and ACK is generated for a received address/data byte, ignoring the state of the SSPOV bit only if the BF bit = 0.
}
void _WriteData(unsigned char data)
{
	do
	{		
        SSP2CON1bits.WCOL=0;
		SSP2BUF = data;
	}
	while(SSP2CON1bits.WCOL);
	SSP2CON1bits.CKP = 1;
}
void do_i2c_tasks()
{
		unsigned int dat =0 ;
		unsigned char temp,idx;

		unsigned char token = 0;		
		
		//interrupt flag for the serial port
        if (PIR4bits.SSP2IF)
		{
			token  = SSP2STAT & mask;	//obtain current state

			if(SSP2STATbits.S)
			{
					switch (token)
					{
		
						case MWA :								//MASTER WRITES ADDRESS STATE
							temp=SSP2BUF;
							i2c_status=I2C_SLAVE_ADDRESS_RECEIVED;
						break;
							
						case MWD : 								//MASTER WRITES DATA STATE
							temp=SSP2BUF;
							
							
							if(	i2c_status == I2C_SLAVE_ADDRESS_RECEIVED )
							{   // first time we get the slave address, after that set to word address
								i2c_wd_address = temp;
								i2c_index=0;
								i2c_status = I2C_WORD_ADDRESS_RECEIVED;
							}
							else if ( i2c_status == I2C_WORD_ADDRESS_RECEIVED )
							{	// second time we get the word address, so look into word address 
								if ( i2c_wd_address == SET_FLASH_POINTER_COMMAND)	// 0x01 is buffer word address
								{
									if (i2c_index == 0)
									{
                                        flash_addr_pointer.bytes.byte_H= temp;
										i2c_index++;
									}
									else if (i2c_index == 1)
									{
										 flash_addr_pointer.bytes.byte_L= temp;
                                         //range check the address pointer once we have both bytes
                                         if (flash_addr_pointer.word.address > MAX_PROG_MEM_ADDR)
                                         {
                                             flash_addr_pointer.word.address = MAX_PROG_MEM_ADDR;
                                         }
									}
								}
								else if ( i2c_wd_address == RECEIVE_FLASH_DATA_COMMAND )	// 0x02 write data word address
								{
									flash_buffer[i2c_index]=temp;
									i2c_index++;
									if (i2c_index == 16)
										i2c_index--;
								}
							}					

						break;
						
						case MRA :								//MASTER READS ADDRESS STATE
								if (i2c_wd_address == GET_FLASH_POINTER_COMMAND)			// buffer word address
								{	
									// Send first byte here, next byte will be send at MRD case, see below		
									_WriteData (flash_addr_pointer.bytes.byte_H);
								}
								else if (i2c_wd_address == READ_FLASH_COMMAND)	// read data from flash memory
								{
									if (i2c_index == 0)
									{
										//LED_1 = 1;
										// read data into flash_buffer
										for (idx = 0; idx <16; idx+=2)
										{	
											dat = flash_memory_read (flash_addr_pointer.word.address);
											flash_buffer[idx  ] = dat>>8;
											flash_buffer[idx+1] = dat & 0xFF;
											flash_addr_pointer.word.address++;
										}		
										//LED_1 = 0;
										// send first byte, the rest will be sent at MRD, see below							
										_WriteData(flash_buffer[i2c_index]);
										i2c_index++;
										if (i2c_index == 16)
											i2c_index--;	// should never get here....
									}		
								}
								else if (i2c_wd_address == ERASE_FLASH_ROW_COMMAND)
								{
									// erase command, erases a row of Device_Prog_Mem_Erase_Block_Size words
									//LED_2 = 1;
									flash_memory_erase (flash_addr_pointer.word.address);
									flash_addr_pointer.word.address += Device_Prog_Mem_Erase_Block_Size;
									_WriteData(0x00);
									//LED_2 = 0;
								}
								else if (i2c_wd_address == WRITE_BUFFER_TO_FLASH_COMMAND)
								{
									// write command. What's stored into flash_buffer is written 
									// to FLASH memory at the address pointed to by the address pointer.
									// The address pointer automatically increments by Device_Prog_Mem_Write_Latches units.
									//LED_3 = 1;	                                  
									flash_memory_write(flash_addr_pointer.word.address, flash_buffer, Device_Prog_Mem_Write_Latches);
									flash_addr_pointer.word.address += Device_Prog_Mem_Write_Latches;
									_WriteData(0x00);
									//LED_3 = 0;	
									
								}	
								else if (i2c_wd_address == JUMP_TO_APPLICATION_COMMAND)
								{
									// jump to appplication code
									_WriteData(0x00);
									for ( idx =0; idx < 255; idx++ )
                                        ;
									
                                    asm("RESET");
									
								}
                                else if (i2c_wd_address == PING_COMMAND) //ping check command for bootloader
                                {
                                    //acknowledge with 0xAA
                                    _WriteData(0xAA);
                                }
                                else if (i2c_wd_address == READ_FLASH_BUFFER_COMMAND)
                                {
                                    _WriteData(flash_buffer[i2c_index]);
									i2c_index++;
                                }
						break;
						
						
						case MRD :								//MASTER READS DATA STATE
                                if (i2c_wd_address == GET_FLASH_POINTER_COMMAND)	// buffer word address
								{		
									_WriteData (flash_addr_pointer.bytes.byte_L);
								}
								else if (i2c_wd_address == READ_FLASH_COMMAND)
								{
									_WriteData(flash_buffer[i2c_index]);
									i2c_index++;
									if (i2c_index == 16)
										i2c_index--;
								}
                                else if (i2c_wd_address == READ_FLASH_BUFFER_COMMAND)
                                {
                                    _WriteData(flash_buffer[i2c_index]);
									i2c_index++;
                                    if (i2c_index == 16)
										i2c_index--;
                                }								
						break;		
					}
			}
			else if(SSP2STATbits.P)
			{	//STOP state	
				asm("nop");
				i2c_status = I2C_NO_TRANSACTION; 
			}	

            
			PIR4bits.SSP2IF = 0;
			SSP2CON1bits.SSPEN = 1;														
			SSP2CON1bits.CKP = 1;	//release clock
		}
}


