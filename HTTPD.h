/*
********************************************************************************
* Wiznet.
* 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
* Seoul, Korea
*
* (c) Copyright 2002, Wiznet, Seoul, Korea
*
* Filename : httpd.h
* Programmer(s): 
* Created : 2002/01/
* Modified :    Date - 2002/10/28
*		Description - Modify Default HTML in DoHTML() function
*                           - Process escape characters(%XX-->0xXX)
* Description : web server related functions.
********************************************************************************
*/
extern unsigned int WriteBuf(unsigned char xdata *TX_Buf, unsigned char *Stream);	/* Write stream to W3100A's TX buffer */
extern unsigned int PrintHeader(unsigned char *TX_Buf, unsigned char content_type);	/* Generate HTTP Header and then write it to W3100A's TX Buffer */
extern unsigned int MSG_Length(unsigned char *TX_Buf, unsigned int DataLength);		/* Fill length filed of HTTP Header */
extern unsigned int DoHTML(unsigned char *TX_Buf);					/* Generate HTTP Body */
extern unsigned char ParseReq(unsigned char *Data_Buf);					/* Analize HTTP request and then apply it. */

/*
Description   :  Copy HTTP header for only jpeg content type to specific buffer
Argument      :  TX_Buf - Buffer pointer for storing HTTP JPEG Header(INPUT, OUTPUT)

Return Value  :  the size of buffer storing HTTP Header 
Note          : 
*/
extern unsigned int PrintJPEGHeader(unsigned char *TX_Buf);

/*
Description   :   Make default.HTML(index.html)
Argument      :  TX_Buf - Buffer Pointer storing HTML document(INPUT, OUTPUT)
Return Value  : The size of data added to TX_Buf
Note          : 
*/
unsigned int DoJPEG(unsigned char *TX_Buf, unsigned int cnt_body);
