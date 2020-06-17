/*
********************************************************************************
* Central Mechanical Engineering Research Institute 
* Durgapur - 713209,
* West Bengal, India
*
* (c) Copyright 2006, Central Mechanical Engineering Research Institute, Durgapur
*
* Filename : main.c
* Programmer(s): Tirtha Kanti Ghosh	 
* Created : 12/04/2006
* Modified :    Date - 2002/10/28
*		Description - Modify Default HTML in DoHTML() function
*                           - Process escape characters(%XX-->0xXX)
					  For Network Camera the module is doJPEG().
* Description : web server related functions
********************************************************************************
*/

#include <reg51.h>		// 8051 SFR definition file
//#include <stdio.h>		// To use 'printf'
#include "serial.h"		// serial related functions
#include "socket.h"		// W3100A driver file
#include "lcd.h"		// LCD related functions
#include "allutil.h"		// utility function
#include "httpd.h"		// web server related functions



char xdata LEDStatus = 0;   	// Global Variable to store the LED status
char xdata SwitchStatus = 0;	// Global Variable to store the Relay Switch status 


/*
********************************************************************************
*              Function to copy data to Tx buffer
*
* Description:
* Arguments  : Tx_Buf : sending buffer
*	  Stream : data to be copied
* Returns    : size of copied data
* Note       : 
********************************************************************************
*/
unsigned int WriteBuf(unsigned char xdata *TX_Buf, unsigned char *Stream)
{
	unsigned int Length;

	for (Length = 0; *Stream != '\0'; Length++)
		*TX_Buf++ = *Stream++;
	return Length;
}

/*
********************************************************************************
*              Function to create HTML header to send to the Tx buffer
*
* Description:
* Arguments  : Tx_Buf : sending buffer
*		 content_type	: type of HTML
* Returns    : header size.
* Note       : 
********************************************************************************
*/
unsigned int PrintHeader(unsigned char *TX_Buf, unsigned char content_type)
{
	unsigned int Length;

	Length = WriteBuf(TX_Buf, "HTTP/1.1 200 OK\r\n");
	Length +=WriteBuf(TX_Buf + Length, "Server: Apache/1.3.9 (Unix)\r\n");

	switch (content_type) { 
		case 't':					// text
			Length += WriteBuf(TX_Buf + Length, "Content-Type: text/plain\r\n");
			break;
		case 'g':					// gif image
			Length += WriteBuf(TX_Buf + Length, "Content-Type: image/gif\r\n");
			break;
		case 'j':					// jpeg image
			Length += WriteBuf(TX_Buf + Length, "Content-Type: image/jpeg\r\n");
			break;
		case 'h':					// html doc.
			Length += WriteBuf(TX_Buf + Length, "Content-Type: text/html\r\n");
			break;
	}

	return Length;
}

/*
********************************************************************************
*              Function to output length of HTML document in ASCII
*
* Description:
* Arguments  : Tx_Buf : sending buffer
*	DataLength	: size of HTML document
* Returns    : total size.
* Note       : 
********************************************************************************
*/
unsigned int MSG_Length(unsigned char *TX_Buf, unsigned int DataLength)
{
	unsigned int Length;

	Length = WriteBuf(TX_Buf, "Content-Length: ");

	*(TX_Buf + Length++) = D2C(DataLength / 10000);
	*(TX_Buf + Length++) = D2C((DataLength / 1000) % 10);
	*(TX_Buf + Length++) = D2C((DataLength / 100) % 10);
	*(TX_Buf + Length++) = D2C((DataLength / 10) % 10);
	*(TX_Buf + Length++) = D2C(DataLength % 10);

	Length += WriteBuf(TX_Buf + Length, "\r\n\r\n");

	return Length;
}

/*
********************************************************************************
*              Function to create HTML document
*
* Description: Due to limited memory of 8051, only one document is allowed.
* Arguments  : Tx_Buf : sending buffer
* Returns    : size.
* Note       : 
********************************************************************************
*/
unsigned int DoHTML(unsigned char *TX_Buf)
{
	unsigned int Length;//, i, Len;
	unsigned char *Msg;
	
	/*Determine the length of content of HTML data (it varies due to on.gif and off.gif*/ 
	if(SwitchStatus == 0)
		Length = MSG_Length(TX_Buf, 1742);
	else if(SwitchStatus == 0x01 || SwitchStatus == 0x02 || SwitchStatus == 0x04)
		Length = MSG_Length(TX_Buf, 1741);
	else if(SwitchStatus == 0x03 || SwitchStatus == 0x05 || SwitchStatus == 0x06)
		Length = MSG_Length(TX_Buf, 1740);
	else
		Length = MSG_Length(TX_Buf, 1739);
	
	Msg = "<html><head><title>== EMBEDDED WEB SERVER****CMERI ==</title></head>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<body bgcolor=\"#FFFFFF\"><h1 align=\"center\"><b><u>Web Based Remote Control Hardware</u></b></h1>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<h3 align=\"center\"><b>Embedded System Lab, CMERI, Durgapur</b></h3>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<center><img border=\"0\" src=\"http://192.168.0.48/test/webserver/board.gif\" alt=\"Experimental Setup Board\" width=\"300\" height=\"250\">";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "</center><form method=\"GET\" action=\"doit.cgi\"><center><table border=\"1\" id=\"table1\">";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<tr><td width=\"183\">Text to be displayed on LCD</td><td width=\"120\"><input type=\"text\" name=\"lcd\" size=\"20\"></td>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<td width=\"183\">(the text should be less than 16 characters)</td></tr>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<tr><td width=\"183\">LEDs Control</td>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<td width=\"120\"><input type=\"checkbox\" name=\"led\" value=\"1\">LED1";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"checkbox\" name=\"led\" value=\"2\">LED2</td>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<td width=\"183\">&nbsp;</td></tr><tr><td width=\"183\">LEDs Number of Blink</td>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "<td width=\"120\"><input type=\"text\" name=\"blink\" size=\"20\"></td><td width=\"183\">(should be less than 10)</td>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "</tr></table></center><p align=\"center\"><input type=\"submit\" value=\"OK\" name=\"submit\"></p>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "</form><h3 align=\"center\"><u>Relay Controller</u></h3><p align=\"center\">";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	/* images according to Relay Switch Status*/
	if(SwitchStatus == 0){ // All Switch are off
		PutStringLn("SwitchStatus = 000");
		
		Msg = "<a href=\"sw4.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw2.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw1.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x01){ //Switch 3 is ON
		PutStringLn("SwitchStatus = 001");

	 	Msg = "<a href=\"sw5.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw3.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw0.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x02){ //Switch 2 is ON
		PutStringLn("SwitchStatus = 010");

	 	Msg = "<a href=\"sw6.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw0.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw3.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x03){ //Switch 2 and 3 are ON
		PutStringLn("SwitchStatus = 011");

	 	Msg = "<a href=\"sw7.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw1.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw2.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x04){ //Switch 1 is ON
		PutStringLn("SwitchStatus = 100");
	 	
	 	Msg = "<a href=\"sw0.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw6.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw5.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x05){ //Switch 1 and 3 is ON
		PutStringLn("SwitchStatus = 101");

	 	Msg = "<a href=\"sw1.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw7.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw4.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x06){ //Switch 1 and 2 are ON
		PutStringLn("SwitchStatus = 110");

	 	Msg = "<a href=\"sw2.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw4.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw7.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/off.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }

	 if(SwitchStatus == 0x07){ //All switches are ON
		PutStringLn("SwitchStatus = 111");
	 	
	 	Msg = "<a href=\"sw3.htm\"><img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw5.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"sw6.htm\">";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

		Msg = "<img border=\"0\" src=\"http://192.168.0.48/test/webserver/on.gif\" width=\"44\" height=\"50\"></a></p>";
		Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	 }


	Msg = "<p align=\"center\">switch1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page

	Msg = "switch2&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;switch3</p></body></html>";
	Length += WriteBuf(TX_Buf + Length, Msg);		// prints the page
	
	return Length;
}

/*
********************************************************************************
*              Function to analyze request from the browser
*
* Description: For simple implementatioon, compare first character of file name
* Arguments  : Data_Buf : receiving buffer
* Returns    : request type.
* Note       : 
********************************************************************************
*/
unsigned char ParseReq(unsigned char *Data_Buf)
{
	unsigned char *Pointer, *BlinkPtr;
	unsigned char i = 0;
	unsigned int blinkno;
	unsigned int SW_No;
	Pointer = Data_Buf;

	while(*Pointer++ != 0x2E);						// Search '.'
	

	if (*Pointer == 'g') {
		//printf("GIF\r\n");
		PutStringLn("GIF");
		return 'g';
	} else if (*Pointer == 'j') {
		//printf("JPG\r\n");
		PutStringLn("JPEG");
		return 'j';
	} else if (*Pointer == 'h') {
		PutStringLn("HTML");
		Pointer -= 2;
		/********************/
		PutStringLn("Switch Value: ");
		PutByte(*(Pointer));
		PutByte(0x0a);
		PutByte(0x0d);
		/********************/
		SW_No = CTOI(Pointer);
		/*Derive the Switch numbers*/
		switch(SW_No){
			case 0:
				SwitchStatus = 0;
				break;
			case 1:
				SwitchStatus = 0x01;
				break;
			case 2:
				SwitchStatus = 0x02;
				break;
			case 3:
				SwitchStatus = 0x03;
				break;
			case 4:
				SwitchStatus = 0x04;
				break;
			case 5:
				SwitchStatus = 0x05;
				break;
			case 6:
				SwitchStatus = 0x06;
				break;
			case 7:
				SwitchStatus = 0x07;
				break;
		}
		P1 = SwitchStatus;			//Apply Switch status controlled by PORT1 to RELAY BOARD
		return 'h';                       
	} else if (*Pointer == 'c') {						// LED Blink functions
       		PutStringLn("CGI");
			if ((*(Pointer + 4) == 'l') && (*(Pointer + 6) == 'd')) 
			{
				Pointer += 8;
				InitLcd();						// Initialize Text LCD
				while ((*Pointer != '&') && (*Pointer != 0x20))		// Output TEXT on LCD
				{
					LcdReady();
					if (*Pointer == 0x2B) 
					{
						LcdDataW = 0x20;
						Pointer++;
					}
					else LcdDataW = *Pointer++;
				}
								/*LED Status Derivation*/
				LEDStatus =0;
				BlinkPtr = Pointer;
				*(FindFirstStr(Pointer,"Referer")) = 0; 		// Trim Gabage Data Trim
				while((Pointer = FindFirstStr(Pointer, "led")) != 0)
				{
					Pointer += 4;
					switch(*Pointer)
					{
						case '1' :  
							LEDStatus |= 0x01;
							break;
						case '2' :
							LEDStatus |= 0x02;
							break;
					}//end of switch(*Pointer)
				}//end of while((Pointer = FindFirstStr(Pointer, "led")) != 0)

					if (!(LEDStatus & 0x01)) T0 = 1;   	// LED0 OFF
					if ( !(LEDStatus & 0x02)) T1 = 1;  	// LED1 OFF

				if(LEDStatus == 0) 				// LED ALL OFF
				{
					PutString("LEDSTATUS: ");
					PutByte(LEDStatus);
					PutByte(0x0a);
					PutByte(0x0d);
					T0 = 1; T1 = 1;
				}


							/*LED Blinks or ON/OFF*/
				while(*BlinkPtr++ != 'b');						// Search bilnk text box

				if(*BlinkPtr == 'l' && *(BlinkPtr + 3) == 'k')
				{
					BlinkPtr += 5;
 				    if(*BlinkPtr != '&')
					{
						while(*BlinkPtr == '+' || *BlinkPtr == 0x20)
						BlinkPtr++;

						blinkno = CTOI(BlinkPtr);
						if(blinkno > 1 && blinkno < 10){
							switch(LEDStatus){
								case 0x01:
									for(i = 0; i < (2 * blinkno); i++){
										T0 = !T0;
										wait_10ms(10);
									}
									break;
								case 0x02:
									for(i = 0; i < (2 * blinkno); i++){
										T1 = !T1;
										wait_10ms(10);
									}
									break;
								case 0x03:
									for(i = 0; i < (2 * blinkno); i++){
										T0 = !T0;
										T1 = !T1;
										wait_10ms(10);
									}
									break;
							}//end of SWITCH(ledno)
						}//end of IF(blinkno > 1 && blinkno < 10)
					}//end of if(*Pointer != '&') 
					else{
						switch(LEDStatus){
							case 0x01:
								T0 = 0;				//LED0 ON
								break;
							case 0x02:
								T1 = 0;				//LED1 ON
								break;
							case 0x03:				//BOTH LED ON
								T0 = 0;			 
								T1 = 0;
								break;
						}
					}
				}//end of if(*Pointer == 'b' && *(Pointer + 4) == 'k') 
		return 'c';
		} else return 'x'; //end of IF(*(Pointer + 4) == 'l') && (*(Pointer + 6) == 'd')

	} //end of IF(*Pointer == 'C')
	else {
		PutStringLn("Any Type");		
		return 'a';
	}
}


									/*WEBCAM Module starts here*/
/******************************************************************************************/

/*
Description   :  Copy HTTP header for only jpeg content type to specific buffer
Argument      :  TX_Buf - Buffer pointer for HTTP JPEG header to be stored (INPUT, OUTPUT)

Return Value  :  Size of buffer for HTTP header to be stored
Note          : 
*/
unsigned int PrintJPEGHeader(unsigned char *TX_Buf)
{
	unsigned int Length;
	Length = WriteBuf(TX_Buf, "HTTP/1.1 200 OK\r\n");
	Length +=WriteBuf(TX_Buf + Length, "Server: Apache/1.3.9 (Unix)\r\n");
	Length += WriteBuf(TX_Buf + Length, "Content-Type: image/jpeg\r\n");
	return Length;
}

/*
Description   :  Make default.HTML(index.html)
Argument      :  TX_Buf - Buffer Pointer storing HTML document(INPUT, OUTPUT)
Return Value  : The size of data added to TX_Buf
Note          : 
*/
unsigned int DoJPEG(unsigned char *TX_Buf, unsigned int cnt_body)
{
	unsigned int Length;

	Length = MSG_Length(TX_Buf, cnt_body);
	return Length;
}


