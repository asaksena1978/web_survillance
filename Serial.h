#ifndef __SERIAL_H
#define __SERIAL_H

/*
###############################################################################

File Name :  SERIAL.H

Version : 2.0

Created :  2001/01/09

Description :  Definition of functions referring to RS232C 
                  All of functions in 'Serial.h' are implemented by polling.
  
Modified History
		Modified : 2002/06/28
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
#include <TYPE.H>


/*
###############################################################################
Define Part
###############################################################################
*/
#define X2MODE				// For MCU supported X2 Mode

/*
If there are unused functions in program and on compiling with Keil-C, it result in warning.In this reason, wrong operation could be happend.
So it prevent to compile unused functions to use define-function.
*/
#define __SERIAL_UNUSED   // If defined with "  __SERIAL_UNUSED", actually it's not to be compiled " __SERIAL_UNUSED Block"


/*
###############################################################################
Grobal Variable Definition Part
###############################################################################
*/




/*
###############################################################################
Function Prototype Definition Part
###############################################################################
*/

extern void InitSerial(void);			/* Initialization of Serial Port(Ex.Baud Rate setting) */

extern void PutByte(UCHAR byData);		/* Output 1 character through Serial Port */

extern unsigned char GetByte(void);		/* Read 1 character from Serial. */

char IsPressedKey();				/* Check to input to Serial or not. */

extern void PutHTOA(UCHAR byData);		/* Output 1 Byte Hexadecimal digit to 2Byte ASCII character.  ex) 0x2E --> "2E" */

extern void PutITOA(UINT byData);		/* Output 2 Byte Integer to 4Byte ASCII character ex) 0x12FD --> "12FD" */

extern void PutLTOA(unsigned long byData);	/* Output 4 Byte Long to 8Byte ASCII character. ex) 0x001234FF --> "001234FF" */

extern void PutString(char *Str) reentrant;	/* Output to Serial. */

extern void PutStringLn(char *Str) reentrant;	/* Output to Serial and then specific character,'Carrage Return & New Line'. */


/*
###############################################################################
Unused Function Prototype Definition Part
###############################################################################
*/

#ifndef __SERIAL_UNUSED

extern void GetString(char* str);		/* Read 1 line string from Serial. */

#endif  //  end __SERIAL_UNUSED


#endif  //  end __SERIAL_H
