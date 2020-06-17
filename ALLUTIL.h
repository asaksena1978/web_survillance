#ifndef __ALL_UTIL__
#define __ALL_UTIL__


#include "type.h"

void inet_ntoa(u_char* addr,char* addr_str);		/* Convert 32bit Address into Dotted Decimal Format */

void GetIPAddress(u_char* addr);			/* Get Source IP Address of W3100A. */

void GetGWAddress(u_char* addr);			/* Get Source IP Address of W3100A. */

void GetSubMask(u_char* addr);				/* Get Source Subnet mask of W3100A. */

char GetDotNotationAddr(u_char* addr, u_int base, u_int len); /* To be input Dotted Notation string from RS232C and convert 32-bit or 48bit Decimal Address */

void GetNetConfig();					/* Read established network information(G/W, IP, S/N, Mac) of W3100A and Output that through Serial.*/

extern UINT ATOI(UCHAR* str,char base);		/* Convert Character string to integer number */

extern UINT CTOI(UCHAR* str);		/*Convert Charecter string to Decimal number */

extern UCHAR D2C(char c);			/* Convert Hex(0-F) to Character */

extern char C2D(UCHAR c);			/* Convert Character to Hex */

extern UCHAR * FindFirstStr(UCHAR *src, UCHAR * tag);		/*Find beginning of character in string  */

extern UCHAR * FindFirst(UCHAR * src,char c);		/*Find location of character in string  */

//char* GetDestAddr(SOCKET s,u_char* addr);		/* Output destination IP address of appropriate channel */

#endif
