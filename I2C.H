#ifndef __I2C
#define __I2C
/*
###############################################################################

Wiznet.
5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
Seoul, Korea

(c) Copyright 2002, Wiznet, Seoul, Korea

File Name : I2C.H

Version : 1.0

Created : 2002/01/29

Description :  Implementation of I2C I/F using port1 of 8051MCU
	      SCL LOW TIMING  : It depends on I2C BUS I/F.It's used  Delay more than 2us.
	      SCL HIGH TIMING : It depends on I2C BUS I/F.It's used  Delay more than 2us. 
	      SDA <-----> connect with P1.3 of 8051 MCU
	      SCL <-----> connect with P1.4 of 8051 MCU

	      SDA & SCL TIMING :
	                  DataInvalid  Data Valid  DataInvalid
			  SDA : ------------(           )-----------
			  SCL : _______________|------|____________		SCL--> LOW : SDA is changeable , SCL--> High: SDA is not changeable
  
Modified History

		Modified : 2002/04/13             
		

###############################################################################
*/


/*
###############################################################################
Define Part
###############################################################################
*/
#define SDA_H	0x08			  	// SDA HIGH
#define SDA_L	~(SDA_H)			// SDA LOW 

#define SCL_H	0x10			  	// SCL HIGH   -- SDA Valid
#define SCL_L	~(SCL_H)			// SCL LOW    -- SDA Changable

#define ACK	0x00			  	// ACK Signal
#define NACK	0X01			  	// NACK Signal


/*
###############################################################################
Function Prototype Definition Part
###############################################################################
*/
void GEN_START();				// START Bit-Patten Generate

void GEN_STOP();				// END Bit-Pattern Generate

void SEND_ACK(char ack);			// Send ACK or NACK Signal

char WAIT_ACK();				// Wait ACK or NACK Siganl

void WriteByte(char b);				// Write Byte via I2C

char ReadByte();				// Read Byte via I2C

void Delay(int i);				// Delay Func.

#endif








