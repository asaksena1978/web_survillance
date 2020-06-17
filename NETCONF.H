#ifndef __CONFIG_H
#define __CONFIG_H
/*
###############################################################################
Wiznet.
5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
Seoul, Korea

(c) Copyright 2002, Wiznet, Seoul, Korea

File Name : NETCONF.H

Version : 1.1

Created : 2002/01/31

Description : Network Configuration Function for W3100A
		  For using Terminal device's Serial interface,Update network's informations.
		  It's implemented Function to Store and Read W3100A's Network Information and User-Specific Information into AT24C02(EEPROM).
		  <Memory-Map>
		  0x00-0x03 : Gateway IP Address of W3100A (4 Bytes)
		  0x04-0x07 : Subnet Mask of W3100A	   (4 Bytes)
		  0x08-0x0B : Local IP Address of W3100A   (4 Bytes)
		  0x0C-0x21 : MAC Address		   (6 Bytes)
		  0x22-0xFD : User-Specific Information    (236 Bytes)
		  0xFE-0xFF : Reserved. EEPROM Test	   (2 Bytes)

Modified History

		Modified : 2002/04/12             
		Description : Implemented Additional Function 
		              inet_ntoa(), GetIPAddress(), GetGWAddress(),,GetSubMask(),GetDotNotationAddr()
		Modified : 2002/04/13             
		Description :

		Modified : 2002/10/20             
		Description : Version Up
			      Move following functions to sockutil.h
		              inet_ntoa(), GetIPAddress(), GetGWAddress(),GetSubMask(),GetDotNotationAddr()
###############################################################################
*/

/*
###############################################################################
Include Part
###############################################################################
*/
#include "type.h"	      		// Type Definition

/*
###############################################################################
Define Part
###############################################################################
*/

#define __UNUSED_EEPROM

#define	GIPADDR		0x00		// Starting Address to store Gateway, IP Address
#define SNET		0x04		// Starting Address to store Subnet Mask
#define LIPADDR		0x08		// Starting Address to store Local(Source) IP Address
#define MADDR		0x0C		// Starting Address to store Mac Address
#define TESTADDR	0xFE		// Starting Address of memory reserved to test EEPROM



/*
###############################################################################
Function Prototype Definition Part
###############################################################################
*/
extern void Configure(void);							    	// Display User Menu and Update Network Information by selected menu.

extern void Config_Menu(void);								// Display User's Menu

extern char Config_Load(void);								// Load Network Information stored in EEPROM to Register referring to W3100A's Network. 

extern char Config_Save(UCHAR Select);							// Store Register's value referring to W3100A's Network in EEPROM .

extern char Config_Edit(UCHAR Select);							// Input appropriate Network Information with Serial and selected menu by User.And then Store in W3100A Register.

/*
###############################################################################
Unused Function Prototype Definition Part
###############################################################################
*/
#ifndef __UNUSED_EEPROM

extern char SaveInfo(UCHAR addr, char* src, int len);					// Store src in appointed address.

extern char LoadInfo(UCHAR addr, char* src, int len);					// Store information of appointed address in dst.

#endif	// END __UNUSED_EEPROM

#endif	// END __CONFIG_H
