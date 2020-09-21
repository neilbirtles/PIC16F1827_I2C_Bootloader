/*********************************************************************
* FileName:        i2c_slave.h
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
*   Changed files to remove refs to PKSA 
*   Updated for XC8 compiler / PIC16F1827
* Author               Cristian Toma, Neil Birtles
********************************************************************/

#include "typedef.h"

// I2C serial communication states
#define I2C_NO_TRANSACTION					0
#define I2C_SLAVE_ADDRESS_RECEIVED			1
#define I2C_WORD_ADDRESS_RECEIVED			2
#define I2C_READ_ADDRESS					3
#define I2C_READ_DATA						4
#define I2C_MASTER_NACK						5		

//I2C states mask - masks all bits in SSPxSTAT apart from:
//bit 5 - D/A: Data (1) / Address (0) bit
//bit 2 - R/W: Read (1) /Write (0) bit
//bit 0 - BF: Buffer Full Status bit - 1 if rx complete or tx in progress
const unsigned char mask = 0x25;

//Above mask is ANDed with SSPxSTAT to get the below states
#define MWA 0x1			//Master Writes Address SSPxSTAT[xx0xx0x1]
#define MWD 0x21		//Master Writes Data    SSPxSTAT[xx1xx0x1]
#define MRA 0x5			//Master Reads Address  SSPxSTAT[xx0xx1x1]
#define MRD 0x24		//Master Reads Data     SSPxSTAT[xx1xx1x0]

//define commands
#define GET_FLASH_POINTER_COMMAND       0x01
#define SET_FLASH_POINTER_COMMAND       0x01
#define RECEIVE_FLASH_DATA_COMMAND      0x02
#define READ_FLASH_COMMAND              0x03
#define ERASE_FLASH_ROW_COMMAND         0x04
#define WRITE_BUFFER_TO_FLASH_COMMAND   0x05
#define JUMP_TO_APPLICATION_COMMAND     0x06
#define PING_COMMAND                    0xAA

// address typdef
extern ADDRESS	flash_addr_pointer;

// function prototypes
void I2C_Slave_Init(void);
void do_i2c_tasks(void);

// externs
extern unsigned char flash_buffer[];
extern unsigned char i2c_index;
extern unsigned char i2c_wd_address;
extern unsigned char i2c_status;



