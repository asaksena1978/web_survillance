#include<stdio.h>
#include<conio.h>
#include<stdlib.h>

void main()
{
	FILE *fs, *ft;
	char *f_source,ch,str[]="No. of Bytes: ";
	int ct = 0,i;
	printf("\nEnter the html file source:");
	scanf("%s",f_source);
	fs = fopen(f_source,"r");
	if(fs == NULL)
	{
		printf("\nERROR: Wrong source file name or cann't open the file");
		exit(1);
	}
	ft = fopen("result.txt","w");
	if(ft == NULL)
	{
		printf("\nERROR: Target file cann't be created");
		exit(1);
	}
	while((ch = fgetc(fs)) != EOF)
	{
		while(ch == '\n' || ch =='\t' || ch == '\r')
			ch = fgetc(fs);
		if(ch == '\"' || ch == '%')
			fputc('\\',ft);
		fputc(ch,ft);
		ct++;
		if(ch == '>')
		{
				while((ch = fgetc(fs)) == ' ' || ch == '\n' || ch == '\t');
				fputc(ch,ft);
				ct++;
		}
	}
	fclose(fs);
	printf("\nNumber of bytes: %d",ct);
	fputc('\n',ft),
	i = 0;
	while(str[i] != '\0')
		fputc(str[i++],ft);
	i = 0;
	while(ct != 0)
	{
		str[i] = (char)(ct % 10) + 30;
		ct = ct / 10;
		i++;
	}
	str[i] = '\0';
	i = strlen(str) - 1;
	while(i >= 0)
		fputc(str[i--],ft);
	fclose(ft);
	printf("\nThe file has been successfully created");
	getch();
}