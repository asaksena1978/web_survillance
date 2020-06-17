/****************************************************************
	Include part
****************************************************************/
#include<stdio.h>
#include<conio.h>

/************************************************************************
*	Function Name: Main
*	Arguments:     None
*	Returns:       None
*	Desccription:  it will count the number of bytes within the message
*	Note:	       copy the message in a file and pass the file name to
		       this program
*************************************************************************/

void main()
{
	FILE *fp;
	char file_name[15], ch;
	static unsigned int no_of_char;
	printf("Enter the file name:");
	scanf("%s",file_name);
	fp = fopen(file_name,"r");
	if(fp == NULL)
	{
		printf("\nERROR: Wrong path or file name or error in opening the file");
		getch();
		exit(0);
	}
	while((ch = fgetc(fp)) != EOF)
	{
		if(ch != '\\')
			no_of_char++;
	}
	fclose(fp);
	printf("\nThe length of the text is %u bytes", no_of_char);
	getch();
}