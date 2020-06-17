#ifndef __AT24C02
#define __AT24C02

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
                                        
#define DEVICE_WR	0xA0				// DEVICE ADDRESS for write operation of AT24C02
#define DEVICE_RD   	0xA1				// DEVICE ADDRESS for read operation of AT24C02


char Check_EEPROM();					// Check whether Serial EEPROM, AT24C02, is operated normally.

char EEP_Write(char addr, char d);				// Write 1 byte data, 'd', to indicated address

char EEP_WriteBytes(char addr, char* d,int len);			// Write data stream to memory from indicated address to indicated address + len

char EEP_Read(char addr, char* d,char IsContinue);		// Read 1 byte from indicated address area of AT24C02
	
char EEP_ReadBytes(char addr, char* d, int len);			// Write to a variable with data to be read from specific address

#endif
