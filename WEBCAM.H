/*
********************************************************************************
* Wiznet.
* 5F Simmtech Bldg., 228-3, Nonhyun-dong, Kangnam-gu,
* Seoul, Korea
*
* (c) Copyright 2002, Wiznet, Seoul, Korea
*
* Filename : webcam.h
* Version  : 1.0
* Programmer(s): 
* Created : 2002/01/
* Modified :
* Description : Definition and Function about OV7620 & LC82210
********************************************************************************
*/


/*
###############################################################################
Define Part
###############################################################################
*/

//#define YUV_FORMAT      0       /*      0 : YUV411,  1 : YUV422         */

/* Registers on LC82210 */
#define CPUMODE		0x09400
#define FIFO		0x09401
#define RS		0x09402
#define DATA		0x09403

/* LC82210 Indirect Registers */
#define CTRL            0x0             /* Control Registor */
#define COMCF           0x2             /* Command Set Registor */
#define COM             0x4             /* Command Registor */
#define DRAMARL         0x6             /* DRAM Address M,L Registor */
#define DRAMARH         0x8             /* DRAM Address H Registor */
#define INT             0xA             /* DRAM Address ÁöÁ¤ Registor */
#define INTM            0xC             /* Interrupt Mask Registor */
#define BITNUM          0xE             /* Total coded data read Registor */
#define BUFMON          0x10            /* Code buffer Monitor Registor */
#define QTBLY           0x12            /* Quantization Table Entry(Y) Registor */
#define QTBLC           0x14            /* Quantization Table Entry(C) Registor */
#define SF              0x16            /* Scaling Factor Registor */
#define LSIMOD          0x18            /* LSI Mode Set Registor */
#define CDTIM           0x1A            /* Codec Timing Set Registor */
#define IMGSIZ          0x1C            /* Max Image Size Registor */
#define CDADR1Y         0x1E            /* Compression/Decompression image start(1Y) Registor */
#define CDADR1C         0x20            /* Compression/Decompression image start(1C) Registor */
#define CDADR2Y         0x22            /* Compression/Decompression image start(2Y) Registor */
#define CDADR2C         0x24            /* Compression/Decompression image start(2C) Registor */
#define CDSIZ           0x26            /* Compression/Decompression image Size Registor */
#define VDADR1Y         0x28            /* Capture/Display Image start Address(1Y) Registor */
#define VDADR1C         0x2A            /* Capture/Display Image start Address(1C) Registor */
#define VDADR2Y         0x2C            /* Capture/Display Image start Address(2Y) Registor */
#define VDADR2C         0x2E            /* Capture/Display Image start Address(2Y) Registor */
#define VDSIZH          0x30            /* Capture/Display Image Size(H) Registor */
#define VDSIZV          0x32            /* Capture/Display Image Size(V) Registor */
#define VCF             0x34            /* Video I/F Set Registor */
#define VDOFFH          0x36            /* Display Start Position (H) Registor */
#define VDOFFV          0x38            /* Display Start Position (V) Registor */
#define CBCF            0x3A            /* Code Buffer Set Registor */

/* ?????? */   
#define AREAX		0x3C
#define AREAY		0x3E
#define BLKBS1		0x40
#define BLKBS2		0x42
#define BLKUPA		0x44
#define BLKUPB		0x46
#define BLKUPC		0x48
#define AREA		0x4A

/* Control Register */
#define CTRL_VDEN	0x0004
#define CTRL_CDEN	0x0002

/* Interrupt */   
#define INT_FIFORDY     0x01
#define INT_VSYNC       0x02
#define INT_BUFEMP      0x04
#define INT_BUFLVL	0x08
#define INT_BUFFUL      0x10
#define INT_SOI         0x20
#define INT_EOP         0x40
#define INT_ERR         0x80
    
/* JPEG */
#define SOI             0xFFD8    
#define EOI             0xFFD9
#define QUALITY 	50
//#define QUALITY 	75

/*-----------------------------------------------------------------------------------*/
//#define BUFSIZE		6*1024
//#define BUFSIZE		6656
//#define BUFSIZE		6912
#define BUFSIZE		12*1024
//#define BUFSIZE		13*1024


/*
###############################################################################
Global Variable Definition Part
###############################################################################
*/
extern unsigned char xdata Header[650];
extern unsigned char IMAGE_RDY;
extern unsigned int IMAGE0_SIZE;
extern unsigned int IMAGE1_SIZE;
extern unsigned char xdata *IMAGE0_BUF;
extern unsigned char xdata *IMAGE1_BUF;

/*
###############################################################################
Function Prototype Definition Part
###############################################################################
*/
/*-----------------------------------------------------------------------------------*/
/* LCD82210 Initialization */
extern char InitOV7620(unsigned char Addr, unsigned Data);
extern unsigned char I2cWr(unsigned char Data);
extern unsigned int InitLC82210(unsigned char size, unsigned char quality);
extern void SetRegister(unsigned char RegSel, unsigned int Value);
extern void MakeQT(unsigned int q, unsigned char xdata *lum_q, unsigned char xdata *chr_q);
extern void MakeTables(unsigned int q, unsigned char xdata *lum_q, unsigned char xdata *chr_q);
extern unsigned char *MakeQuantHeader(unsigned char xdata *p, unsigned char *qt, unsigned int tableNo);
extern unsigned char *MakeHuffmanHeader(unsigned char xdata *p, unsigned char *codelens, unsigned int ncodes,	unsigned char *symbols,unsigned int nsymbols, unsigned int tableNo, unsigned int tableClass);
extern unsigned int MakeHeaders(unsigned char xdata *p, unsigned int w, unsigned int h);
extern void SetYQtable(void);
extern void SetCQtable(void);


