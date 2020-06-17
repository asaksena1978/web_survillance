/*
********************************************************************************
* Central Mechanical Engineering Research Institute 
* Durgapur - 713209,
* West Bengal, India
*
* (c) Copyright 2006, Central Mechanical Engineering Research Institute, Durgapur
*
* Filename : main.c
* Programmer(s): A. Saksena	 
* Created : 12/04/2006
* Modified :	- Use W3100A Driver ver 2.2
* Description : HTTP Server Program with Network Camera Module
       This is a simple HTTP server program using well-known port #80 and service web page to user's web browser.
       This server program service only home.htm.
       This default.html includes a form to control LCD and LED on EVB8051 and to control switches of Remote Controller.
       If user enter some text in the form, the text will be shown on LCD.
       If user select LED ON/OFF control box (with LED blink), LEDs will be turn on/off on EVB8051.
       These functions are implemented by CGI. In this case, if there's CGI in request message from web browser,
       call the fucntion() and handle it. This way is different from normal CGI service that there's CGI file externally.
	   The Switch control of Remote Control can be done through htm filetype and Network Camera done through anyfiletype.

********************************************************************************
*/

/*
###############################################################################
Include Part
###############################################################################
*/
#include <reg51.h>			// 8051 SFR Definition
//#include <stdio.h>			// To use 'printf'
#include "serial.h"			// serial related functions
#include "socket.h"			// W3100A driver file
#include "httpd.h"			// http related functions
#include "lcd.h"    			// LCD
#include "at24c02.h"                    // EEPROM
#include "netconf.h"                    // Network Configuration
#include "allutil.h"			// Useful function of W3100A
#include "webcam.h"                 // LC82210 & OV7620
#include "type.h"

/*
###############################################################################
Define Part
###############################################################################
*/
#define	MAX_BUF_SIZE	2048		// Maximum buffer size of Rx buffer

/*
###############################################################################
Local  Variable Declaration Part
###############################################################################
*/


sfr  CKCON    	      = 0x8F;		// Definition of CKCON Register
u_char xdata * rx_buf = 0x7000;		// Position of Rx Buffer
u_char xdata * tx_buf = 0x7800;		// Position of Tx Buffer
			/*WEBCAM VARIABLES*/
u_char xdata *TXBuf = 0x6E00;			// Send buffer address for sending Web-page (0x6E00 ~ 0x6FFF)
extern unsigned char STAT;		// Current status of LC82210
//u_char SSTATUS[4];                      // A variable for storing socket status of W3100A
//unsigned char HTTP_NC_Flag;


/*
###############################################################################
Function Prototype Declaration Part
###############################################################################
*/
void Init8051();			// Initialize 8051 MCU
void init_sock(u_char i);		// Initialize designated channel
void InitNetConfig(void);		// Initialize Network Information
//void PutIP(void);				// Set New IP

/*
###############################################################################
Function Implementation Part
###############################################################################
*/
/*
********************************************************************************
*              Main function for Web server
*
* Description: Initialize 8051 and W3100A.
*	       Transfer the appropriate web page by web page request and wait for the connection establishment from browser
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/
void main()
{
	SOCKET i;
	int len, Length;
	u_char state;
	u_int cnt_header, tSize, tQuality; /*WEBCAM Variables*/
	//u_char addr[4];						/*WEBCAM Variables*/
	//u_char addrstr[16];					/*WEBCAM Variables*/
	u_char type;

	Init8051();
	//PutIP(); /*For Setting New IP*/
	PutStringLn("\r\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-");
	PutString  (" Embbedded Web Server Program - Created Date : ");PutLTOA(0x20060322); PutStringLn("");
	PutStringLn("                            - Created By   : CMERI, Durgapur");
	PutStringLn("                            - W3100A Driver: V2.2");
	PutStringLn("                            - Flatform     : 8051 EVB V3.0");
	PutStringLn("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\r\n");
	InitLcd();									// Initialize LCD
	initW3100A();   		
	
	InitNetConfig();
	
	/****************************WEBCAM Initialization Part**********************************/

	InitOV7620(0x13, 0x01);				// Auto adjust Mode
	InitOV7620(0x12, 0x24);			
	PutStringLn("\r\nAuto Adjust Mode");

	InitOV7620(0x06, 0x80);				// Control Brightness : default 0x80
	PutStringLn("Brightness Control");

	InitOV7620(0x19, 0x04);				// Control Vertical Window Start
	PutStringLn("Control Vertical Window Start\r\n");

	tSize = 1;							// CIF
	tQuality = QUALITY;

	cnt_header = InitLC82210(tSize, tQuality);	 		// LCD82210 Initialization 
	PutString("HEADER SIZE : "); PutITOA(cnt_header); PutStringLn("");

	/******************************END********************************************************/
	

	for (i = 0; i < MAX_SOCK_NUM; i++) init_sock(i);				// Initialize socket and Stay in wait mode for connection request

	while (1) 
	{
		i = 0;
		for (i = 0; i < MAX_SOCK_NUM; i++) 
		{
	       	state = select(i, SEL_CONTROL);					// Re-confirm socket status in connected state
			switch(state)                                                   // In this simple HTTP server of EVB8051, service only HTML document
			{
			case SOCK_ESTABLISHED :
					//PutString("The Interrupt is for: ");
					//PutByte(HTTP_NC_Flag);
					//if(HTTP_NC_Flag == 1){
			     		if (((len = select(i, SEL_RECV)) > 0) && (STAT == 6)) 	       		// Confirm if received data 
				     	{
				     		if (len > MAX_BUF_SIZE)	len = MAX_BUF_SIZE;	// When received data more than system buffer, handle as size as buffer.
							len = recv(i, rx_buf, len);
							type = ParseReq(rx_buf);			// Analyze request
							//len = PrintHeader(tx_buf, type);		// Make HTTP Response Header
		
							switch (type) 					// Transfer web page as for each request	
							{ 
								case 'g':				// gif processing
							       				// ADD TO YOUR CODE
									//break;
		       					case 'j':				// jpeg processing
												// ADD TO YOUR CODE
            	            	  	//break; 
       							case 'h':				// HTML 
									//len += DoHTML(tx_buf + len);    // Make HTML BODY
									//len = send(i, tx_buf, len);     // SEND WEB PAGE
									//break;					// ADD TO YOUR CODE
								//case 'a':       			// Any Type
									//break; 
								case 'c':				// In case of CGI - CGI, it is processed already by ParseReq().
									len = PrintHeader(tx_buf, type);
									wait_1ms(2);
									len += DoHTML(tx_buf + len);    // Make HTML BODY
									len = send(i, tx_buf, len);     // SEND WEB PAGE
									break;
								case 'a':
									Length = PrintJPEGHeader(TXBuf);	// Create Jpeg Web Page Header
									wait_1ms(2);
									if (IMAGE_RDY == 1)			// if completed image is number 1
									{
										EX1 = 0;
										Length += DoJPEG(TXBuf + Length, IMAGE1_SIZE + cnt_header);	// Convert size of Jpeg Header and Jpeg Image Data to string then store it to send buffer
										Length = send(i, TXBuf, Length);				// send the total size of Jpeg
										Length = send(i, Header, cnt_header);				// send Jpeg Header
										Length = send(i, IMAGE1_BUF, IMAGE1_SIZE);			// send Jpeg Image Data
										EX1 = 1;
									}
									else if (IMAGE_RDY == 0)		// If completed image is number 0
									{
										EX1 = 0;
										Length += DoJPEG(TXBuf + Length, IMAGE0_SIZE + cnt_header);
										Length = send(i, TXBuf, Length);
										Length = send(i, Header, cnt_header);
										Length = send(i, IMAGE0_BUF, IMAGE0_SIZE);
										EX1 = 1;
									} 
									break;
							}// END OF SWITCH
						} //END OF if ((len = select(i, SEL_RECV)) > 0 && STAT == 6)
					//} //END OF if(HTTP_NC_Flag == 1)
				break;
			case SOCK_CLOSE_WAIT :						// Terminate connection
				//printf("CLOSE_WAIT : %bd\r\n", i);
				PutHTOA(i); PutStringLn(" : CLOSE_WAIT");
				close(i); 						// SOCKET CLOSE
				break;
			case SOCK_CLOSED :						// Close conneciton
				//printf("CLOSED : %bd\r\n", i);
				PutHTOA(i);PutStringLn(" : CLOSED");
				init_sock(i);						
				break;
			}
		}	// for
	} // while 
}

/*
********************************************************************************
*              8051 Initialization Function
*
* Description: 
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/
void Init8051(void)
{ 
	P1 = 0;
	EA = 0; 		// Disable all interrupt
	CKCON = 0x01;		// X2 mode
	IT0 = 1;		// interrupt level trigger
	IT1 = 1;
	EX0 = 1;		// INT 0 enable
	EX1 = 1; 		// INT 1 disable
	EA = 1; 		// Enable to all interrupt
	InitSerial();		// Initialize serial port (Refer to serial.c)
	wait_10ms(1);
}

/*
********************************************************************************
*              Re-initialization function for the disconnected channel.
*
* Description: Close and re-initialize for the disconnected channel.
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/
void init_sock(u_char i) 
{
//	printf("socket : ");
	PutString("socket : ");	
	socket(i, SOCK_STREAM, 80, 0);		// Create socket
//	printf("%bd ok\r\n",i);
	PutHTOA(i);PutStringLn("");

//	printf("listen : ");
	PutString("listen : ");
	NBlisten(i);           			// Server Mode
//	printf("%bd ok..\r\n", i);
	PutHTOA(i);PutStringLn("");
}


/*
********************************************************************************
* Description: Setup network information ( Source IP, G/W, S/N, MAC Address)
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/
void InitNetConfig(void)
{
	int i, j;
	char c;
	un_l2cval tip;
	u_char xdata ip[6];  	       		// Variable to setup network information
	u_char xdata ipstr[16];
	if(!Check_EEPROM())
	{
		ip[0] = 0x00; ip[1] = 0x01; ip[2] = 0x02; ip[3] = 0x03; ip[4] = 0x04; ip[5] = 0x05;     // ETC.
		setMACAddr(ip);		// Setup MAC
		ip[0] = 192; ip[1] = 168; ip[2] = 0; ip[3] = 2;	// VPN Env.						
		setIP(ip);		// Setup source IP
		ip[3] = 1;		// VPN , Develope Env.
		setgateway(ip);		// Setup gateway address
		ip[0] = 255; ip[1] = 255; ip[2] = 255; ip[3] = 0;
		setsubmask(ip);		// Setup subnet mask
		PutStringLn("Check_EEPROM() = 0");
	}
	else
	{
		ClrScr();
		GotoXY(0,0);
		Puts(" < NET CONFIG > ");
		T0 = 0;
		T1 = 1;

		EEP_ReadBytes(GIPADDR,tip.cVal,4);         // If IP address wasn't set up in EEPROM, setup initial network configuration.
		if(tip.lVal == 0 || tip.lVal == (0-1))     // If Gateway Address is 00.00.00.00 or ff.ff.ff.ff
		{       			
			PutString("Setting Initial Network Configuration ...");PutStringLn("");PutStringLn(""); 
			ip[0] = 0x00; ip[1] = 0x08; ip[2] = 0xDC; ip[3] = 0x00; ip[4] = 0x00; ip[5] = 0x00;     // etc.
			setMACAddr(ip);		// Setup MAC
			ip[0] = 192; ip[1] = 168; ip[2] = 0; ip[3] = 2;		// VPN Env.						
			setIP(ip);		// Setup source IP
			ip[3] = 1;		// VPN , Develope Env.
			setgateway(ip);		// Setup gateway address
			ip[0] = 255; ip[1] = 255; ip[2] = 255; ip[3] = 0;
			setsubmask(ip);
			Config_Save('A');
			PutStringLn("Check_EEPROM() = 1 and tip.lVal == 0 || tip.lVal == (0-1)");
		}

		PutString("Press 'C' Key To Update Network Configuration");
		GotoXY(0,1);
		for(i = 0; i < 16; i++)
		{
			for( j = 0 ; j < 50 ; j++)
			{
				if(IsPressedKey() == 1)
				{
					c = GetByte();PutStringLn("");
					if(c == 'C' || c == 'c')
					{
						PutStringLn("Calling Configure()");
						Configure();
						c = '*';
						break;
					}
					else if(c== 0x1B)	// Escape character
					{
						c = '*';
						break;
					}
				}
				wait_1ms(2);
			}
			if(c == '*') break;
			T0 = !T0;			    	// LED TOGGLE
			T1 = !T1;
			Putch(0xFF);
			PutByte('.');
		}
		PutStringLn("");
		Config_Load();
	}
	GetNetConfig();						// Display network information
	sysinit(0xFF,0x55);
	ClrScr();
	GotoXY(0,0);
	Puts("CMERI WEBSERVER");
	GotoXY(1,1);
	GetIPAddress(ip);
	inet_ntoa(ip,ipstr);					// Output IP Address on LCD
	Puts(ipstr);
	T0 = 0;							// LED0 ON 
	T1 = 0;							// LED1 ON 
	wait_10ms(50);
	T0 = 1;
	T1 = 1;
}
/*void PutIP(void)
{
	u_char xdata newip[4];
	PutStringLn("Setting IP 192.168.0.121");
	newip[0] = 192; newip[1] = 168; newip[2] = 0; newip[3] = 121;		// VPN Env.						
	setIP(newip);		// Setup source IP
	Config_Save('I');
}*/
