/*
********************************************************************************
* Wiznet.
* 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
* Seoul, Korea
*
* (c) Copyright 2002, Wiznet, Seoul, Korea
*
* Filename : type.h
* Created : 2002/01/
* Modified :
* Description : Redefinition of type with 8051MCU 
********************************************************************************
*/

#ifndef _TYPE_H_
#define _TYPE_H_

typedef	unsigned char	BYTE;		// 8-bit value
typedef	unsigned char	UCHAR;		// 8-bit value
typedef	unsigned int	INT;		// 16-bit value
typedef	unsigned int	UINT;		// 16-bit value
typedef	unsigned short	USHORT;		// 16-bit value
typedef	unsigned short	WORD;		// 16-bit value
typedef	unsigned long	ULONG;		// 32-bit value
typedef	unsigned long	DWORD;		// 32-bit value

// bsd
typedef unsigned char	u_char;		// 8-bit value
typedef unsigned short	u_short;	// 16-bit value
typedef unsigned int	u_int;		// 16-bit value
typedef unsigned long	u_long;		// 32-bit value

typedef UCHAR	SOCKET;



/* Type to process 4 byte-variable by each 1 byte */
typedef union _un_l2cval {
	u_long	lVal;
	u_char	cVal[4];
}un_l2cval;

/* Type to process 2 byte-variable by each 1 byte  */
typedef union _un_i2cval {
	u_int	iVal;
	u_char	cVal[2];
}un_i2cval;

#endif

//extern unsigned char HTTP_NC_Flag;		//whether the interrupt is for HTTP or Network Camera (NC).
// unsigned char HTTP_NC_Flag;		//whether the interrupt is for HTTP or Network Camera (NC).
