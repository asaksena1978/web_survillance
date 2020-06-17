/*
********************************************************************************
* Wiznet.
* 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
* Seoul, Korea
*
* (c) Copyright 2002, Wiznet, Seoul, Korea
*
* Filename : Webcam.c
* Version  : 1.0
* Programmer(s): 
* Created : 2002/01/
* Modified :
* Description : OV7620 & LC82210 Functionality Implements
********************************************************************************
*/
/*
###############################################################################
Include Part
###############################################################################
*/
#include <reg51.h>
#include <serial.h>
#include <webcam.h>
#include <string.h>
#include "allutil.h"
#include "socket.h"


/*
###############################################################################
Define Part
###############################################################################
*/
#define ctrlBaseValue 0x2801  					// CIF
//#define ctrlBaseValue 0x7801	       				// QCIF

#define SDAH	0x20						// SDA HIGH (use P1.5)
#define SCLH	0x40						// SCL HIGH (use P1.6)

#define SDA_LOW() 	(P1 &= ~(SDAH))
#define SDA_HIGH()	(P1 |= SDAH)
#define SCL_LOW()	(P1 &= ~(SCLH))
#define SCL_HIGH()	(P1 |= SCLH) 

/*
###############################################################################
Local  Variable Definition Part
###############################################################################
*/
unsigned char IMAGE_RDY;					// Completion of Jpeg Image Capture
unsigned int IMAGE0_SIZE = 0x0000;	       			// the size of #0 Image
unsigned int IMAGE1_SIZE = 0x0000;				// the size of #1 Image

unsigned char xdata *IMAGE0_BUF = 0x0E00;      			// memory address to store #0 Image (0x0E00 ~ 0x3DFF)
unsigned char xdata *IMAGE1_BUF= 0x3E00;			// memory address to store #1 Image (0x3E00 ~ 0x6DFF)
unsigned char xdata Header[650] = 0x0B10;			// Variable to store Jpeg Header

unsigned char INTVAL = 0x00;					//  Interrupt Value
unsigned char SETIMAGESTART = 0;				
unsigned int BUF_CNT = 0;					// Image Buffer Count
unsigned char STAT = 0;						// Image Capture Status
unsigned char SPIN = 1;									
unsigned int STAT_CNT = 0;					// Variable to check exit condition of Image Capure Status
unsigned int CTRL_BASEVAL = 0;
extern unsigned char HTTP_NC_Flag;


// Variable and constant corresponding JPEG(refer to JPEG document)
unsigned char xdata QTBLY_value[64] = {
    16, 12, 10, 14, 24, 24, 52, 72,
    12, 12, 14, 20, 22 ,58, 64, 56,
    14, 14, 16, 22, 40, 56, 70, 96,
    16, 18, 24, 30, 56, 88, 88, 62,
    18, 26, 38, 52, 68, 82,104,112,
    40, 36, 58, 64,110,104,122, 92,
    50, 60, 78, 80,104,114,120,104,
    62, 28, 56, 98, 78,100,102,100
};
    
unsigned char xdata QTBLC_value[64] = {
    18, 18, 24, 48,100,100,100,100,
    18, 22, 26, 66,100,100,100,100,
    24, 26, 56,100,100,100,100,100,
    48, 66,100,100,100,100,100,100,
   100,100,100,100,100,100,100,100,
   100,100,100,100,100,100,100,100,
   100,100,100,100,100,100,100,100,
   100,100,100,100,100,100,100,100
};

unsigned char xdata QTY[64];
unsigned char xdata QTC[64];

unsigned char xdata lqt[64];
unsigned char xdata cqt[64];

unsigned char xdata jpeg_luma_quantizer[64] = {
        16, 12, 12, 14, 12, 10, 14, 14,
        14, 14, 16, 18, 18, 16, 20, 24,
        24, 22, 22, 24, 26, 40, 50, 36,
        38, 30, 40, 58, 52, 72, 64, 56,
        56, 52, 58, 60, 62, 92, 78, 64,
        68, 88, 70, 56, 96, 88, 82,110,
        80, 56, 98,104,104,104, 62,112,
       112,114, 78,100,120, 92,104,100
};

unsigned char xdata jpeg_chroma_quantizer[64] = {
        18, 18, 18, 24, 22, 24, 48, 26,
        26, 48, 26, 26, 48,100, 66, 56,
        66,100,100,100,100,100,100,100,
       100,100,100,100,100,100,100,100,
       100,100,100,100,100,100,100,100,
       100,100,100,100,100,100,100,100,
       100,100,100,100,100,100,100,100,
       100,100,100,100,100,100,100,100
};

unsigned char xdata lum_dc_codelens[] = {
        0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};

unsigned char xdata lum_dc_symbols[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

unsigned char xdata lum_ac_codelens[] = {
        0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};

unsigned char xdata lum_ac_symbols[] = {
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
        0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
        0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
        0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
        0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
        0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa,
};

unsigned char xdata chm_dc_codelens[] = {
        0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

unsigned char xdata chm_dc_symbols[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

unsigned char xdata chm_ac_codelens[] = {
        0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};

unsigned char xdata chm_ac_symbols[] = {
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
        0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
        0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
        0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
        0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
        0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
        0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa,
};


/*
###############################################################################
Function Implementation Part
###############################################################################
*/

/*
********************************************************************************
*              ISR
*
* Description: LC82210ÀÇ Interrupt Service Routine
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/
void Int1(void) interrupt 2		// Interrupt 1
{
	int i;
	unsigned char xdata *ADDRESS;
	unsigned char xdata * pImageBuf = IMAGE0_BUF;

	EA = 0;
	
	//HTTP_NC_Flag = 2;						//Set Flag 1 to recognise the interrupt is for Network Camera
	//PutStringLn("I am in EX. Interrupt 1");

	ADDRESS = RS;
	*ADDRESS = (INTM & 0x3F);
	ADDRESS = DATA;
	*ADDRESS = 0xFF;

	ADDRESS = RS;
	*ADDRESS = (INT & 0x3F);
	ADDRESS = DATA;
	INTVAL = *ADDRESS;

	if (INTVAL & INT_FIFORDY) 	// FIFO READY ?
	{
		pImageBuf = IMAGE0_BUF + 0x3000*SPIN;
		for(i = 0; i < 32; i++) 
		{
			if (SETIMAGESTART) 
			{
				if (BUF_CNT > BUFSIZE - 4) BUF_CNT -= 2;
				ADDRESS = FIFO;
				*(pImageBuf + BUF_CNT++) = *ADDRESS;
				ADDRESS = FIFO;
				*(pImageBuf + BUF_CNT++) = *ADDRESS;
			}
			else
			{
				ADDRESS = FIFO;
				if ((*(pImageBuf + BUF_CNT) = *ADDRESS) == 0xFF) 
				{
					ADDRESS = FIFO;
					if ((*(pImageBuf + BUF_CNT + 1) = *ADDRESS) == 0xD8) 
					{
						BUF_CNT = 0;
						SETIMAGESTART = 1;
					}
				}
			}
		}
	}

	if(INTVAL & INT_VSYNC) 					// Wait 1 VSync
	{
		switch(STAT)
		{
		case 0:
			ADDRESS = RS;
			*ADDRESS = (CTRL & 0x3F);
			ADDRESS = DATA;
			*ADDRESS = (unsigned char) ((CTRL_BASEVAL | CTRL_VDEN) & 0x00FF);
                	ADDRESS = RS;
			*ADDRESS = ((CTRL + 1) & 0x3F);
			ADDRESS = DATA;
			*ADDRESS = (unsigned char) (((CTRL_BASEVAL | CTRL_VDEN) >> 8) & 0x00FF);
			STAT = 1;
			break;
		case 1:
			STAT = 2;
			break;
		case 2:
			ADDRESS = RS;
			*ADDRESS = (CTRL & 0x3F);
			ADDRESS = DATA;
			*ADDRESS = (unsigned char) ((CTRL_BASEVAL | CTRL_CDEN) & 0x00FF);
                	ADDRESS = RS;
			*ADDRESS = ((CTRL + 1) & 0x3F);
			ADDRESS = DATA;
			*ADDRESS = (unsigned char) (((CTRL_BASEVAL | CTRL_CDEN) >> 8) & 0x00FF);
			STAT = 3;
			break;
		case 3:
			ADDRESS = RS;
			*ADDRESS = (CTRL & 0x3F);
                	ADDRESS = DATA;
			*ADDRESS = (unsigned char) (CTRL_BASEVAL & 0x00FF);
                	ADDRESS = RS;
			*ADDRESS = ((CTRL + 1) & 0x3F);
			ADDRESS = DATA;
			*ADDRESS = (unsigned char) ((CTRL_BASEVAL >> 8) & 0x00FF);
			STAT = 4;
	 		break;
		case 6: 
			if(STAT_CNT > 2) STAT = 0;
		default:
			STAT_CNT++;
		}
	}
	if(STAT_CNT > 250)
	{
		ADDRESS = RS;
		*ADDRESS = COM;
		ADDRESS = DATA;
		*ADDRESS = 0x06;

		SETIMAGESTART = 0;
		BUF_CNT = 0;
		STAT_CNT = 0;
		STAT = 0;
		if(STAT == 4)	INTVAL = INT_EOP;
		else		INTVAL = INT_BUFEMP;

		SBUF = '#';
		while( !TI );
		TI = 0;
	}
	if ( INTVAL & INT_EOP )
	{
		STAT_CNT = 0;
		STAT = 5;
	}
	else if ( INTVAL & INT_BUFEMP )
	{
		if (SPIN) IMAGE1_SIZE = BUF_CNT;
		else IMAGE0_SIZE = BUF_CNT;
               	IMAGE_RDY = SPIN;
               	SPIN = 1 - SPIN;
	      	SETIMAGESTART = 0;
	       	BUF_CNT = 0;
	       	STAT = 6;
	       	STAT_CNT = 0;
	}
	else	STAT_CNT++; 

	ADDRESS = RS;
	*ADDRESS = (INTM & 0x3F);
	ADDRESS = DATA;
	*ADDRESS = 0x98;

	EA = 1;
}

/*
********************************************************************************
*              OV7620
*
* Description: OV7620 Setting function (use I2C I/F)
* Arguments  : None.
* Returns    :  if succeed then 0, otherwise 1;
* Note       : 
********************************************************************************
*/
char InitOV7620(unsigned char Addr, unsigned Data)
{
	char ret = 0;
	// Start
	SDA_HIGH();
	SCL_HIGH();
	SDA_LOW();	// SDA Low
	SCL_LOW();	// SCL Low
	if (!I2cWr(0x42)) {
		I2cWr(Addr);		// Write Address
		I2cWr(Data);		// Write Data
//		PutString("OV7620 Control OK\r\n");
	}
	else 
	{
		ret = 1;
		PutString("OV7620 Control Error!\r\n");
	}

	// End
	SCL_LOW();		// SCL Low
	SDA_LOW();
	SCL_HIGH();	// SCL High
	SDA_HIGH();	// SDA High

	return ret;
}

/*
********************************************************************************
*              OV7620
*
* Description: Write function of OV7620 using I2C I/F
* Arguments  : Data - value to write (INPUT)
* Returns    : ACK(0) or NACK(1)
* Note       : 
********************************************************************************
*/
unsigned char I2cWr(unsigned char Data)
{
	unsigned char i;

	for (i = 0x80; i; i = i >> 1) 
	{
		SCL_LOW();		// SCL Low
		if (Data & i) SDA_HIGH();
		else	SDA_LOW();
		SCL_HIGH();		// SCL High
	}
	SCL_LOW();		// SCL Low

	SCL_HIGH();		// SCL High
	Data = ((P1 & SDAH) >> 5) ;
	SCL_LOW();			// SCL Low
	
	return Data;
}
/*
********************************************************************************
*              LC82210
*
* Description: Initialization function of LC82210(Setting registers of LC82210)
* Arguments  : size - Set QCIF(3) or CIF(1) or VGA(2)(INPUT)
                      quality - Set JPEG Quality(INPUT)
* Returns    :  the size of Jpeg Header by selected image format
* Note       : 
********************************************************************************
*/
unsigned int InitLC82210(unsigned char size, unsigned char quality)
{
	unsigned char Temp;
	unsigned int header_size;

	unsigned char xdata *ADDRESS;

	
	if (size == 1)				// CIF
		CTRL_BASEVAL = 0x2801;
	else if (size == 3)			// QCIF
		CTRL_BASEVAL = 0x7801;
	else					// VGA
		CTRL_BASEVAL = 0x0001;

	ADDRESS = CPUMODE;			// Read CPUMOD : 8 bit bus width
	Temp = *ADDRESS;

	PutHTOA(Temp);
	PutString(" : INTEL MODE");

	SetRegister(CTRL,	0x0000);	// 1bank
	SetRegister(COM,	0x0001);	
	SetRegister(COMCF,	0x0000);	// 1bank
	SetRegister(LSIMOD,	0x0D18);        // Compressed, MSB First
	SetRegister(CDTIM,	0x0001);
	SetRegister(CBCF,	0x6464);
	SetRegister(IMGSIZ,	0x0009);
	SetRegister(CDADR1Y,	0x0320);
	SetRegister(CDADR1C,	0x15E0);
	SetRegister(CDADR2Y,	0x28A0); 	
	SetRegister(CDADR2C,	0x3B60);	
    
	if (size == 1)				// CIF
	        SetRegister(CDSIZ,0x0E13);
	else if (size == 3)			// QCIF
        	SetRegister(CDSIZ,0x0609);
	else					// VGA
        	SetRegister(CDSIZ,0x1D27);	 
        
	SetRegister(VDADR1Y,	0x0320);
	SetRegister(VDADR1C,	0x15E0);
	SetRegister(VDADR2Y,	0x28A0);	
	SetRegister(VDADR2C,	0x3B60);	
    
	SetRegister(VDSIZV,	0x0077);
	SetRegister(VDSIZH,	0x013F);
	SetRegister(VCF,	0x488F);
	SetRegister(VDOFFH,	0xC06B); 
	SetRegister(VDOFFV,	0x0012);

	SetRegister(INT,    	0x0000);
	SetRegister(INTM,   	0x0098);

	MakeQT(quality, QTY, QTC);

	MakeTables(quality, lqt, cqt); 
  
	// Calculate the size of Jpeg Header 
	if (size == 1) header_size = MakeHeaders(Header, 320, 240);		// CIF
	else if (size == 3) header_size = MakeHeaders(Header, 160, 120);	// QCIF
	else header_size = MakeHeaders(Header, 640, 480);			// VGA
    
	SetYQtable();			// Y Entry Quantization Set			
	SetCQtable();			// C Entry Qunatization Set

	PutString("\r\nLC82210 Initialized\r\n");

	return header_size;
}

/*
********************************************************************************
*              LC82210
*
* Description: Set registers of LC82210
* Arguments  : RegSel - Register Address(INPUT)
               Value - Setting value(INPUT)
* Returns    :  
* Note       : 
********************************************************************************
*/
void SetRegister(unsigned char RegSel, unsigned int Value)
{
	unsigned char xdata *ADDRESS;

	ADDRESS = RS;
	*ADDRESS = (RegSel & 0x3F);			//  Set Address

	ADDRESS = DATA;						
	*ADDRESS = (unsigned char) (Value & 0x00FF);	// write lower byte of value

	ADDRESS = RS;
	*ADDRESS = ((RegSel + 1) & 0x3F);	// Address + 1

	ADDRESS = DATA;
	*ADDRESS = (unsigned char) ((Value >> 8) & 0x00FF);	// write upper byte of value
}

/*
********************************************************************************
*              LC82210
*
* Description:  Set L(Y Entry), C(UV Entry) Quantizier
* Arguments  : q - Quality Factor (INPUT)
               lum_q - Luminance Table Pointer(INOUT)
	       chr_q - Color DifferenceTable Pointer(INOUT)
* Returns    :  
* Note       : 
********************************************************************************
*/
void MakeQT(unsigned int q, unsigned char xdata *lum_q, unsigned char xdata *chr_q)
{
	unsigned int i;
	unsigned int factor = q;

	if (q < 1) factor = 1;
	if (q > 99) factor = 99;
	if (q < 50) q = 5000 / factor;
	else q = 200 - factor * 2;

	for (i = 0; i < 64; i++) 
	{
		unsigned int lq = (QTBLY_value[i] * q + 50) / 100;
		unsigned int cq = (QTBLC_value[i] * q + 50) / 100;

		// Limit the quantizers to 1 <= q <= 255
		if (lq < 1) lq = 1;
		else if (lq > 255) lq = 255;
		lum_q[i] = lq;
	
		if (cq < 1) cq = 1;
		else if (cq > 255) cq = 255;
		chr_q[i] = cq;
	}
}

/*
********************************************************************************
*              LC82210
*
* Description:  MakeTables with the Q factor and two int[64] return arrays
* Arguments  : q - Quality Factor (INPUT)
                      lum_q - Luminance Table Pointer(INOUT)
					  chr_q - Color DifferenceTable Pointer(INOUT)
* Returns    :  
* Note       : 
********************************************************************************
*/
void MakeTables(unsigned int q, unsigned char xdata *lum_q, unsigned char xdata *chr_q)
{
	unsigned int i;
    
	unsigned int factor = q;
  
	if (q < 1) factor = 1;
	if (q > 99) factor = 99;
	if (q < 50)
		q = 5000 / factor;
	else
		q = 200 - factor * 2;

	for (i = 0; i < 64; i++) {
		unsigned int lq = (jpeg_luma_quantizer[i] * q + 50) / 100;
		unsigned int cq = (jpeg_chroma_quantizer[i] * q + 50) / 100;

		// Limit the quantizers to 1 <= q <= 255
		if (lq < 1) lq = 1;
		else if (lq > 255) lq = 255;
		lum_q[i] = lq;

		if (cq < 1) cq = 1;
		else if (cq > 255) cq = 255;
		chr_q[i] = cq;
	}
}

/*
********************************************************************************
*              LC82210
*
* Description:  Make Quantization Header
* Arguments  : p - Pointer to store Quantization Header(INOUT)
               qt - Quantization Table Pointer (INPUT)
	       tableNo - Table Number ( INPUT)
* Returns    : Quantization Header Pointer
* Note       : 
********************************************************************************
*/
unsigned char* MakeQuantHeader(unsigned char xdata *p, unsigned char *qt, unsigned int tableNo)
{

        *p++ = 0xff;
        *p++ = 0xdb;            // DQT
        *p++ = 0;               // length msb
        *p++ = 67;              // length lsb
        *p++ = tableNo;
        memcpy(p, qt, 64);
        return (p + 64);
}

/*
********************************************************************************
*              LC82210
*
* Description:  Make Huffman Code Header
* Arguments  : 
* Returns    :   Huffman Code Header Pointer
* Note       : 
********************************************************************************
*/
unsigned char *MakeHuffmanHeader(unsigned char xdata *p, unsigned char *codelens,
				unsigned int ncodes, unsigned char *symbols,
				unsigned int nsymbols, unsigned int tableNo, unsigned int tableClass)
{
        *p++ = 0xff;
        *p++ = 0xc4;            // DHT
        *p++ = 0;               // length msb
        *p++ = 3 + ncodes + nsymbols; // length lsb
        *p++ = tableClass << 4 | tableNo;
        memcpy(p, codelens, ncodes);
        p += ncodes;
        memcpy(p, symbols, nsymbols);
        p += nsymbols;
        return (p);
}

/*
********************************************************************************
*              LC82210
*
* Description:  Given an RTP/JPEG type code, q factor, width, and height,
					 generate a frame and scan headers that can be prepended
					 to the RTP/JPEG data payload to produce a JPEG compressed
					 image in interchange format (except for possible trailing
					 garbage and absence of an EOI marker to terminate the scan).
* Arguments  :	p - Pointer to store Jpeg Header(INOUT)
						w - Width(INPUT)
						h - Height(INPUT)
* Returns    :  JPEG Header Size
* Note       : 
********************************************************************************
*/
unsigned int MakeHeaders(unsigned char xdata *p, unsigned int w, unsigned int h)
{
        unsigned char *start = p;

        *p++ = 0xff;
        *p++ = 0xd8;            // SOI

        p = MakeQuantHeader(p, lqt, 0);

        p = MakeQuantHeader(p, cqt, 1);

        p = MakeHuffmanHeader(p, lum_dc_codelens,
                              sizeof(lum_dc_codelens),
                              lum_dc_symbols,
                              sizeof(lum_dc_symbols), 0, 0);
        p = MakeHuffmanHeader(p, lum_ac_codelens,
                              sizeof(lum_ac_codelens),
                              lum_ac_symbols,
                              sizeof(lum_ac_symbols), 0, 1);
        p = MakeHuffmanHeader(p, chm_dc_codelens,
                              sizeof(chm_dc_codelens),
                              chm_dc_symbols,
                              sizeof(chm_dc_symbols), 1, 0);
        p = MakeHuffmanHeader(p, chm_ac_codelens,
                              sizeof(chm_ac_codelens),
                              chm_ac_symbols,
                              sizeof(chm_ac_symbols), 1, 1);

        *p++ = 0xff;
        *p++ = 0xc0;            // SOF
        *p++ = 0;               // length msb
        *p++ = 17;              // length lsb
        *p++ = 8;               // 8-bit precision
        *p++ = h >> 8;          // height msb
        *p++ = h;               // height lsb
        *p++ = w >> 8;          // width msb
        *p++ = w;               // wudth lsb
	
        *p++ = 3;               // number of components
	
        *p++ = 0;               // comp 0

        *p++ = 0x22;		  // hsamp = 2, vsamp = 2

        *p++ = 0;               // quant table 0

        *p++ = 1;               // comp 1
        *p++ = 0x11;            // hsamp = 1, vsamp = 1
        *p++ = 1;               // quant table 1

        *p++ = 2;               // comp 2
        *p++ = 0x11;            // hsamp = 1, vsamp = 1
        *p++ = 1;               // quant table 1

        *p++ = 0xff;
        *p++ = 0xda;            // SOS
        *p++ = 0;               // length msb
        *p++ = 12;              // length lsb
        *p++ = 3;               // 3 components
        *p++ = 0;               // comp 0
        *p++ = 0;               // huffman table 0
        *p++ = 1;               // comp 1
        *p++ = 0x11;            // huffman table 1
        *p++ = 2;               // comp 2
        *p++ = 0x11;            // huffman table 1
        *p++ = 0;               // first DCT coeff
        *p++ = 63;              // last DCT coeff
        *p++ = 0;               // sucessive approx.

        return (p - start);
}

/*
********************************************************************************
*              LC82210
*
* Description:  Set Y Entry Quantization Table Set
* Arguments  :
* Returns    :  
* Note       : 
********************************************************************************
*/
void SetYQtable(void)
{
	unsigned int i;

	for (i = 0;i < 64; i++) 
		SetRegister(QTBLY, QTY[i]);
}

/*
********************************************************************************
*              LC82210
*
* Description:  Set Color Difference(UV) Entry Quantization Table Set
* Arguments  :
* Returns    :  
* Note       : 
********************************************************************************
*/
void SetCQtable(void)
{
	unsigned int i;

	for (i = 0;i < 64; i++) 
		SetRegister(QTBLC, QTC[i]);
}

