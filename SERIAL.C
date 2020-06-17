/*
###############################################################################

File Name :  SERIAL.C

Version : 2.0

Created :  2001/01/09

Description :  Definition of functions referring to RS232C 
               All of functions in 'Serial.h' are implemented by polling.
  
Modified History 

		Modified : 2002/03/15				   
		Description : Eliminate disused function
		Modified : 2002/10/20
		Description : Version Up
			      Add GetString()
			      Delete PrintLn()
			      Delete Print()
	                      Modify PutString() to Re-Entrance function
			      Modify PutStringLn() to Re-Entrance function
###############################################################################
*/



/*
###############################################################################
Include Part
###############################################################################
*/

#include <reg51.h>
#include <serial.h>


/*
###############################################################################
Define Part
###############################################################################
*/


/*
###############################################################################
Grobal Variable Definition Part
###############################################################################
*/




/*
###############################################################################
Function Implementation Part
###############################################################################
*/

/*
Description   :  Initialization of Serial Port(Ex.Baud Rate setting)
Argument      :  
Return Value  :  
Note          :  
*/
void InitSerial(void)
{
	ET1 = 0;		/* TIMER1 INT DISABLE */
	TMOD = 0x20;
	PCON |= 0x80;

//	TH1 = 0xFC;		/* X2 57600(SMOD=1) at 22.1184 MHZ */
	TH1 = 0xFD;		/* X2 57600(SMOD=1) at 16 MHZ */

	TR1 = 1;		/* START THE TIMER1 */	
	SCON = 0x52;		/* SERIAL MODE 1, REN=1, TI=1, RI=0 */

	/* Interrupt */
	ES   = 0;	 	/* Serial interrupt disable */
	RI   = 0; 		
	TI   = 0; 		
	while(TI && RI);		
}

/*
Description   :  Output 1 character through Serial Port
Argument      :  byData - character to output(INPUT)
Return Value  :  
Note          :
*/
void PutByte(UCHAR byData)
{
	// Write data into serial-buffer.
	SBUF = byData; 
	// Wait till data recording is finished.
	while(!TI);
	TI = 0;
}

/*
Description   :  Read 1 character from Serial.
Argument      :  
Return Value  :   Read 1 character from Serial and Return.
Note          :  
*/
unsigned char GetByte(void)	
{
	unsigned char byData;
	// Wait till data is received.
	while( !RI );		
	RI = 0;
	// Read data.
	byData = SBUF;		
	return byData;
}


/*
Description   :  Check to input to Serial or not.
Argument      :  
Return Value  :    1)If there's input, then returned value is '1'.
	           2)If there's no input, then returned value is '-1'.
Note          :  
*/
char IsPressedKey()
{
	if( RI == 1 ) return 1;
	return -1;
}


/*
Description   :  Output 1 Byte Hexadecimal digit to 2Byte ASCII character.  ex) 0x2E --> "2E"
Argument      :   byData - character to output(INPUT)
Return Value  :  
Note          :  
*/
void PutHTOA(UCHAR byData)
{
	// HIGH DIGIT
	if((byData / 0x10) >= 10)
		PutByte('A'+((byData/0x10)%0x0A));
	else
		PutByte('0'+((byData/0x10)%0x0A));
	// LOW DIGIT
	if((byData % 0x10) >= 10)
		PutByte('A' + ((byData%0x10)%0x0A));
	else
		PutByte('0' + ((byData%0x10)%0x0A));
}

/*
Description   : Output 2 Byte Integer to 4Byte ASCII character ex) 0x12FD --> "12FD"
Argument      :    byData - Integer to output(INPUT)
Return Value  :  
Note          :  
*/
void PutITOA(UINT byData)
{
	PutHTOA(byData / 0x100);
	PutHTOA(byData % 0x100);
}

/*
Description   :   Output 4 Byte Long to 8Byte ASCII character. ex) 0x001234FF --> "001234FF"
Argument      :  byData - Long to output (INPUT)
Return Value  :  
Note          :  
*/
void PutLTOA(unsigned long byData)
{
	// upper 2 Byte
	PutITOA(byData / 0x10000);
	// lower 2 Byte
	PutITOA(byData % 0x10000);
}

/*
Description   :  Output to Serial.
Argument      :  Str - Character Stream to output (INPUT)
Return Value  :  
Note          :  Version 2.0
*/
void PutString(char *Str) reentrant 
{
	UINT i;
	for (i = 0; Str[i] != '\0'; i++)	PutByte(Str[i]);

}

/*
Description   :  Output to Serial and then specific character,'Carrage Return & New Line'.
Argument      : Str -  Character Stream to output(INPUT)
Return Value  :  
Note          :  Version 2.0
*/
void PutStringLn(char * Str) reentrant 
{
        PutString(Str);
	PutByte(0x0a);
	PutByte(0x0d);
}


/*
###############################################################################
Function Prototype Definition Part
###############################################################################
*/

#ifndef __SERIAL_UNUSED

/*
Description   :  Read 1 line string from Serial.
Argument      :  1 line string to be retuned
Return Value  :  
Note          :  Version 2.0
		 if First input character is '!' then display previous command and wait enter.
*/
void GetString(char* str)
{
	char c;
	char * tsrc = str;
	char IsFirst = 1;
	while((c=GetByte()) != 0x0D)
	{      
		if(IsFirst && c=='!')
		{
			PutString(str);
			while(*str != '\0')str++;
			IsFirst = 0;
			continue;
		}
		if(c == 0x08 && tsrc != str)
		{
	       		PutByte(0x08);
			PutByte(' ');
			PutByte(0x08);
			str--;
			continue;
		}
		else if (c == 0x1B)
		{
			while(tsrc != str)
			{
		       		PutByte(0x08);
				PutByte(' ');
				PutByte(0x08);
				str--;
			}
			IsFirst = 1;
			continue;			
		}
		else if( (c < 32 || c > 126) && c != '\t')	continue; 
		PutByte(c);
		*str++ = c;
		IsFirst = 0;
	}
	*str = '\0';
	PutStringLn("");
}

#endif // end __SERIAL_UNUSED

