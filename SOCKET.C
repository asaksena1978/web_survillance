/*
********************************************************************************
* Wiznet.
* 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
* Seoul, Korea
*
* (c) Copyright 2002, Wiznet, Seoul, Korea
*
* Filename      : socket.c
* Programmer(s) : Wooyoul Kim
* Version       : 2.2 
* Created       : 2002/04/10
* Modified      : 2002/06/20 - added delay in initseqnum() function
		               to complete write operation of TX Pointer Registers.
		  2002/09/27 - Vesion Up
                             - Global Interrupt Status Variable & MACRO Renaming
				      S_STATUS[] --> I_STATUS[]
				      INT_STATUS --> INT_REG
				      STATUS(i) --> INT_STATUS(i)
				      C_STATUS(i) --> SOCK_STATUS(i)
		   	     - When you read UDP or IP RAW Header at recvfrom() function, Verify Rx_Pointer Wrap-off
			     - At close() function, Modify "while(S_STATUS[s] !=  SCLOSED)" statement to while(!(S_STATUS[s] & SCLOSED)).
			     - At sendto()function, Checke previous send command before destination update
                                                    Error handling
                             - At select() and recvfrom() function,  
                                       Received UDP data process is modified 'packet size' to 'bulk size'
		  2002/10/20 - Version Up
			     - At select() function,
                                       Modify to caculate free buffer size at UDP,IP_RAW protocol
		  2002/10/28 - Version Up
			     - At select(), integrated each received size caculation of TCP, UDP or IP_RAW into one calculation.
			     - At recvfrom(), Received data is processed by packet unit.
                                       
* Description   : Driver API program of W3100A (Big Endian for Keil Compiler)
********************************************************************************
*/

/*
###############################################################################
File Include Section
###############################################################################
*/
#include <at89x51.h>                   // 8051 SFR definition file
//#include <stdio.h>
#include "serial.h"                    // serial related functions
#include "socket.h"                    // W3100A driver file
#include "type.h"

/*
###############################################################################
Define Part
###############################################################################
*/
//#define DEBUG                        // Add debugging code

/*
###############################################################################
Local Variable Declaration Section
###############################################################################
*/
u_char xdata I_STATUS[4];              // Store Interrupt Status according to channels
u_int xdata Local_Port;                // Designate Local Port
un_l2cval xdata SEQ_NUM;               // Set initial sequence number
                                                
u_long xdata SMASK[MAX_SOCK_NUM];      // Variable to store MASK of Tx in each channel, on setting dynamic memory size.
u_long xdata RMASK[MAX_SOCK_NUM];      // Variable to store MASK of Rx in each channel, on setting dynamic memory size.
int xdata SSIZE[MAX_SOCK_NUM];         // Maximun Tx memory size by each channel
int xdata RSIZE[MAX_SOCK_NUM];         // Maximun Rx memory size by each channel
u_char xdata* SBUFBASEADDRESS[MAX_SOCK_NUM];     // Maximun Tx memory base address by each channel
u_char xdata* RBUFBASEADDRESS[MAX_SOCK_NUM];     // Maximun Rx memory base address by each channel
extern unsigned char HTTP_NC_Flag;

/*
###############################################################################
Function Implementation Section
###############################################################################
*/


/*
********************************************************************************
*               Interrupt handling function of the W3100A
*
* Description :
*   Stores the status information that each function waits for in the global variable S_STATUS for transfer.
*   S_STATUS stores the interrupt status value for each channel.
* Arguments   : None
* Returns     : None
* Note        : Internal Function
********************************************************************************
*/
void Int0(void) interrupt 0
{
	u_char status;

	EX0 = 0;                      	  // INT0 DISABLE

	status = INT_REG;
	//HTTP_NC_Flag = 1;				//Set Flag 1 to recognise the interrupt is for HTTP
	//PutStringLn("I am in EX. Interrupt 0");

	while (status) {
		if (status & 0x01) {      // channel 0 interrupt(sysinit, sockinit, established, closed, timeout, send_ok, recv_ok)
			I_STATUS[0] = INT_STATUS(0);

//			if (I_STATUS[0] & SESTABLISHED) ISR_ESTABLISHED(0);
//			if (I_STATUS[0] & SCLOSED) ISR_CLOSED(0);

			INT_REG = 0x01;
		}

		if (status & 0x02) {      // channel 1 interrupt(sysinit, sockinit, established, closed, timeout, send_ok, recv_ok)
			I_STATUS[1] = INT_STATUS(1);

//			if (I_STATUS[1] & SESTABLISHED) ISR_ESTABLISHED(1);
//			if (I_STATUS[1] & SCLOSED) ISR_CLOSED(1);

			INT_REG = 0x02;
		}

		if (status & 0x04) {      // channel 2 interrupt(sysinit, sockinit, established, closed, timeout, send_ok, recv_ok)
			I_STATUS[2] = INT_STATUS(2);

//			if (I_STATUS[2] & SESTABLISHED) ISR_ESTABLISHED(2);
//			if (I_STATUS[2] & SCLOSED) ISR_CLOSED(2);

			INT_REG = 0x04;
		}

		if (status & 0x08) {      // channel 3 interrupt(sysinit, sockinit, established, closed, timeout, send_ok, recv_ok)
			I_STATUS[3] = INT_STATUS(3);

//			if (I_STATUS[3] & SESTABLISHED) ISR_ESTABLISHED(3);
//			if (I_STATUS[3] & SCLOSED) ISR_CLOSED(3);

			INT_REG = 0x08;
		}

		if (status & 0x10) {      // channel 0 receive interrupt
//			ISR_RX(0);
			INT_REG = 0x10;
		}

		if (status & 0x20) {      // channel 1 receive interrupt
//			ISR_RX(1);
			INT_REG = 0x20;
		}

		if (status & 0x40) {      // channel 2 receive interrupt
//			ISR_RX(2);
			INT_REG = 0x40;
		}

		if (status & 0x80) {      // channel 3 receive interrupt
//			ISR_RX(3);
			INT_REG = 0x80;
		}

		status = INT_REG;
	}

	INT_REG = 0xFF;

	EX0 = 1;
}

/*
********************************************************************************
*               Established connection interrupt handling function.
*
* Description : 
*   Called upon connection establishment, and may be inserted in user code if needed by the programmer.
* Arguments   : None
* Returns     : None
* Note        : Internal Function
********************************************************************************
*/
/*
void ISR_ESTABLISHED(SOCKET s) 
{
	// TO ADD YOUR CODE
}
*/

/*
********************************************************************************
*               Closed connection interrupt handling function
*
* Description :  
*   Called upon connection closure, and may be inserted in user code if needed by the programmer.
* Arguments   : None
* Returns     : None
* Note        : Internal Function
********************************************************************************
*/
/*
void ISR_CLOSED(SOCKET s) 
{
	// TO ADD YOUR CODE
}
*/

/*
********************************************************************************
*               Received data interrupt handling function
*
* Description :  
*   Called upon receiving data, and may be inserted in user code if needed by the programmer.
* Arguments   : None
* Returns     : None
* Note        : Internal Function
********************************************************************************
*/
/*
void ISR_RX(SOCKET s) 
{
	// TO ADD YOUR CODE
}
*/

/*
********************************************************************************
*               W3100A initialization function
*
* Description :
*   Function for S/W resetting of the W3100A.
*   Sets the initial SEQ# to be used for TCP communication.
* Arguments   : None
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void initW3100A(void)
{
#ifdef DEBUG
//	printf("initW3100A()\r\n");
    PutStringLn("initW3100A()");
#endif 
    Local_Port = 1000;          // This default value will be set if you didn't designate it when you create a socket
                                // If you don't designate port number and create a socket continuously,
                                // the port number will be assigned with incremented by one to Local_Port
    SEQ_NUM.lVal = 0x12233445;  // Sets the initial SEQ# to be used for TCP communication. (It should be ramdom value)
    COMMAND(0) = CSW_RESET;     // Software RESET
}

/*
********************************************************************************
*               W3100A initialization function
*
* Description : 
*   Sets the Tx, Rx memory size by each channel, source MAC, source IP, gateway, and subnet mask
*   to be used by the W3100A to the designated values.
*   May be called when reflecting modified network information or Tx, Rx memory size on the W3100A
*   Include Ping Request for ARP update (In case that a device embedding W3100A is directly connected to Router)
* Arguments   : sbufsize - Tx memory size (00 - 1KByte, 01- 2KBtye, 10 - 4KByte, 11 - 8KByte)
*                          bit 1-0 : Tx memory size of channel #0 
*                          bit 3-2 : Tx memory size of channel #1 
*                          bit 5-4 : Tx memory size of channel #2
*                          bit 7-6 : Tx memory size of channel #3 
*               rbufsize - Rx memory size (00 - 1KByte, 01- 2KBtye, 10 - 4KByte, 11 - 8KByte)
*                          bit 1-0 : Rx memory size of channel #0 
*                          bit 3-2 : Rx memory size of channel #1 
*                          bit 5-4 : Rx memory size of channel #2
*                          bit 7-6 : Rx memory size of channel #3 
* Returns     : None
* Note        : API Function
*               Maximum memory size for Tx, Rx in W3100A is 8KBytes,
*               In the range of 8KBytes, the memory size could be allocated dynamically by each channel 
*               Be attentive to sum of memory size shouldn't exceed 8Kbytes
*               and to data transmission and receiption from non-allocated channel may cause some problems.
*               If 8KBytes memory already is assigned to centain channel, other 3 channels couldn't be used, for there's no available memory.
*               If two 4KBytes memory are assigned to two each channels, other 2 channels couldn't be used, for there's no available memory.
*               (Example of memory assignment)
*                sbufsize => 00000011, rbufsize => 00000011 : Assign 8KBytes for Tx and Rx to channel #0, Cannot use channel #1,#2,#3
*                sbufsize => 00001010, rbufsize => 00001010 : Assign 4KBytes for Tx and Rx to each channel #0,#1 respectively. Cannot use channel #2,#3
*                sbufsize => 01010101, rbufsize => 01010101 : Assign 2KBytes for Tx and Rx to each all channels respectively.
*                sbufsize => 00010110, rbufsize => 01010101 : Assign 4KBytes for Tx, 2KBytes for Rx to channel #0
*                                                             2KBytes for Tx, 2KBytes for Rx to channel #1
*                                                             2KBytes for Tx, 2KBytes for Rx to channel #2
*                                                             2KBytes is available exclusively for Rx in channel #3. There's no memory for Tx.
********************************************************************************
*/
void sysinit(u_char sbufsize, u_char rbufsize)
{
	char i;
	int ssum,rsum;

	ssum = 0;
	rsum = 0;
	
	TX_DMEM_SIZE = sbufsize;                 // Set Tx memory size for each channel
	RX_DMEM_SIZE = rbufsize;                 // Set Rx memory size for each channel
	
	SBUFBASEADDRESS[0] = SEND_DATA_BUF;      // Set Base Address of Tx memory for channel #0
	RBUFBASEADDRESS[0] = RECV_DATA_BUF;      // Set Base Address of Rx memory for channel #0

#ifdef DEBUG
		PutStringLn("Channel : SEND MEM SIZE : RECV MEM SIZE");
#endif

	for(i = 0 ; i < MAX_SOCK_NUM; i++)               // Set maximum memory size for Tx and Rx, mask, base address of memory by each channel
	{
		SSIZE[i] = 0;
		RSIZE[i] = 0;
		if(ssum < 8192)
		{
			switch((sbufsize >> i*2) & 0x03) // Set maximum Tx memory size
			{
			case 0:
				SSIZE[i] = 1024;
				SMASK[i] = 0x000003FF;
				break;
			case 1:
				SSIZE[i] = 2048;
				SMASK[i] = 0x000007FF;
				break;
			case 2:
				SSIZE[i] = 4096;
				SMASK[i] = 0x00000FFF;
				break;
			case 3:
				SSIZE[i] = 8192;
				SMASK[i] = 0x00001FFF;
				break;
			}
		}
		if( rsum < 8192)
		{
			switch((rbufsize>> i*2) & 0x03)  // Set maximum Rx memory size
			{
			case 0:
				RSIZE[i] = 1024;
				RMASK[i] = 0x000003FF;
				break;
			case 1:
				RSIZE[i] = 2048;
				RMASK[i] = 0x000007FF;
				break;
			case 2:
				RSIZE[i] = 4096;
				RMASK[i] = 0x00000FFF;
				break;
			case 3:
				RSIZE[i] = 8192;
				RMASK[i] = 0x00001FFF;
				break;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];

		if(i != 0)                               // Set base address of Tx and Rx memory for channel #1,#2,#3
		{
			SBUFBASEADDRESS[i] = SBUFBASEADDRESS[i-1] + SSIZE[i-1];
			RBUFBASEADDRESS[i] = RBUFBASEADDRESS[i-1] + RSIZE[i-1];
		}
#ifdef DEBUG
		PutHTOA(i); PutString("      :    0x");PutITOA(SSIZE[i]); PutString("     :    0x");PutITOA(RSIZE[i]);PutStringLn("");
#endif
	}
	I_STATUS[0] = 0;
	COMMAND(0) = CSYS_INIT;
	while(!(I_STATUS[0] & SSYS_INIT_OK));
}

/*
********************************************************************************
*               Subnet mask setup function
*
* Description : Subnet mask setup function
* Arguments   : addr - pointer having the value for setting up the Subnet Mask
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void setsubmask(u_char * addr)
{
	u_char i;

	for (i = 0; i < 4; i++) {
		*(SUBNET_MASK_PTR + i) = addr[i];
	}
}

/*
********************************************************************************
*               gateway IP setup function
*
* Description : gateway IP setup function
* Arguments   : addr - pointer having the value for setting up the gateway IP
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void setgateway(u_char * addr)
{
	u_char i;

	for (i = 0; i < 4; i++) {
		*(GATEWAY_PTR + i) = addr[i];
	}
}

/*
********************************************************************************
*               W3100A IP Address setup function
*
* Description : W3100A IP Address setup function
* Arguments   : addr - pointer having the value for setting up the source IP Address
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void setIP(u_char * addr)
{
	u_char i;

	for (i = 0; i < 4; i++) {
		*(SRC_IP_PTR + i) = addr[i];
	}
}

/*
********************************************************************************
*               MAC Address setup function
*
* Description : MAC Address setup function
* Arguments   : addr - pointer having the value for setting up the MAC Address
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void setMACAddr(u_char * addr)
{
	u_char i;

	for (i = 0; i < 6; i++) {
		*(SRC_HA_PTR + i) = addr[i];
	}
}

#ifdef __IP_RAW__
/*
********************************************************************************
*               Upper layer protocol setup function in IP RAW Mode
*
* Description : Upper layer protocol setup function in protocol field of IP header when
*                    developing upper layer protocol like ICMP, IGMP, EGP etc. by using IP Protocol
* Arguments   : s          - Channel number
*               ipprotocol - Upper layer protocol setting value of IP Protocol
*                            (Possible to use designated IPPROTO_ in header file)
* Returns     : None
* Note        : API Function
*                  This function should be called before calling socket() that is, before socket initialization.
********************************************************************************
*/
void setIPprotocol(SOCKET s, u_char ipprotocol)
{
	IP_PROTOCOL(s) = ipprotocol;
}
#endif


#ifdef	__OPT__

/*
********************************************************************************
*               TCP timeout setup function
*
* Description : TCP retransmission time setup function.
*     Timeout Interrupt occurs if the number of retransmission exceed the limit when establishing connection and data transmission. 
* Arguments   : val - Pointer having the value to setup timeout
*     Upper 2byte is for initial timeout, lower 1byte is for the number of retransmission by timeout
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void settimeout(u_char * val)
{
	u_char i;

	for (i = 0; i < 3; i++) {
		*(TIMEOUT_PTR + i) = val[i];
	}
}

/*
********************************************************************************
*               interrupt mask setup function
*
* Description : Interrupt mask setup function. Enable/Disable appropriate Interrupt.
* Arguments   : mask - mask value to setup ('1' : interrupt enable)
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void setINTMask(u_char mask)
{
	INTMASK = mask;
}

/*
********************************************************************************
*               TOS value setup function for TOS field of IP header
*
* Description : TOS value setup function for TOS field of IP header
* Arguments   : s   - channel number
*    tos - Valuse to setup for TOS field of IP Header
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void setTOS(SOCKET s, u_char tos)
{
	TOS(s) = tos;
}
#endif

/*
********************************************************************************
*               Initialization function to appropriate channel
*
* Description : Initialize designated channel and wait until W3100 has done.
* Arguments   : s - channel number
*               protocol - designate protocol for channel
*                          SOCK_STREAM(0x01) -> TCP.
*                          SOCK_DGRAM(0x02)  -> UDP.
*                          SOCK_IPL_RAW(0x03) -> IP LAYER RAW.
*                          SOCK_MACL_RAW(0x04) -> MAC LAYER RAW.
*               port     - designate source port for appropriate channel
*               flag     - designate option to be used in appropriate.
*                          SOCKOPT_BROADCAST(0x80) -> Send/receive broadcast message in UDP
*                          SOCKOPT_NDTIMEOUT(0x40) -> Use register value which designated TIMEOUT value
*                          SOCKOPT_NDACK(0x20)     -> When not using no delayed ack
*                          SOCKOPT_SWS(0x10)       -> When not using silly window syndrome
* Returns     : When succeeded : Channel number, failed :1
* Note        : API Function
********************************************************************************
*/
char socket(SOCKET s, u_char protocol, u_int port, u_char flag)
{
	u_char k;

	OPT_PROTOCOL(s) = protocol | flag;                // Designate socket protocol and option

	if (port != 0)                                    // setup designated port number
	 {
		k = (u_char)((port & 0xff00) >> 8);
		*(SRC_PORT_PTR(s)) = k;
		k = (u_char)(port & 0x00ff);
		*(SRC_PORT_PTR(s) + 1) = k;
	} 
	else                                              // Designate random port number which is managed by local when you didn't designate source port
	{
		Local_Port++;					
		*SRC_PORT_PTR(s) = (u_char)((Local_Port & 0xff00) >> 8);
		*(SRC_PORT_PTR(s) + 1) = (u_char)(Local_Port & 0x00ff);
	}

	I_STATUS[s] = 0;
	COMMAND(s) = CSOCK_INIT;                          // SOCK_INIT
	while (I_STATUS[s] == 0);                         // Waiting Interrupt to CSOCK_INIT

	if (!(I_STATUS[s] & SSOCK_INIT_OK)) return (-1);  // Error
	
	initseqnum(s);                                    // Use initial seq# with random number

	return	(s);
}

#ifdef	__TCP_CLIENT__
/*
********************************************************************************
*               Connection establishing function to designated peer.
*
* Description : This function establish a connection to the peer by designated channel,
*     and wait until the connection is established successfully. (TCP client mode)
* Arguments   : s    - channel number
*               addr - destination IP Address
*               port - destination Port Number
* Returns     : when succeeded : 1, failed : -1
* Note        : API Function
********************************************************************************
*/
char connect(SOCKET s, u_char * addr, u_int port)
{
	if (port != 0) 
	{                                                           // designate destination port
		*DST_PORT_PTR(s) = (u_char)((port & 0xff00) >> 8);
		*(DST_PORT_PTR(s) + 1) = (u_char)(port & 0x00ff);
	}
	else    return (-1);

	*DST_IP_PTR(s) = addr[0];                                   // designate destination IP address
	*(DST_IP_PTR(s) + 1) = addr[1];
	*(DST_IP_PTR(s) + 2) = addr[2];        
	*(DST_IP_PTR(s) + 3) = addr[3];

	I_STATUS[s] = 0;

	COMMAND(s) = CCONNECT;                                      // CONNECT

	
	while (I_STATUS[s] == 0)                                    // Wait until connection is established successfully
		if (select(s, SEL_CONTROL) == SOCK_CLOSED) return -1;   // When failed, appropriate channel will be closed and return an error

	if (!(I_STATUS[s] & SESTABLISHED)) return (-1);             // Error

    return	(1);
}
#endif

#ifdef	__TCP_SERVER__
/*
********************************************************************************
*               Waits for connection request from a peer (Blocking Mode)
*
* Description : Wait for connection request from a peer through designated channel (TCP Server mode)
* Arguments   : s    - channel number
*               addr - IP Address of the peer when a connection is established
*               port - Port number of the peer when a connection is established 
* Returns     : When succeeded : 1, failed : -1
* Note        : API Function
********************************************************************************
*/
/*
char listen(SOCKET s, u_char * addr, u_int * port)
{
	u_int i;

	I_STATUS[s] = 0;

	COMMAND(s) = CLISTEN;                                       // LISTEN

	while (I_STATUS[s] == 0)                                    // Wait until connection is established
	    if (select(s, SEL_CONTROL) == SOCK_CLOSED) return -1;   // When failed to connect, the designated channel will be closed and return an error.

	if (I_STATUS[s] & SESTABLISHED)                             // Receive IP address and port number of the peer connected
	{ 
		i = *DST_PORT_PTR(s);
		*port = (u_int)((i & 0xff00) >> 8);
		i = *(DST_PORT_PTR(s) + 1);
		i = (u_int)(i & 0x00ff);
		*port += (i << 8);

		addr[0] = *DST_IP_PTR(s);
		addr[1] = *(DST_IP_PTR(s) + 1);
		addr[2] = *(DST_IP_PTR(s) + 2);
		addr[3] = *(DST_IP_PTR(s) + 3);
	} 
	else	return (-1);                                        // Error

	return (1);
}
*/

/*
********************************************************************************
*              Waits for connection request from a peer (Non-blocking Mode)
*
* Description : Wait for connection request from a peer through designated channel (TCP Server mode)
* Arguments   : s - channel number
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void NBlisten(SOCKET s)
{
	I_STATUS[s] = 0;
	COMMAND(s) = CLISTEN;         // LISTEN
}
#endif

/*
********************************************************************************
*               Create random value for initial Seq# when establishing TCP connection
*
* Description : In this function, you can add some source codes to create random number for initial Seq#
*     In real, TCP initial SEQ# should be random value. 
*               (Currently, we're using static value in EVB/DK.)
* Arguments   : s - channel number
* Returns     : None
* Note        : API Function
********************************************************************************
*/
void initseqnum(SOCKET s)
{
	int i;

	i = s;

	SEQ_NUM.lVal++;     // Designate initial seq#
                            // If you have random number generation function, assign random number instead of SEQ_NUM.lVal++.

	*TX_WR_PTR(s)	    = SEQ_NUM.cVal[0];
	*(TX_WR_PTR(s) + 1) = SEQ_NUM.cVal[1];
	*(TX_WR_PTR(s) + 2) = SEQ_NUM.cVal[2];
	*(TX_WR_PTR(s) + 3) = SEQ_NUM.cVal[3];
	wait_1us(2);	    // Wait until TX_WR_PRT has been written safely. ( Must have delay(1.6us) if next action is to write 4byte-pointer register )

	*TX_RD_PTR(s)	    = SEQ_NUM.cVal[0];
	*(TX_RD_PTR(s) + 1) = SEQ_NUM.cVal[1];
	*(TX_RD_PTR(s) + 2) = SEQ_NUM.cVal[2];
	*(TX_RD_PTR(s) + 3) = SEQ_NUM.cVal[3];
	wait_1us(2);	    // Wait until TX_RD_PRT has been written safely.

	*TX_ACK_PTR(s)	     = SEQ_NUM.cVal[0];
	*(TX_ACK_PTR(s) + 1) = SEQ_NUM.cVal[1];
	*(TX_ACK_PTR(s) + 2) = SEQ_NUM.cVal[2];
	*(TX_ACK_PTR(s) + 3) = SEQ_NUM.cVal[3];
}

#ifdef	__TCP__
/*
********************************************************************************
*               Function for sending TCP data.
*
* Description : Function for sending TCP data and Composed of the send() and send_in() functions.
*     The send() function is an application I/F function.
*     It continues to call the send_in() function to complete the sending of the data up to the size of the data to be sent
*     when the application is called.
*     The send_in() function receives the return value (the size of the data sent), calculates the size of the data to be sent,
*     and calls the send_in() function again if there is any data left to be sent.
* Arguments   : s   - channel number
*               buf - Pointer pointing data to send
*               len - data size to send
* Returns     : Succeed: sent data size, Failed:  -1;
* Note        : API Function
********************************************************************************
*/
int send(SOCKET s, const u_char xdata* buf, u_int len)
{
	int ptr, size;

	if (len <= 0) return (0);
	else 
	{
		ptr = 0;
		do {
			size = send_in(s, buf + ptr, len);
			if (size == -1) return -1;	// Error
			len = len - size;
			ptr += size;
		} while ( len > 0);
	}

	return ptr;
}

/*
********************************************************************************
*               Internal function for sending TCP data.
*
* Description : Called by the send() function for TCP transmission.
*    It first calculates the free transmit buffer size
*    and compares it with the size of the data to be transmitted to determine the transmission size.
*    After calculating the data size, it copies data from TX_WR_PTR.
*    It waits if there is a previous send command in process.
*    When the send command is cleared, it updates the TX_WR_PTR up to the size to be transmitted and performs the send command.
* Arguments   : s   - channel number
*               buf - Pointer pointing data to send
*               len - data size to send
* Returns     : Succeeded: sent data size, Failed: -1
* Note        : Internal Function
********************************************************************************
*/
int send_in(SOCKET s, const u_char xdata * buf, u_int len)
{
	u_char k;
	int size;
	un_l2cval wr_ptr, ack_ptr;
	u_char * send_ptr;

S_START:
	EX0 = 0;
	k = *SHADOW_TXWR_PTR(s);	// Must read the shadow register for reading 4byte pointer registers
	wait_1us(2);                    // wait for reading 4byte pointer registers safely
	wr_ptr.cVal[0] = *TX_WR_PTR(s);
	wr_ptr.cVal[1] = *(TX_WR_PTR(s) + 1);
	wr_ptr.cVal[2] = *(TX_WR_PTR(s) + 2);
	wr_ptr.cVal[3] = *(TX_WR_PTR(s) + 3);

	k = *SHADOW_TXACK_PTR(s);       // Must read the shadow register for reading 4byte pointer registers
	wait_1us(2);                    // wait for reading 4byte pointer registers safely
	ack_ptr.cVal[0] = *TX_ACK_PTR(s);
	ack_ptr.cVal[1] = *(TX_ACK_PTR(s) + 1);
	ack_ptr.cVal[2] = *(TX_ACK_PTR(s) + 2);
	ack_ptr.cVal[3] = *(TX_ACK_PTR(s) + 3);
	EX0 = 1;

	// Calculate send free buffer size
	if (wr_ptr.lVal >= ack_ptr.lVal) size = SSIZE[s] - (wr_ptr.lVal - ack_ptr.lVal);
	else size = SSIZE[s] - (0 - ack_ptr.lVal + wr_ptr.lVal);

	
	if (size > SSIZE[s])                                                            // Recalulate after some delay because of error in pointer caluation
	{
		if (select(s, SEL_CONTROL) != SOCK_ESTABLISHED) return -1;              // Error

		wait_1ms(1);
#ifdef DEBUG
//		printf("1 ");
//		printf("%.8lx ", wr_ptr.lVal);
//		printf("%.8lx\r\n", ack_ptr.lVal);
		PutString("send_in() at S_START : ");PutHTOA(s); PutString(" : "); PutLTOA(wr_ptr.lVal) ; PutString(" : ");PutLTOA(ack_ptr.lVal) ;PutStringLn("");
#endif
		goto S_START;
	}

	if (size == 0)                                                                  // Wait when previous sending has not finished yet and there's no free buffer
	{
		if (select(s, SEL_CONTROL) != SOCK_ESTABLISHED) return -1;              // Error
		wait_1ms(1);
#ifdef DEBUG
		PutStringLn("send_in() at S_START : size == 0");
#endif
		goto S_START;
	}
	else if (size < len)	len = size;

	send_ptr = (u_char *)( SBUFBASEADDRESS[s] + (UINT)(wr_ptr.lVal & SMASK[s]));    // Calculate pointer to data copy
	write_data(s, buf, send_ptr, len);                                              // data copy

	while (COMMAND(s) & CSEND)                                                      // Confirm send command
		if (select(s, SEL_CONTROL) != SOCK_ESTABLISHED) return -1;                  // Error

	wr_ptr.lVal = wr_ptr.lVal + len;                                                // tx_wr_ptr update
	*TX_WR_PTR(s) = wr_ptr.cVal[0];
	*(TX_WR_PTR(s) + 1) = wr_ptr.cVal[1];
	*(TX_WR_PTR(s) + 2) = wr_ptr.cVal[2];
	*(TX_WR_PTR(s) + 3) = wr_ptr.cVal[3];

	COMMAND(s) = CSEND;                                                             // SEND

    return	(len);
}

/*
********************************************************************************
*               TCP data receiving function.
*
* Description : This function is for receiving TCP data.
*     The recv() function is an application I/F function. It continues to wait for as much data as the application wants to receive.
* Arguments   : s   - channel number
*               buf - Pointer where the data to be received is copied
*               len - Size of the data to be received
* Returns     : Succeeded: received data size, Failed: -1
* Note        : API Fcuntion
********************************************************************************
*/
int recv(SOCKET s, const u_char xdata* buf, u_int len)
{
	u_char k;
	u_int size;
	un_l2cval wr_ptr, rd_ptr;
	u_char * recv_ptr;

R_START:
	EX0 = 0;
	k = *SHADOW_RXWR_PTR(s);   	// Must read the shadow register for reading 4byte pointer registers
	wait_1us(2);                    // wait for reading 4byte pointer registers safely
	wr_ptr.cVal[0] = *RX_WR_PTR(s);
	wr_ptr.cVal[1] = *(RX_WR_PTR(s) + 1);
	wr_ptr.cVal[2] = *(RX_WR_PTR(s) + 2);
	wr_ptr.cVal[3] = *(RX_WR_PTR(s) + 3);

	k = *SHADOW_RXRD_PTR(s);        // Must read the shadow register for reading 4byte pointer registers
	wait_1us(2);                    // wait for reading 4byte pointer registers safely
	rd_ptr.cVal[0] = *RX_RD_PTR(s);
	rd_ptr.cVal[1] = *(RX_RD_PTR(s) + 1);
	rd_ptr.cVal[2] = *(RX_RD_PTR(s) + 2);
	rd_ptr.cVal[3] = *(RX_RD_PTR(s) + 3);
	EX0 = 1;

	// calculate received data size
	if ( len <= 0 ) return (0);
	else if (wr_ptr.lVal >= rd_ptr.lVal) size = wr_ptr.lVal - rd_ptr.lVal;
	else size = 0 - rd_ptr.lVal + wr_ptr.lVal;

	
	if (size < len)                                                                 // Wait until receiving is done when received data size is less then len
	{
		if (select(s, SEL_CONTROL) != SOCK_ESTABLISHED) return -1;              // Error
		wait_1ms(1);  
#ifdef DEBUG
//		printf("size < len\r\n");
		PutStringLn("size < len");
#endif
		goto R_START;
	}

	recv_ptr = (u_char *) (RBUFBASEADDRESS[s] + (UINT)(rd_ptr.lVal & RMASK[s]));    // Calculate pointer to be copied received data

	read_data(s, recv_ptr, buf, len);                                               // Copy receibed data

	rd_ptr.lVal += len;                                                             // Update rx_rd_ptr
	*RX_RD_PTR(s) = rd_ptr.cVal[0];
	*(RX_RD_PTR(s) + 1) = rd_ptr.cVal[1];
	*(RX_RD_PTR(s) + 2) = rd_ptr.cVal[2];
	*(RX_RD_PTR(s) + 3) = rd_ptr.cVal[3];

	COMMAND(s) = CRECV;                                                             // RECV

    return	(len);
}
#endif	// __TCP__

#ifdef	__UDP__
/*
********************************************************************************
*               UDP data sending function.
*
* Description : Composed of the sendto()and sendto_in() functions.
*    The send() function is an application I/F function.
*    It continues to call the send_in() function to complete the sending of the data up to the size of the data to be sent
*    when the application is called.Unlike TCP transmission, it designates the destination address and the port.
* Arguments   : s    - channel port
*               buf  - Pointer pointing data to send
*               len  - data size to send
*               addr - destination IP address to send data
*               port - destination port number to send data
* Returns     : Sent data size
* Note        : API Function
********************************************************************************
*/
u_int sendto(SOCKET s, const u_char xdata* buf, u_int len, u_char * addr, u_int port)
{
	int ptr, size;

	while(COMMAND(s) & CSEND)				    // Wait until previous send commnad has completed
		if(select(s,SEL_CONTROL) == SOCK_CLOSED) return -1; // Errors.

	if (port != 0)                                              // Designate destination port number
	{						
		*DST_PORT_PTR(s) = (u_char)((port & 0xff00) >> 8);
		*(DST_PORT_PTR(s) + 1) = (u_char)(port & 0x00ff);
	}

	*DST_IP_PTR(s) = addr[0];                                   // Designate destination IP address
	*(DST_IP_PTR(s) + 1) = addr[1];
	*(DST_IP_PTR(s) + 2) = addr[2];
	*(DST_IP_PTR(s) + 3) = addr[3];


#ifdef DEBUG
//	printf("sendto Parameter\r\n");
//	printf("Destination Port : %d\r\n", (u_int)*(DST_PORT_PTR(s)));
//	printf("Destination IP : %d.%d.%d.%d\r\n", *DST_IP_PTR(s) ,*DST_IP_PTR(s)+1, *DST_IP_PTR(s)+2, *DST_IP_PTR(s)+3);
	PutStringLn("sendto Parameter");
	PutString("Destination Port : ");PutHTOA(*DST_PORT_PTR(s));PutHTOA(*(DST_PORT_PTR(s)+1));PutStringLn("");
	PutString("Destination IP : "); 
	PutHTOA(*DST_IP_PTR(s)); PutByte('.'); PutHTOA(*(DST_IP_PTR(s)+1)); PutByte('.'); PutHTOA(*(DST_IP_PTR(s)+2)); PutByte('.'); PutHTOA(*(DST_IP_PTR(s)+3));
	PutStringLn("");
#endif
	if (len <= 0) return (0);
	else 
	{
		ptr = 0;
		do{
			size = sendto_in(s, buf + ptr, len);
			if(size == -1) return -1;	// Error
			len = len - size;
			ptr += size;
		}while ( len > 0);
	}
	return ptr;
}
/*
********************************************************************************
*               UDP data sending function.
*
* Description : An internal function that is the same as the send_in() function of the TCP.
* Arguments   : s   - Channel number
*               buf - Pointer indicating the data to send
*               len - data size to send
* Returns     : Sent data size
* Note        : Internal Function
********************************************************************************
*/
u_int sendto_in(SOCKET s, const u_char xdata* buf, u_int len)
{
	u_char k;
	u_int size;
	un_l2cval wr_ptr, rd_ptr;
	u_char * send_ptr;

S2_START:
	if(select(s,SEL_CONTROL)==SOCK_CLOSED) return -1;	// Error
	EX0 = 0;
	k = *SHADOW_TXWR_PTR(s);
	wait_1us(2);
	wr_ptr.cVal[0] = *TX_WR_PTR(s);
	wr_ptr.cVal[1] = *(TX_WR_PTR(s) + 1);
	wr_ptr.cVal[2] = *(TX_WR_PTR(s) + 2);
	wr_ptr.cVal[3] = *(TX_WR_PTR(s) + 3);

	k = *SHADOW_TXRD_PTR(s);
	wait_1us(2);
	rd_ptr.cVal[0] = *TX_RD_PTR(s);
	rd_ptr.cVal[1] = *(TX_RD_PTR(s) + 1);
	rd_ptr.cVal[2] = *(TX_RD_PTR(s) + 2);
	rd_ptr.cVal[3] = *(TX_RD_PTR(s) + 3);
        EX0 = 1;

	// Calculate free buffer size to send
	if (wr_ptr.lVal >= rd_ptr.lVal) size = SSIZE[s] - (wr_ptr.lVal - rd_ptr.lVal);
	else size = SSIZE[s] - (0 - rd_ptr.lVal + wr_ptr.lVal);
	
	if (size > SSIZE[s])                                                            // Recalulate after some delay because of error in pointer caluation
	{
		wait_1ms(1);
#ifdef DEBUG
		PutString("sendto_in() at S2_START : ");PutHTOA(s); PutString(" : "); PutLTOA(wr_ptr.lVal) ; PutString(" : ");PutLTOA(rd_ptr.lVal) ;PutStringLn("");
#endif
		goto S2_START;
	}

	if (size == 0)                                                                  // Wait when previous sending has not finished yet and there's no free buffer
	{
		wait_1ms(1);
#ifdef DEBUG
		PutString("sendto_in() at S2_START : size == 0");
#endif
		goto S2_START;
	} 
	else if (size < len)	len = size;

	send_ptr = (u_char *)(SBUFBASEADDRESS[s] + (UINT)(wr_ptr.lVal & SMASK[s]));     // Calculate pointer to copy data pointer

	write_data(s, buf, send_ptr, len);                                              // Copy data

	while (COMMAND(s) & CSEND)							// Confirm previous send command
		if(select(s,SEL_CONTROL)==SOCK_CLOSED) return -1;                       // Error

	wr_ptr.lVal = wr_ptr.lVal + len;                                                // Update tx_wr_ptr
	*TX_WR_PTR(s) = wr_ptr.cVal[0];
	*(TX_WR_PTR(s) + 1) = wr_ptr.cVal[1];
	*(TX_WR_PTR(s) + 2) = wr_ptr.cVal[2];
	*(TX_WR_PTR(s) + 3) = wr_ptr.cVal[3];

	COMMAND(s) = CSEND;                                                             // SEND

    return	(len);
}
/*
********************************************************************************
*               UDP or IP data receiving function.
*
* Description : Function for receiving UDP and IP layer RAW mode data, and handling the data header.
* Arguments   : s    - channel number
*               buf  - Pointer where the data to be received is copied
*               len  - any number greater than zero.
*               addr - Peer IP address for receiving
*               port - Peer port number for receiving
* Returns     : Data size of received packet 
* Note        : API Function
********************************************************************************
*/
u_int recvfrom(SOCKET s, const u_char xdata * buf, u_int len,u_char * addr, u_int * port)
{
	struct _UDPHeader                      // When receiving UDP data, header added by W3100A
	{
		union 
		{
			struct
			 { 
				u_int size; 
				u_char addr[4]; 
				u_int port;
			}header;
			u_char stream[8];
        	}u;
	}xdata UDPHeader;

	u_int ret;
	u_char * recv_ptr;
	un_l2cval wr_ptr, rd_ptr;
	u_long size;
	u_char k;

	if(select(s,SEL_CONTROL)==SOCK_CLOSED) return -1;
	EX0 = 0;
	k = *SHADOW_RXWR_PTR(s);
	wait_1us(2);
	wr_ptr.cVal[0] = *RX_WR_PTR(s);
	wr_ptr.cVal[1] = *(RX_WR_PTR(s) + 1);
	wr_ptr.cVal[2] = *(RX_WR_PTR(s) + 2);
	wr_ptr.cVal[3] = *(RX_WR_PTR(s) + 3);

	k = *SHADOW_RXRD_PTR(s);
	wait_1us(2);
	rd_ptr.cVal[0] = *RX_RD_PTR(s);
	rd_ptr.cVal[1] = *(RX_RD_PTR(s) + 1);
	rd_ptr.cVal[2] = *(RX_RD_PTR(s) + 2);
	rd_ptr.cVal[3] = *(RX_RD_PTR(s) + 3);
	EX0 = 1;

	// Calculate received data size
	if(len <=0) return 0;
	else if (wr_ptr.lVal >= rd_ptr.lVal) size = wr_ptr.lVal - rd_ptr.lVal;
	else size = 0 - rd_ptr.lVal + wr_ptr.lVal;

	if(size == 0) return 0;
	
	recv_ptr = (u_char *)(RBUFBASEADDRESS[s] + (UINT)(rd_ptr.lVal & RMASK[s]));     // Calulate received data pointer

	if ((OPT_PROTOCOL(s) & 0x07) == SOCK_DGRAM)                                     // Handle UDP data
	{
		read_data(s,recv_ptr,UDPHeader.u.stream,8);                             // W3100A UDP header copy
  		addr[0] = UDPHeader.u.header.addr[0];                                   // Read IP address of the peer
		addr[1] = UDPHeader.u.header.addr[1];
		addr[2] = UDPHeader.u.header.addr[2];
		addr[3] = UDPHeader.u.header.addr[3];                                   // Read Port number of the peer
		*port = UDPHeader.u.stream[6];
		*port = (*port << 8) + UDPHeader.u.stream[7];

#ifdef DEBUG
//		printf("UDP msg arrived\r\n");
//		printf("source Port : %d\r\n", *port);
//		printf("source IP : %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
		PutStringLn("UDP msg arrived");
		PutString("UDP Header Size : 0x");PutITOA(UDPHeader.u.header.size);PutStringLn("");
		PutString("Source Port : ");PutITOA(*port);PutStringLn("");
		PutString("Destination IP : ");PutHTOA(addr[0]);PutByte('.');PutHTOA(addr[1]);PutByte('.');PutHTOA(addr[2]);PutByte('.');PutHTOA(addr[3]);PutStringLn("");
#endif  
		size = UDPHeader.u.header.size - 8;
		rd_ptr.lVal += 8;		
		recv_ptr = (u_char *)(RBUFBASEADDRESS[s] + (UINT)(rd_ptr.lVal & RMASK[s]));
		ret = read_data(s, recv_ptr, buf,size);
		rd_ptr.lVal += ret;
#ifdef DEBUG
{
 		int i;	
//		printf("%.8lx : %.8lx : %.8lx\r\n", recv_ptr, buf, rd_ptr.lVal);
//		for (i = 0; i < ret; i++) printf("%c", buf[i]);
		PutLTOA((u_long)(recv_ptr)); PutString(" : ");PutLTOA((u_long)buf); PutString(" : ");PutLTOA(rd_ptr.lVal); PutStringLn("");
		for (i = 0; i < ret; i++) PutByte(buf[i]);
		PutStringLn("");
}
#endif
	}
#ifdef __IP_RAW__	
	else if ((OPT_PROTOCOL(s) & 0x07) == SOCK_IPL_RAW)                                  // When received IP layer RAW mode data
	{
//		for(i = 0; i < 4; i++) addr[i] = recv_ptr[2+i];                             // IP layer header copy
		read_data(s,recv_ptr,UDPHeader.u.stream,6);
		addr[0] = UDPHeader.u.header.addr[0];
		addr[1] = UDPHeader.u.header.addr[1];
		addr[2] = UDPHeader.u.header.addr[2];
		addr[3] = UDPHeader.u.header.addr[3];
#ifdef DEBUG
//		printf("ICMP msg received\r\n");
		PutStringLn("ICMP msg received");
		PutString("IP RAW Packet Data Size = ");PutITOA(UDPHeader.u.header.size);PutStringLn("");
		PutString("Destination IP : ");PutHTOA(addr[0]);PutByte('.');PutHTOA(addr[1]);PutByte('.');PutHTOA(addr[2]);PutByte('.');PutHTOA(addr[3]);PutStringLn("");		
#endif
		rd_ptr.lVal += 6;                                                           // Increment read pointer by 6, because already read as IP RAW header size
		recv_ptr = (u_char *)(RBUFBASEADDRESS[s] + (UINT)(rd_ptr.lVal & RMASK[s])); // Calculate IP layer raw mode data pointer
		ret = read_data(s, recv_ptr, buf, UDPHeader.u.header.size);                                     // data copy.
		rd_ptr.lVal += (ret-4);
	} 
#endif	// end __IP_RAW__

	*RX_RD_PTR(s)       = rd_ptr.cVal[0];		// Update rx_rd_ptr
	*(RX_RD_PTR(s) + 1) = rd_ptr.cVal[1];
	*(RX_RD_PTR(s) + 2) = rd_ptr.cVal[2];
	*(RX_RD_PTR(s) + 3) = rd_ptr.cVal[3];

#ifdef DEBUG
	EX0 = 0;
	k = *SHADOW_RXWR_PTR(s);
	wait_1us(2);
	wr_ptr.cVal[0] = *RX_WR_PTR(s);
	wr_ptr.cVal[1] = *(RX_WR_PTR(s) + 1);
	wr_ptr.cVal[2] = *(RX_WR_PTR(s) + 2);
	wr_ptr.cVal[3] = *(RX_WR_PTR(s) + 3);

	k = *SHADOW_RXRD_PTR(s);
	wait_1us(2);
	rd_ptr.cVal[0] = *RX_RD_PTR(s);
	rd_ptr.cVal[1] = *(RX_RD_PTR(s) + 1);
	rd_ptr.cVal[2] = *(RX_RD_PTR(s) + 2);
	rd_ptr.cVal[3] = *(RX_RD_PTR(s) + 3);
	EX0 = 1;

//	printf("%.8lx : %.8lx\r\n", wr_ptr.lVal, rd_ptr.lVal);
	PutLTOA(wr_ptr.lVal); PutString(" : ");PutLTOA(rd_ptr.lVal); PutStringLn("");
#endif


	COMMAND(s) = CRECV;                                                                 // RECV
    return	(ret);	// Real received size return
}
#endif	// __UDP__

/*
********************************************************************************
*               Channel closing function.
*
* Description : Function for closing the connection of the designated channel.
* Arguments   : s - channel number
* Returns     : None
* Note        : API Function	       
********************************************************************************
*/
void close(SOCKET s)
{
	u_int len;

	if (select(s, SEL_CONTROL) == SOCK_CLOSED) return;	   // Already closed

	// When closing, if there's data which have not processed, Insert some source codes to handle this
	// Or before application call close(), handle those data first and call close() later.


	len = select(s, SEL_SEND);
	if (len == SSIZE[s])
	{
		I_STATUS[s] =0;
		COMMAND(s) = CCLOSE;                               // CLOSE
		while(!(I_STATUS[s] & SCLOSED));
	}
}

/*
********************************************************************************
*               Function handling the channel socket information.
*
* Description : Return socket information of designated channel
* Arguments   : s    - channel number
*               func - SEL_CONTROL(0x00) -> return socket status 
*                      SEL_SEND(0x01)    -> return free transmit buffer size
*                      SEL_RECV(0x02)    -> return received data size
* Returns     : socket status or free transmit buffer size or received data size
* Note        : API Function
********************************************************************************
*/
u_int select(SOCKET s, u_char func)
{
	u_int val;
	un_l2cval rd_ptr, wr_ptr, ack_ptr;
	u_char k;

	switch (func) {
	case SEL_CONTROL :                                     // socket status information
		val = SOCK_STATUS(s);
		break;

	case SEL_SEND :                                        // Calculate send free buffer size
		EX0 = 0;
		k = *SHADOW_TXWR_PTR(s);
		wait_1us(2);
		wr_ptr.cVal[0] = *TX_WR_PTR(s);
		wr_ptr.cVal[1] = *(TX_WR_PTR(s) + 1);
		wr_ptr.cVal[2] = *(TX_WR_PTR(s) + 2);
		wr_ptr.cVal[3] = *(TX_WR_PTR(s) + 3);

		if( (OPT_PROTOCOL(s)& 0x07) != SOCK_STREAM)
		{
			k = *SHADOW_TXRD_PTR(s);
			wait_1us(2);
			ack_ptr.cVal[0] = *TX_RD_PTR(s);
			ack_ptr.cVal[1] = *(TX_RD_PTR(s) + 1);
			ack_ptr.cVal[2] = *(TX_RD_PTR(s) + 2);
			ack_ptr.cVal[3] = *(TX_RD_PTR(s) + 3);
		}
		else
		{
			k = *SHADOW_TXACK_PTR(s);
			wait_1us(2);
			ack_ptr.cVal[0] = *TX_ACK_PTR(s);
			ack_ptr.cVal[1] = *(TX_ACK_PTR(s) + 1);
			ack_ptr.cVal[2] = *(TX_ACK_PTR(s) + 2);
			ack_ptr.cVal[3] = *(TX_ACK_PTR(s) + 3);
		}
		EX0 = 1;

		if (wr_ptr.lVal >= ack_ptr.lVal) val = SSIZE[s] - (wr_ptr.lVal - ack_ptr.lVal);
		else val = SSIZE[s] - (0 - ack_ptr.lVal + wr_ptr.lVal);
		break;
	case SEL_RECV :                                        // Calculate received data size	
		EX0 = 0;
		k = *SHADOW_RXWR_PTR(s);
		wait_1us(2);
		wr_ptr.cVal[0] = *RX_WR_PTR(s);
		wr_ptr.cVal[1] = *(RX_WR_PTR(s) + 1);
		wr_ptr.cVal[2] = *(RX_WR_PTR(s) + 2);
		wr_ptr.cVal[3] = *(RX_WR_PTR(s) + 3);

		k = *SHADOW_RXRD_PTR(s);
		wait_1us(2);
		rd_ptr.cVal[0] = *RX_RD_PTR(s);
		rd_ptr.cVal[1] = *(RX_RD_PTR(s) + 1);
		rd_ptr.cVal[2] = *(RX_RD_PTR(s) + 2);
		rd_ptr.cVal[3] = *(RX_RD_PTR(s) + 3);
		EX0 = 1;

		if (wr_ptr.lVal == rd_ptr.lVal){ val = 0;}
		else if (wr_ptr.lVal > rd_ptr.lVal) val = wr_ptr.lVal - rd_ptr.lVal;
		else val = 0 - rd_ptr.lVal + wr_ptr.lVal;
		
#ifdef DEBUG
		PutString("wr_ptr.lVal = ");PutLTOA(wr_ptr.lVal);PutString(" : rd_ptr.lVal = ");PutLTOA(rd_ptr.lVal);PutString(" : size = ");PutITOA(val);PutStringLn("");
#endif		
		break;
	default :
		val = -1;
		break;
	}

    return	( val );
}

/*
********************************************************************************
*               Copies the receive buffer data of the W3100A to the system buffer.
*
* Description : Copies the receive buffer data of the W3100A to the system buffer.
*    It is called from the recv()or recvfrom() function.
* Arguments   : s   - channel number
*               src - receive buffer pointer of W3100A
*               dst - system buffer pointer
*               len - data size to copy
* Returns     : copied data size
* Note        : Internal Function
********************************************************************************
*/
u_int read_data(SOCKET s, u_char xdata* src, u_char xdata* dst, u_int len)
{
	u_int i, size, size1;

	if (len == 0) return 0;

	if( (((u_int)src & RMASK[s]) + len)  > RSIZE[s] ) 
	{
		size = RSIZE[s] - ((u_int)src & RMASK[s]);
		for (i = 0; i < size; i++) *dst++ = *src++;
		size1 = len - size;
		src =  RBUFBASEADDRESS[s];
		for (i = 0; i < size1; i++) *dst++ = *src++;
	}
	else
        	for (i = 0; i < len; i++) *dst++ = *src++;
	return len;
}

/*
********************************************************************************
*               Copies the system buffer data to the transmit buffer of the W3100A.
*
* Description : Copies the system buffer data to the transmit buffer of the W3100A.
*               It is called from the send_in()or sendto_in() function.
* Arguments   : s   - channel number
*               src - system buffer pointer
*               dst - send buffer pointer of W3100A
*               len - data size to copy
* Returns     : copied data size
* Note        : Internal Function
********************************************************************************
*/
u_int write_data(SOCKET s, u_char xdata* src, u_char xdata* dst, u_int len)
{
	u_int i, size, size1;

	if (len == 0) return 0;

	if ( (((u_int)dst & SMASK[s]) + len) > SSIZE[s] ) 
	{
		size = SSIZE[s] - ((UINT)dst & SMASK[s]);

		for (i = 0; i < size; i++) *dst++ = *src++;
		size1 = len - size;
		dst = (SBUFBASEADDRESS[s]);
		for (i = 0; i < size1; i++) *dst++ = *src++;
	} 
	else
		for (i = 0; i < len; i++) *dst++ = *src++;

	return len;
}

/*
********************************************************************************
*               Designate the delay
*
* Description : Wait for 10 milliseconds
* Arguments   : cnt - time to wait
* Returns     : None
* Note        : Internal Function
********************************************************************************
*/

void wait_10ms(int cnt)
{
	u_int i;

	for (i = 0; i < cnt; i++) wait_1ms(10);
}


/*
********************************************************************************
*               Designate the delay
*
* Description : Wait for 1 millisecond
* Arguments   : cnt - time to wait
* Returns     : None
* Note        : Internal Function
********************************************************************************
*/
void wait_1ms(int cnt)
{
	u_int i;

	for (i = 0; i < cnt; i++) wait_1us(1000);
}

/*
********************************************************************************
*               Designate the delay
*
* Description : Wait for 1 microsecond
* Arguments   : cnt - time to wait
* Returns     : None
* Note        : Internal Function, System Dependant            
********************************************************************************
*/
void wait_1us(int cnt)
{
	u_int i;

	for (i = 0; i < cnt; i++) ;
}
