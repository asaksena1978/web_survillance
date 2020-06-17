/*
###############################################################################
Include Part
###############################################################################
*/

#include "reg51.h"
#include "allutil.h"
#include "serial.h"
#include "socket.h"
#


/*
Description   :  Convert 32bit Address into Dotted Decimal Format
Argument      :  addr - Pointer variable to store converted value(INPUT)
		 addr_str - Character string to store Decimal Dotted Notation Format.(INPUT,OUTPUT)
Return Value  :  
Note          :  
*/
void inet_ntoa(u_char* addr,char* addr_str)
{
	char i ;
	int d;

	for(i = 0; i < 4; i++)
	{
		d = *(addr+i);
		d = d & 0x00FF;
		/* Convert to decimal number */
		*addr_str++ = D2C(d/100);		
		*addr_str++ = D2C(( d / 10 )%10);
		*addr_str++ = D2C(D2C(d%10));		 
		*addr_str++ = '.';
	}
	*(--addr_str) = 0;
}
/*
Description   :  Get Source IP Address of W3100A.
Argument      :  addr - Pointer to store Source IP Address(32bit Address)(INPUT, OUTPUT)
Return Value  :  
Note          :  
*/
void GetIPAddress(u_char* addr)
{
	char i ;
	for(i = 0; i < 4; i++)
		addr[i] = *(SRC_IP_PTR+i);
}

/*
Description   :  Get Source IP Address of W3100A.
Argument      :  addr -  Pointer to store Gateway IP Address(32bit Address)(INPUT, OUTPUT)
Return Value  :  
Note          :  
*/
void GetGWAddress(u_char* addr)
{
	char i ;
	for(i = 0; i < 4; i++)
		addr[i] = *(GATEWAY_PTR+i);
}

/*
Description   : Get Source Subnet mask of W3100A.
Argument      :  addr -  Pointer to store Subnet Mask(32bit Address)(INPUT, OUTPUT)
Return Value  :  
Note          :  
*/
void GetSubMask(u_char* addr)
{
	char i ;
	for(i = 0; i < 4; i++)
		addr[i] = *(SUBNET_MASK_PTR+i);
}

/*
Description   :  To be input Dotted Notation string from RS232C and convert 32-bit or 48bit Decimal Address
Argument      :  addr - Pointer variable to be stored Converted value (INPUT, OUTPUT)
		 base - binary,decimal,hexa-decimal (INPUT)
                 len - number of Dot Character string to be input(INPUT)
Return Value  :  Length of source string
Note          :  If converting has finished, then '1'.Else if user cancel the input or input wrong valued, then '-1'.
*/
char GetDotNotationAddr(u_char* addr, u_int base, u_int len)
{
	u_char xdata str[9];						// Consider the case of binary numberr
	u_char i,c;
	u_char j = 0;
	
	if(base == 0x10) i = 2;						// max number 
	else if (base == 10) i = 3;
	else i = 8;

	while(1) 
	{
		c = GetByte();						// Read 1 Character
		switch(c)
		{
		case 0x0D :						// If New line
			str[j++] = '\0';
			*addr++ = (u_char) ATOI(str,base);		// Convert to Decimal and Store
			len--;
			PutStringLn("");
			if( len <= 0 )	return 1;
			else return -1;
		case '.' :						// If Dot
			str[j++] = '\0';
			*addr++ = (u_char) ATOI(str,base);
			len--;
			PutByte('.');
			j = 0;
			break;
		case 0x1B:
			return -1;					// Cancel
		case 0x08:
			if(j !=0)
			{
		       		PutByte(0x08);
				PutByte(' ');
				PutByte(0x08);
				j--;
			}
			break;
		default:
			if( C2D(c) != c && j < i)			// If Value to be input is not character and not above avaiable max input number.
			{
				PutByte(c);				// echo.
				str[j++] = c;						
			}
			break;
		}
	}
}


/*
********************************************************************************
* Description: Read established network information(G/W, IP, S/N, Mac) of W3100A and Output that through Serial.
* Arguments  :  
* Returns    :
* Note       :  Mac Address is output into format of Dotted HexaDecimal.Others are output into format of Dotted Decimal Format.
********************************************************************************
*/
void GetNetConfig()
{
	char xdata str[16];
	u_char xdata addr[6];
	u_char i;
	PutStringLn("\r\n====================================");
	PutStringLn("       Net Config Information");
	PutStringLn("====================================");
	PutString("MAC ADDRESS      : ");
	for(i = 0; i < 5; i++)		 				// HexaDecimal
	{
		PutHTOA(*(SRC_HA_PTR+i));
		PutByte('.');
	}
	PutHTOA(*(SRC_HA_PTR+i));

	GetSubMask(addr);
	inet_ntoa(addr,str);				   		// Dotted Decimal Format convert
	PutString("\r\nSUBNET MASK      : "); PutStringLn(str);

	GetGWAddress(addr);
	inet_ntoa(addr,str);
	PutString("G/W IP ADDRESS   : ");PutStringLn(str);

	GetIPAddress(addr);
	inet_ntoa(addr,str);
	PutString("LOCAL IP ADDRESS : "); PutStringLn(str);
	PutStringLn("====================================");
}


/*
Description   :  Convert Hex(0-F) to Character
Argument      :  c -  Hex( 0x00'~0x0F)to convert into character (INPUT)
Note          :  
*/
UCHAR D2C(char c)
{
	if( c >= 0 && c <= 9)
		return '0' + c;
	if( c >= 10 && c <= 15)
		return 'A' + c - 10;
	return c;	
}


/*
Description   :  Convert Character into Hex
Argument      :  c - character( '0'~'F') to convert into Hex(INPUT)

Note          :  
*/
char C2D(UCHAR c)
{
	if( c >= '0' && c <= '9')
		return c - '0';
	if( c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if( c >= 'A' && c <= 'F')
		return 10 + c -'A';
	return c;
}


/*
Description   :  Convert Character string into integer number.
Argument      :  str - Source string (INPUT)
                 base - Base of value; must be in the range 2 ~ 16
Return Value  :  Value of Number to be converted 
Note          :  
*/
UINT ATOI(UCHAR* str,char base)
{
        UINT num = 0;
        while (*str !=0)
                num = num * base + C2D(*str++);
	return num;
}	

/*
Description   :  Convert Character string into decimal integer number.
Argument      :  str - Source string (INPUT)
           
Return Value  :  Value of decimal number Number to be converted 
Note          :  
*/
UINT CTOI(UCHAR* str)
//UINT CTOI(UCHAR str)
{	
	UINT num = 0;
//	while(*str != 0)
	while(*str >= '0' && *str <= '9')
	{
		num = num * 10 + (unsigned int)((*str) - '0');
		str++;
	}
//	num = num * 10 + (unsigned int)((str) - '0');
	return num;
}


/*
Description   :   Find beginning of character in string  
Argument      :  src - source string (INPUT)
                 tag -  Character to find in source string (INPUT)
Note          :  
*/
UCHAR * FindFirstStr(UCHAR *src,UCHAR * tag)
{
	UINT i;
	UCHAR * tsrc = src;
	
FIND:
	if((tsrc = FindFirst(tsrc,tag[0])) !=0)
	{
		for(i=1; tag[i] != '\0' ; i++)
		{
			if(*(tsrc+i) == '\0') return 0;
			if( *(tag + i) != *(tsrc+ i) )
			{
				tsrc++;
				goto FIND;
			}
		}
		return tsrc;		
	}
	return 0;
}

/*
Description   :  Find location of character in string  
Argument      :  src - source string (INPUT)
                   c -  Character to find in source string (INPUT)
Note          :  
*/
UCHAR * FindFirst(UCHAR * src,char c)
{
	
	while(*src != '\0')
	{
		if(*src == c) 	return src;
		src++;
	}
	return 0;
}

/*
********************************************************************************
*               Output destination IP address of appropriate channel
*
* Description : Output destination IP address of appropriate channel
* Arguments   : s  - Channel number which try to get destination IP Address
*    addr - Buffer address to store destination IP address
* Returns     : None
* Note        : API Function 
*               Output format is written in Hexadecimal.
********************************************************************************
*/
/*char* GetDestAddr(SOCKET s,u_char* addr)
{
	addr[0] = *(DST_IP_PTR(s));
	addr[1] = *(DST_IP_PTR(s)+1);
	addr[2] = *(DST_IP_PTR(s)+2);
	addr[3] = *(DST_IP_PTR(s)+3);
	return addr;
}*/

