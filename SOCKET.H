/*
********************************************************************************
* Wiznet.
* 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
* Seoul, Korea
*
* (c) Copyright 2002, Wiznet, Seoul, Korea
*
* Filename : socket.h
* Version  : 2.2
* Programmer(s): 
* Created : 2002/01/
* Modified      : 2002/06/20 - added delay in initseqnum() function
		               to complete write operation of TX Pointer Registers.
		  2002/09/27 - Global Interrupt Status Variable & MACRO Renaming
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
			     - Move GetDestAddr() function to 'sockutil.h'
		  2002/10/28 - Version Up
			     - At select(), integrated each received size caculation of TCP, UDP or IP_RAW into one calculation.
			     - At recvfrom(), Received data is processed by packet unit.

* Description : Header file of W3100A for 8051
********************************************************************************
*/

#include "type.h"

#ifndef	__SOCKET_H__
#define	__SOCKET_H__

/*
*******************************************************************
To prevent occurrence of warning when there's unused function by KEIL compiler,
include functions to use as using mode
********************************************************************
*/
#define	__TCP__		// Function for TCP
#define __TCP_SERVER__	// Function for TCP server
//#define __TCP_CLIENT__	// Function for TCP client
//#define	__UDP__		// Function for UDP
//#define	__OPT__		// Option function
//#define __IP_RAW__	// To use upper layer protocol with IP Protocol 

#ifdef __IP_RAW__
	#ifndef __UDP__
		#define __UDP__
	#endif
#endif	

/*******************************************************************/

#define	MAX_SOCK_NUM	4			// Concurrent maxmium number of socket

#define	I2CHIP_BASE		0x8000		// Address of W3100A
#define	SEND_DATA_BUF	0xC000			// Internal Tx buffer address of W3100A
#define	RECV_DATA_BUF	0xE000			// Internal Rx buffer address of W3100A

#define MAX_SEGMENT_SIZE	1460		// Maximum TCP transmission packet size
#define MAX_BUF_SIZE1		0


/* Internal register set of W3100A */
#define	COMMAND(i)		(*((volatile u_char xdata *)(I2CHIP_BASE + i)))
#define	INT_STATUS(i)		(*((volatile u_char xdata *)(I2CHIP_BASE + 0x04 + i)))
#define	INT_REG			(*((volatile u_char xdata *)(I2CHIP_BASE + 0x08)))
#define	INTMASK			(*((volatile u_char xdata *)(I2CHIP_BASE + 0x09)))
#define	RESETSOCK		(*((volatile u_char xdata *)(I2CHIP_BASE + 0x0A)))

#define RX_PTR_BASE		0x10
#define RX_PTR_SIZE		0x0C

#define	RX_WR_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + RX_PTR_BASE + RX_PTR_SIZE * i))
#define	RX_RD_PTR(i)   		((volatile u_char xdata *)(I2CHIP_BASE + RX_PTR_BASE + RX_PTR_SIZE * i + 0x04))
#define	RX_ACK_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + TX_PTR_BASE + TX_PTR_SIZE * i + 0x08))

#define TX_PTR_BASE		0x40
#define TX_PTR_SIZE		0x0C

#define	TX_WR_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + TX_PTR_BASE + TX_PTR_SIZE * i))
#define	TX_RD_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + TX_PTR_BASE + TX_PTR_SIZE * i + 0x04))
#define	TX_ACK_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + RX_PTR_BASE + RX_PTR_SIZE * i + 0x08))

/* Shadow Register Pointer Define */
#define SHADOW_RXWR_PTR(i)	((volatile u_char xdata *)(I2CHIP_BASE + 0x1E0 + 3*i))
#define	SHADOW_RXRD_PTR(i)	((volatile u_char xdata *)(I2CHIP_BASE + 0x1E1 + 3*i))
#define	SHADOW_TXACK_PTR(i)	((volatile u_char xdata *)(I2CHIP_BASE + 0x1E2 + 3*i))
#define	SHADOW_TXWR_PTR(i)	((volatile u_char xdata *)(I2CHIP_BASE + 0x1F0 + 3*i))
#define SHADOW_TXRD_PTR(i)	((volatile u_char xdata *)(I2CHIP_BASE + 0x1F1 + 3*i))

#define SOCK_BASE		0xA0
#define SOCK_SIZE		0x18

#define SOCK_STATUS(i)		(*((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i)))
#define OPT_PROTOCOL(i)		(*((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x01)))
#define DST_HA_PTR(i)  		((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x02))
#define DST_IP_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x08))
#define DST_PORT_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x0C))
#define SRC_PORT_PTR(i)		((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x0E))
#define IP_PROTOCOL(i)		(*((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x10)))
#define TOS(i)			(*((volatile u_char xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x11)))
#define MSS(i)			(*((volatile u_int xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x12)))
#define P_WINDOW(i)		(*((volatile u_int xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x14)))
#define WINDOW(i)		(*((volatile u_int xdata *)(I2CHIP_BASE + SOCK_BASE + SOCK_SIZE * i + 0x16)))


#define GATEWAY_PTR		((volatile u_char xdata *)(I2CHIP_BASE + 0x80))
#define SUBNET_MASK_PTR		((volatile u_char xdata *)(I2CHIP_BASE + 0x84))
#define SRC_HA_PTR		((volatile u_char xdata *)(I2CHIP_BASE + 0x88))
#define SRC_IP_PTR		((volatile u_char xdata *)(I2CHIP_BASE + 0x8E))
#define TIMEOUT_PTR		((volatile u_char xdata *)(I2CHIP_BASE + 0x92))

#define RX_DMEM_SIZE		(*((volatile u_char xdata *)(I2CHIP_BASE + 0x95)))
#define TX_DMEM_SIZE		(*((volatile u_char xdata *)(I2CHIP_BASE + 0x96)))

/* SOCKET OPTION(Settting OPT_PROTOCOL REG.) */
#define SOCKOPT_BROADCAST	0x80		// Transmission, Reception of broadcasting data
#define SOCKOPT_NDTIMEOUT	0x40		// Setting timeout
#define SOCKOPT_NDACK		0x20		// Setting No Delayed Ack(TCP)
#define SOCKOPT_SWS		0x10		// Setting Silly Window Syndrome(TCP)

/* OPTION(Setting OPT_PROTOCOL REG.) for MAC LAYER RAW MODE */
#define MACLOPT_RXERR		0x80		// Setting reception of error packet
#define MACLOPT_BROADCAST	0x40		// Setting reception of broadcast packet
#define MACLOPT_PROMISC		0x20		// Setting reception of promiscuous packet

/* Distinguish TCP / UDP / IP RAW / MAC RAW (Setting OPT_PROTOCOL REG.) */
#define	SOCK_CLOSEDM		0x00 		// unused socket
#define	SOCK_STREAM		0x01		// TCP
#define	SOCK_DGRAM		0x02		// UDP
#define	SOCK_IPL_RAW		0x03		// IP LAYER RAW SOCK
#define	SOCK_MACL_RAW		0x04		// MAC LAYER RAW SOCK

/* Setting IP PROTOCOL */
#define IPPROTO_IP              0               // dummy for IP 
#define IPPROTO_ICMP            1               // control message protocol 
#define IPPROTO_IGMP            2               // internet group management protocol 
#define IPPROTO_GGP             3               // gateway^2 (deprecated) 
#define IPPROTO_TCP             6               // tcp 
#define IPPROTO_PUP             12              // pup
#define IPPROTO_UDP             17              // user datagram protocol 
#define IPPROTO_IDP             22              // xns idp 
#define IPPROTO_ND              77              // UNOFFICIAL net disk proto
#define IPPROTO_RAW             255             // raw IP packet 

/* Select parameter to use */
#define SEL_CONTROL		0	// Confirm socket status
#define SEL_SEND		1	       	// Confirm Tx free buffer size
#define SEL_RECV		2	       	// Confirm Rx data size

/* Command variables */
#define CSYS_INIT		0x01	   	// To set up network information(mac address, gateway address, subnet mask, source ip)
#define CSOCK_INIT		0x02		// To initialize socket
#define CCONNECT		0x04		// To establish connection as tcp client mode
#define CLISTEN			0x08		// To wait for connection request as tcp server mode
#define CCLOSE			0x10		// To terminate connection
#define CSEND			0x20		// To send data
#define CRECV			0x40		// To receive data
#define CSW_RESET		0x80		// To do software reset

/* Status Variables */
#define SSYS_INIT_OK		0x01		// Completion of CSYS_INIT command
#define SSOCK_INIT_OK		0x02		// Completion of CSOCK_INIT command
#define SESTABLISHED		0x04		// Completion of connection setup
#define SCLOSED			0x08		// Completion of CCLOSED command
#define STIMEOUT		0x10		// Timout occured at send,sendto command
#define SSEND_OK		0x20		// Completion of sending data
#define SRECV_OK		0x40		// Completion of receiving data

/* Socket Status Vabiables */
#define SOCK_CLOSED		0x00		// Status of connection closed
#define SOCK_ARP		0x01		// Status of ARP
#define SOCK_LISTEN		0x02		// Status of waiting for TCP connection setup
#define SOCK_SYNSENT		0x03		// Status of setting up TCP connection
#define SOCK_SYNSENT_ACK	0x04		// Status of setting up TCP connection
#define SOCK_SYNRECV		0x05		// Status of setting up TCP connection
#define SOCK_ESTABLISHED	0x06		// Status of TCP connection established
#define SOCK_CLOSE_WAIT		0x07		// Status of closing TCP connection
#define SOCK_LAST_ACK		0x08		// Status of closing TCP connection
#define SOCK_FIN_WAIT1		0x09		// Status of closing TCP connection
#define SOCK_FIN_WAIT2		0x0A		// Status of closing TCP connection
#define SOCK_CLOSING		0x0B		// Status of closing TCP connection
#define SOCK_TIME_WAIT		0x0C		// Status of closing TCP connection
#define SOCK_RESET		0x0D		// Status of closing TCP connection
#define SOCK_INIT		0x0E		// Status of socket initialization
#define SOCK_UDP		0x0F		// Status of UDP
#define SOCK_RAW		0x10		// Status of IP RAW


//void ISR_ESTABLISHED(SOCKET s);
//void ISR_CLOSED(SOCKET s);
//void ISR_RX(SOCKET s);

void initW3100A(void);							// Initialize W3100A
void sysinit(u_char sbufsize, u_char rbufsize);                         // Specify flexible memory and soft reset W3100A
void setsubmask(u_char * addr);						// Set subnet mask value
void setgateway(u_char * addr);                                         // Set Gateway IP address
void setMACAddr(u_char * addr);                                         // Set Mac address
void setIP(u_char * addr);                                              // Set source IP address

char socket(SOCKET s, u_char protocol, u_int port, u_char flag);        // Open a channel among TCP, UDP, IP_RAW mode


#ifdef __IP_RAW__
void setIPprotocol(SOCKET s, u_char ipprotocol);			// Set upper protocol of IP 
#endif

#ifdef	__OPT__
void settimeout(u_char * val);						// Set re-transmition timeout value
void setINTMask(u_char mask);						// Set Interrupt mask value
void setTOS(SOCKET s, u_char tos);                                      // Set type of service 
void reset_sock(SOCKET s);                                              // Reset the specified channel
#endif


#ifdef	__TCP_CLIENT__
char connect(SOCKET s, u_char * addr, u_int port);
#endif

#ifdef	__TCP_SERVER__
//char listen(SOCKET s, u_char * addr, u_int * port);
void NBlisten(SOCKET s);
#endif

void initseqnum(SOCKET s);

#ifdef	__TCP__
int send(SOCKET s, const u_char xdata* buf, u_int len);
int send_in(SOCKET s, const u_char xdata* buf, u_int len);
int recv(SOCKET s, const u_char xdata * buf, u_int len);
#endif	// __TCP__

#ifdef	__UDP__
u_int sendto(SOCKET , const u_char  xdata*, u_int, u_char *, u_int);
u_int sendto_in(SOCKET , const u_char  xdata*, u_int);
u_int recvfrom(SOCKET , const u_char  xdata*, u_int, u_char *, u_int *);
#endif	// __UDP__

u_int read_data(SOCKET s, u_char  xdata* src, u_char xdata* dst, u_int len);
u_int write_data(SOCKET s, u_char xdata* src, u_char xdata* dst, u_int len);

void close(SOCKET s);
u_int select(SOCKET s, u_char func);

void wait_10ms(int cnt);
void wait_1ms(int cnt);
void wait_1us(int cnt);

#endif	/* __SOCKET_H__ */
