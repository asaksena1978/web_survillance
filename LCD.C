/*
###############################################################################

File Name : LCD.C

Version : 1.0

Programmer(s) : Unknown

Created : Unknown 

Description : LCD Basic Function Implement File
                  
  
Modified History :

		Modified : 2002/03/13             
		Programmer : Woo Youl Kim
		Description : Comment 

###############################################################################
*/



/*
###############################################################################
Include Part
###############################################################################
*/

#include <lcd.h>



/*
###############################################################################
Define Part
###############################################################################
*/

#define BUSY		0x80



/*
###############################################################################
Grobal Variable definition Part
###############################################################################
*/



/*
###############################################################################
Local  Variable definition Part
###############################################################################
*/



/*
###############################################################################
Function Implementation Part
###############################################################################
*/

/*
Description     :  Check whether LCD is ready or not
Argument       :  
Return Value  :  if ready then 1, otherwise -1
Note              :  
*/
char LcdReady(void)
{
	unsigned char flag;
	while( (flag=LcdCommandR) & BUSY)  
	{
		if(flag == 0xFF) return -1;  
	}
	return 1;
}


/*
Description    :   Erase all of LCD contents
Argument      :   
Return Value :   
Note             : 
*/
void ClrScr(void)       
{
	LcdReady();
	LcdCommandW = 0x01;
}



/*
Description    :  Initialize LCD
Argument      : 
Return Value :  If initialization is OK then 1, otherwise -1
Note             
*/
char InitLcd(void)
{
	if(-1 ==LcdReady()) return -1;
	LcdCommandW = 0x38;

	LcdReady();
	LcdCommandW = 0x0C;

	ClrScr();
	GotoXY(0,0);
}


/*
Description    :  Move Cursor to (x Column, y Row)
Argument      :  x - Column value to move(INPUT), 
                      y - Row value to move(INPUT), 
Return Value :
Note             : The dimension of Included LCD is 2*16, so Row(Argument y) value could not exceed to 1
*/
void GotoXY(unsigned char x, unsigned char y)
{
    LcdReady();
    switch(y){
        case 0 : LcdCommandW = 0x80 + x; break;
        case 1 : LcdCommandW = 0xC0 + x; break;
        case 2 : LcdCommandW = 0x94 + x; break;
        case 3 : LcdCommandW = 0xD4 + x; break;
    }
}


/*
Description    :   Display character string from current cursor position
Argument      :   str - Character string to display to LCD(INPUT)
Return Value :   Pointer of string to be displayed
Note :
*/
char *Puts(char* str)
{
	unsigned char i;

	for (i=0; str[i] != '\0'; i++){
		LcdReady();
		LcdDataW = str[i];
	}
	return str;
}

/*
Description    :  Display a character to current cursor position
Argument      :   ch - A character to display to LCD(INPUT)
Return Value :   
Note             :
*/
void Putch(char ch)
{
	LcdReady();
	LcdDataW = ch;
}

/*
###############################################################################
Unused Function Implementation Part
###############################################################################
*/

#ifndef __LCD_UNUSED

/*
Description    :  Decide shape of cursor
Argument      :   type - Cursor shape(INPUT)
Return Value :   
Note             :
*/
void SetCursorType(unsigned char type)	
{
	LcdReady();
	switch(type) 
	{
			//No Cursor 
	        case 0 : LcdCommandW = 0x0C; break;  
			// Normal Cursor 
	        case 1 : LcdCommandW = 0x0E; break; 
		// No Cursor | Blink
		case 2 : LcdCommandW = 0x0D; break; 
			// Normal Cursor | Blink 
	        case 3 : LcdCommandW = 0x0F; break; 
}

/*
Description    :   Shift current cursor position to left side or right side
Argument      :   dir - deside the direction to shift (INPUT)  
                              if dir is not 0 then right Shift, otherwise left shift
Return Value :   
Note             :   
*/
void ShiftCursor(unsigned char dir)	
{
	LcdReady();
	LcdCommandW = (dir ? 0x14: 0x10);
}

/*
Description    :  Move cursor to first column
Argument      :
Return Value  :
Note :
*/
void HomeCursor(void)       
{
	LcdReady();
	LcdCommandW = 2;
}

#endif
