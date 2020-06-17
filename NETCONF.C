/*
###############################################################################
Wiznet.
5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
Seoul, Korea

(c) Copyright 2002, Wiznet, Seoul, Korea

File Name : NETCONF.C

Version : 1.1

Created : 2002/01/31

Description : Network Configuration Function for W3100A
	     For using Terminal device's Serial interface,Update network's informations.
	     It's implemented Function to Store and Read W3100A's Network Information and User-Specific Information into AT24C02(EEPROM).
		<Memory-Map>
		0x00-0x03 : Gateway IP Address of W3100A (4 Bytes)
		0x04-0x07 : Subnet Mask of W3100A        (4 Bytes)
		0x08-0x0B : Local IP Address of W3100A   (4 Bytes)
		0x0C-0x21 : MAC Address			 (6 Bytes)
		0x22-0xFD : User-Specific Information    (236 Bytes)
		0xFE-0xFF : Reserved. EEPROM Test	 (2 Bytes)

Modified History 

		Modified : 2002/04/12             
		Description : 
		              inet_ntoa(), GetIPAddress(), GetGWAddress(),GetSubMask(),GetDotNotationAddr()

		Modified : 2002/04/13             
		Description : 

		Modified : 2002/10/20             
		Description : Version Up
			      Move the followd function to sockutil.h
		              inet_ntoa(), GetIPAddress(), GetGWAddress(),GetSubMask(),GetDotNotationAddr()

###############################################################################
*/


/*
###############################################################################
Include Part
###############################################################################
*/
#include <reg51.h>	   				// SFR definition
#include <serial.h>	   				// RS232C
#include <AT24C02.h>					// EEPROM
#include <socket.h>					// W3100A
#include <allutil.h>	   				// Util
#include <NetConf.h>					// Header

/*
###############################################################################
Function Implementation Part
###############################################################################
*/


/*
********************************************************************************
* Description: Display user's menu and update Network Information with selected menu by User.
* Arguments  :  
* Returns    :
* Note       :  
********************************************************************************
*/
void Configure(void)
{
	UCHAR Select; 							// Character to be input through Serial('D', 'G', 'S', 'I','H','E'. 'R')
	int i;

	Config_Load();							// Display  Current Network Config.
	GetNetConfig();
	PutStringLn("\r\n< Network Configuration Mode >\r\n");
	while(1)
	{
		Config_Menu();						// Menu Display

		Select = GetByte();					// Read One Literal & Echo.

		switch (Select)	   					// Menu analyzation
		{
			case 'D' :																// Display Current Network Config.
			case 'd' :
				Select = '*';				// Store '*'  not to execute Config_Edit().
				if(Config_Load() == 0)
					PutStringLn("Config_Load() Error");
				GetNetConfig();
				break;
			case 'G' :																// Set Gateway IP Address
			case 'g' :
				Select = 'G';
				PutString("\r\nEnter Gateway IP Address : ");
				break;
			case 'S' :																// Set Subnet Mask 
			case 's' :
				Select = 'S';
				PutString("\r\nEnter Subnet Mask value : ");
				break;
			case 'I' :																// Set Source IP Address
			case 'i' :
				Select = 'I';
				PutString("\r\nEnter Source IP Address : ");
				break;
			case 'H' :																// Set Mac Address
			case 'h' :
				Select = 'H';
				PutString("\r\nEnter Hardware Address : ");
				break;
			case 'E' :																// Exit Config Mode
			case 'e' :
				PutString("\r\n\r\n>Exit Configuration Mode\r\n");
				return;
			case 'R':
			case 'r':
				PutStringLn("Erasing EEPROM...");
				for(i = 0 ; i < 0x100; i++)
				{ 
					EEP_Write(i,0);
					PutByte('*');
				}
				PutStringLn("");
				Select = '*';
				break;
			default :																	// Usage
				Select = '*';				  // Store '*'  not to execute Config_Edit().
				PutStringLn("\r\nPress 'G', 'S', 'I', 'H', 'E' or 'R'");
				break;
		}                                                         
		if(Select != '*')                                         
			Config_Edit(Select);				  // Config Update
	}// END while(1)
}



/*
********************************************************************************
* Description: Display User's Menu
* Arguments  :  
* Returns    :
* Note       :  
********************************************************************************
*/
void Config_Menu(void)
{
	PutStringLn("================================================");
	PutStringLn("(D)isplay network configuration");
	PutStringLn("(G)ateway IP address update - Deciaml");
	PutStringLn("(S)ubnet mask update        - Decimal");
	PutStringLn("(I)p address update         - Decimal");
	PutStringLn("(H)ardware address update   - HexaDeciaml Format");
	PutStringLn("(E)xit network configuration mode");
	PutStringLn("------------------------------------------------");
	PutStringLn("E(r)ase EEPROM with 0x00");
	PutStringLn("================================================");
}

/*
********************************************************************************
* Description:  Load Network Information stored in EEPROM to Register referring to W3100A's Network. 
* Arguments  :  
* Returns    : success = 1, fail = 0
* Note       :  
********************************************************************************
*/
char Config_Load(void)
{
	if(EEP_ReadBytes(GIPADDR,GATEWAY_PTR,4)==0) return 0;		  	// Load Gateway information from EEPROM to W3100A

	if(EEP_ReadBytes(SNET,SUBNET_MASK_PTR,4)==0) return 0;			// Load Subnet Mask from EEPROM to W3100A

	if(EEP_ReadBytes(LIPADDR,SRC_IP_PTR,4)==0) return 0;		  	// Load Source IP Address information from EEPROM to W3100A

	if(EEP_ReadBytes(MADDR,SRC_HA_PTR,6)==0) return 0;		  	// Load Mac Address information from EEPROM to W3100A
}



/*
********************************************************************************
* Description:  Load Network Information stored in EEPROM to Register referring to W3100A's Network. 
* Arguments  : Select - 'G' - G/W , 'S'- S/N , 'I' - IP, 'H' - Mac Address, 'A' - All Update (INPUT)
* Returns    : success = 1, fail = 0
* Note       :  
********************************************************************************
*/
char Config_Save(UCHAR Select)                                            
{                                                                         
	if(Select == 'A' || Select == 'G')				  	//  Load Gateway information from W3100A to EEPROM .
		if(EEP_WriteBytes(GIPADDR,GATEWAY_PTR,4)==0) return 0;    
	if(Select == 'A' || Select == 'S')				  	// Load Subnetmask information from W3100A to EEPROM .
		if(EEP_WriteBytes(SNET,SUBNET_MASK_PTR,4)==0) return 0;
	if(Select == 'A' || Select == 'I')					// Load Source IP Address information from W3100A to EEPROM. 
		if(EEP_WriteBytes(LIPADDR,SRC_IP_PTR,4)==0) return 0;           
	if(Select == 'A' || Select == 'H')					// Source  Mac Address information from W3100A to EEPROM.
		if(EEP_WriteBytes(MADDR,SRC_HA_PTR,6)==0) return 0;
}

/*
********************************************************************************
* Description:Input appropriate Network Information with Serial and selected menu by User.And then Store in W3100A Register.
* Arguments  : Select - 'G' - G/W , 'S'- S/N , 'I' - IP, 'H' - Mac Address  (INPUT)
* Returns    : success = 1, fail = 0
* Note       :  
********************************************************************************
*/
char Config_Edit(UCHAR Select)
{
	u_char Addr[6];
	char ret; 
	if(Select == 'H')	ret = GetDotNotationAddr(Addr,16,6);		// Read Mac Address into Dotted HexaDecimal format through RS232C.
	else			ret = GetDotNotationAddr(Addr,10,4);		
	if(ret == -1)
	{
		PutStringLn("\r\nConfiguration Update Failed");
		return 0;
	}
	switch (Select) 
	{
		case 'G' :
			setgateway(Addr);
			break;
		case 'S' :
			setsubmask(Addr);
			break;
		case 'I' :
			setIP(Addr);
			break;
		case 'H' :
			setMACAddr(Addr);
			break;                                                  
	}                                                                       
	return Config_Save(Select);						// Store each Network informations.
}


/*
###############################################################################
Unused Function Implementation Part
###############################################################################
*/

#ifndef __UNUSED_EEPROM

/*
********************************************************************************
* Description: Store src in appointed address.
* Arguments  : Addr   - EEPROM storage address ( 0x00 - 0xFF)  (INPUT)
	       src    - Source Information to be stored  (INPUT)
	       len    - Byte unit length  of Source Information (INPUT)
* Returns    : success = 1, fail = 0
* Note       :  
********************************************************************************
*/
char SaveInfo(UCHAR addr, char* src, int len)
{
	return EEP_WriteBytes(addr, src, len);
}

/*
********************************************************************************
* Description: Store information of appointed address in dst.
* Arguments  : Addr   - EEPROM storage address( 0x00 - 0xFF)  (INPUT)
	       src    - Starting address of storage (INPUT)
	       len    - Byte unit length  of Source Information(INPUT)
* Returns    : success = 1, fail = 0
* Note       :  
********************************************************************************
*/
char LoadInfo(UCHAR addr, char* src, int len)
{
	return EEP_ReadBytes(addr, src, len);
}

#endif

