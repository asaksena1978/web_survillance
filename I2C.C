/*
###############################################################################
Wiznet.
5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
Seoul, Korea

(c) Copyright 2002, Wiznet, Seoul, Korea

File Name : I2C.C

Version : 1.0

Programmer(s) : WOO YOUL KIM

Created : 2002/01/29

Description :  Definiton of I2C Interface
	      I2C interface is implemented using 8051 Port 1 and it is defined as SDA(P1.3), SCL(P1.4)
	      Function Delay() could be changed by performance of MCU
  
Modified History :

		Modified : 2002/04/13             
		Programmer : Woo Youl Kim
		Description : Comment

###############################################################################
*/


/*
###############################################################################
Include Part
###############################################################################
*/
#include <reg51.h>
#include "i2c.h"


/*
###############################################################################
Function Implementation Part
###############################################################################
*/

/*
Description  :  START Bit-Patten Generate
	        ----|________	SDA
		-------|_____ SCL 
Argument     :  
Return Value :  
Note         :  
*/
void GEN_START()
{
	P1 |= SDA_H;Delay(1);	//SDA HIGH
	P1 |= SCL_H;Delay(1);	//SCL HIGH
	P1 &= SDA_L;Delay(1);	//SDA LOW
	P1 &= SCL_L;Delay(1);	//SCL LOW
}

/*
Description  :  END Bit-Pattern Generate
	     	______|-----	SDA
	     	___|-------- SCL 
Argument     :  
Return Value :  
Note         :  
*/
void GEN_STOP()
{
	P1 &= SCL_L;Delay(1);	// SCL LOW
	P1 &= SDA_L;Delay(1);	// SDA LOW
	P1 |= SCL_H;Delay(1);	// SCL HIGH;
	P1 |= SDA_H;Delay(1);   // SDA HIGH;
}


/*
Description  :  Send ACK or NACK Signal
Argument     :  ack - ACK or NACK (INPUT)
Return Value :  
Note         :  MCU originate ACK or NACK
*/
void SEND_ACK(char ack)
{
	P1 &= SCL_L;Delay(1);					// Send ACK(LOW) or NACK(HIGH) through SDA and generate a SCL clock 
	if(ack == ACK)						// In case of sending ACK
		P1 &= SDA_L;
	else							// In case of sending NACK
		P1 |= SDA_H;
	Delay(2);
	P1 |= SCL_H;Delay(1);					// SCL Clock
	P1 &= SCL_L;Delay(1);
}

/*
Description  :  Wait ACK or NACK Siganl
Argument     :  
Return Value :  ACK or NACK
Note         :  Device, not MCU, originate ACK or NACK. MCU wait for ACK or NACK
*/
char WAIT_ACK()
{
	char ack; 						// Wait for ACK or NACK
	P1 &= SCL_L;Delay(1);
	P1 |= SCL_H;Delay(1);
	if(P1 & SDA_H)	     					// if Signal is 'HIGH', then NACK, otherwise 'LOW', then ACK
		ack = NACK;
	else
		ack = ACK;
	P1 &= SCL_L;Delay(1);
	return ack;
}



/*
Description  :  Write Byte via I2C
Argument     :  b - To be writted a byte value
Return Value :  
Note         :  
*/
void WriteByte(char b)
{
	char i =0;
	for( ; i < 8  ; i++)  					// Send data from MSB to LSB through SDA adjusting SCL clock
	{
		P1 &= SCL_L;Delay(1);  				//SCL LOW

		/* Extract a bit from data 'b' by shift operation continuously, from MSB to LSB*/
		if( ((b << i) & 0x80) )				// if an extracted data is 1, then send HIGH through SDA
			P1 |= SDA_H;
		else						// otherwise it is 0, then send LOW through SDA
			P1 &= SDA_L;    
		Delay(1);
		P1 |= SCL_H;Delay(1);				//SCL HIGH
	}
	P1 &= SCL_L;Delay(1);					// SCL LOW
}


/*
Description  :  Read a Byte via I2C
Argument     :  
Return Value :  Read Byte Value
Note         :  
*/
char ReadByte()
{
	char ret;
	char i = 0;
	ret = 0;
	P1 &= SCL_L;Delay(1);			       		// SCL LOW
	for(; i<8 ; i++)
	{
		P1 |= SCL_H;Delay(1);		       		// SCL HIGH
		ret = (ret << 1);
		if(P1 & SDA_H)	ret++;
		P1 &= SCL_L;Delay(1);				// SCL LOW
	}
	return ret;
}

/*
Description  :  Delay Function 
Argument     :  i - about i*1ms Delay At 24MHz (INPUT)
Return Value :  Read Byte Value
Note         :  This function don't guarantee exact delay
                  The value of delay might be changed by system
*/
void Delay(int i)
{
	int j;
	for(; i != 0 ; i--)
		for( j = 0;j < 512;j++);
}


