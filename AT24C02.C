/*
###############################################################################
 Wiznet.
 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
 Seoul, Korea

 (c) Copyright 2002, Wiznet, Seoul, Korea

 Filename : at24c02.c
 Programmer(s):	WOO YOUL KIM
 Created : 2002/01/29
 Modified :
 Description : Read from or write to Serial EEPROM, AT24C02, using I2C I/F with pin 3 and 4 of Port #1 of 89C51 MCU
	       AT24C02 Size : 2K Bits, 256 Byte, 
	       AT24C02 Address Range : 0x00 ~ 0xFF
	       AT24C02 I/F : I2C using SDA, SCL
Note : AT24C02 has 'Address A2 - A0' for multiple addressing but this code don't support such function.
       And AT24C02 has 'PAGE(8 BYTE) WRITE' function but this code don't support that too.
       (If user want to use this function, you can implement this using other functions in this code)
###############################################################################
*/

/*
###############################################################################
Include Part
###############################################################################
*/
#include "i2c.h" 						// i2c definition file
#include "AT24C02.h"						// Header file

/*
###############################################################################
Function Implementation Part
###############################################################################
*/

/*
Description  :  Check whether Serial EEPROM, AT24C02, is operated normally.
	 	Write to 0xFE address area with 0xEE and 0xFF address area with 0xFF and read back those.
Argument     :  
Return Value :  success = 1, fail = 0;
Note         :  
*/
char Check_EEPROM()
{
	char c=0;
	if(EEP_Write(0xFE,0xEE)==0) return 0;	     			// Write 0xEE to 0xFE address area
	if(EEP_Write(0xFF,0xFF)==0) return 0;	     			// Write 0xFF to 0xFF address area
	if(EEP_Read(0xFE,&c, NACK)==0) return 0;			// Read a byte from 0xFE address area
	if(c != (char)0xEE)	return 0;	     			// Verify
	if(EEP_Read(0xFF,&c,NACK)==0) return 0;				// Read a byte form 0xFF address area
	if(c != (char)0xFF) return 0;		     			// Verify
	return 1;
}

/*
Description  :  Write a byte, 'd', to specific address area
Argument     :  
Return Value :  success = 1, fail = 0;
Note         :  
*/
char EEP_Write(char addr, char d)
{
	GEN_START();  							// Notify start of I2C communication to AT24C02 using 'START BIT PATTEN'
	WriteByte(DEVICE_WR);						// Write DEVICE ADDRESS which AT24C02 define
	if( WAIT_ACK()!=ACK ) 						// Wait for ACK to check whether AT24C02 accepted DEVICE_WR or not
		return 0;

	WriteByte(addr);      						// Inform address for writing AT24C02
	if( WAIT_ACK()!= ACK )						// Wait for ACK
		return 0;

	WriteByte(d);	      						// Write Data to informed address area

	if( WAIT_ACK()!= ACK )						// Wait for ACK
		return 0;
	GEN_STOP();	      						// Notify end of I2C communication to AT24C02 using 'STOP BIT Patten'
	Delay(10);
	return 1;
}

/*
Description  :  Write data stream to AT24C02 from indicated address to indicated address + len
Argument     :  addr - Start address of AT24C02 to be written
	        d    - start address of data stream to write(INPUT)
	        len  - size of data stream to write(INPUT)
Return Value :  success = 1, fail = 0;
Note         :  
*/
char EEP_WriteBytes(char addr, char* d,int len)
{
	int i;
	for(i = 0; i < len ; i++)					// Write data stream by a byte with looping.
		if(EEP_Write(addr+i,d[i])==0) return 0;
	return 1;
}

/*
Description  :  Return to a variable, d, value to be read from specific address area
Argument     :  addr - Address of AT24C02 to be read (INPUT)
		d    - A variable which a value to be read from AT24C02 will be stored(OUTPUT)
		IsContinue - Send ACK instead NACK for Sequential Read. 
			     This is a flag to differentiate those. ACK is ACK, NACK is NACK (INPUT)
Return Value :  success = 1, fail = 0;
Note         :  
*/
char EEP_Read(char addr, char* d,char IsContinue)
{
	GEN_START();
	WriteByte(DEVICE_WR);	 					// Write DEVICE ADDRESS which AT24C02 define
	if( WAIT_ACK()!= ACK )	
		return 0;
	WriteByte(addr);	 					// Inform address to write
	if( WAIT_ACK()!= ACK )
		return 0;        
	GEN_START();		
	WriteByte(DEVICE_RD);						// Write DEVICE ADDRESS which AT24C02 define for read operation
	if( WAIT_ACK()!= ACK )
		return 0;
	*d = ReadByte(); 
	if(IsContinue == NACK )
	{
		if(WAIT_ACK()!=NACK) return 0;
		GEN_STOP();		      				// Inform that all of operations are over,if it read only 1 byte
	}                           
	else								
		SEND_ACK(IsContinue);					// If you want to read continuously then send ACK
 									// Otherwise send NACK
	return 1;
}

/*
Description   :  Write to a variable with data to be read from specific address
Argument      :  addr - Address of AT24C02 to be read (INPUT)
	    	 d    - A variable which data stream to be read from AT24C02 will be stored(INPUT)
		 len  - Size of data stream to be read(INPUT)
Return Value  :  success = 1, fail = 0;

*/
char EEP_ReadBytes(char addr, char* d, int len)
{
	char c;
	int i ;

	for(i = 0 ; i < len ; i++)					// store data to be read to d by 1 byte sequentially.
	{
		if(EEP_Read(addr+i,&c,NACK)==0)
			return 0;
		d[i] = c;
	}
/*
	if(!EEP_Read(addr,&c,ACK)) return 0;				// Read 1 byte using I2C communication.(Use ACK instead NACK and Read 1 byte without STOP)
	d[0] = c;
	for(i = 1 ; i < len ; i++)					// Continuously, Read 1 byte using I2C communication and Wait for ACK
	{
		d[i] = ReadByte();
		PutHTOA(d[i]);PutStringLn("");                          
		if(i == len -1 )					// If you read final byte, Send NACK without waiting for ACK
		{
			if(WAIT_ACK()!=NACK) return 0;
		}
		else	SEND_ACK(ACK);
	}
	GEN_STOP();							// Inform that all of operations are over
*/
	return 1;
}

