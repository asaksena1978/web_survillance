/*
###############################################################################

File Name :  LCD.H

Version : 1.0

Description :  LCD Basic Function Definition
  
Modified History

		Modified : 2002/03/13
		Description :  Eliminate unused functions

###############################################################################
*/



/*
###############################################################################
Include Part
###############################################################################
*/





/*
###############################################################################
Define Part
###############################################################################
*/

// LCD Command Write Register
#define LcdCommandW (*((unsigned char xdata *)0x09000))

// LCD Command Read Register
#define LcdCommandR (*((unsigned char xdata *)0x09001))

// LCD Data Write Register
#define LcdDataW    (*((unsigned char xdata *)0x09002))

// LCD Data Read Register
#define LcdDataR    (*((unsigned char xdata *)0x09003))


/* If there are unused functions in program and on compiling with Keil-C, it result in warning.In this reason, wrong operation could be happend.
So it prevent to compile unused functions to use define-function.
*/
#define __LCD_UNUSED   // If defined with " __LCD_UNUSED", actually it's not to be compiled "__LCD_UNUSED Block"


/*
###############################################################################
Grobal Variable Definition Part
###############################################################################
*/




/*
###############################################################################
Function Prototype Definition  Part
###############################################################################
*/

/*
Description     :  Check for LCD to be ready
Argument       :  
Return Value  :  If it's ready ,then '1',else then '-1'.
Note              : If LCD Device is not insert actually,then return '-1'.
*/
extern char LcdReady(void);

/*
Description    :   Clear LCD. 
Argument      :   
Return Value :   
Note             : 
*/
extern void ClrScr(void);

/*
Description    : Initialize LCD.
Argument      : 
Return Value :  If initialization is O.K, then return '1', else then return '-1'. 
Note             
*/
extern char InitLcd(void);


/*
Description    :  Move Cursor to (x Column, y Row)
Argument      :  x - Column value to move(INPUT), 
                      y - Row value to move(INPUT), 
Return Value :
Note             : The dimension of Included LCD is 2*16, so Row(Argument y) value could not exceed to 1
*/
extern void GotoXY(unsigned char x, unsigned char y);

/*
Description    :   Display character string from current cursor position
Argument      :   str - Character string to display to LCD(INPUT)
Return Value :   Pointer of string to be displayed
Note :
*/
extern char *Puts(char* str);

/*
Description    :  Display a character to current cursor position
Argument      :   ch - A character to display to LCD(INPUT)
Return Value :   
Note             :
*/
extern void Putch(char ch);


/*
###############################################################################
Unused Function Prototype Definition Part
###############################################################################
*/

#ifndef __LCD_UNUSED

/*
Description    :  Move Cursor first Column.
Argument      :
Return Value  :
Note :
*/
extern void HomeCursor(void);

/*
Description    :  Decide Cursor type.
Argument      :   type - Cursor type(INPUT)
Return Value :   
Note             :
*/
extern void SetCursorType(unsigned char type);

/*
Description    :   Shift to Left and Right current Cursor.
Argument      :   dir - Decide direction to be Shift.(INPUT)  dir !=0  -> Right Shift, dir == 0 -> Left Shift
Return Value :   
Note             :   
*/
extern void ShiftCursor(unsigned char dir);


#endif

