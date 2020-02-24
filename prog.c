#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int main(void){

/*FOPEN() CALLING*/
	printf("Calling the fopen() function..\n");

	FILE *fd = fopen("test.txt","r");
	if(!fd){
		printf("fopen() returned NULL\n");
		//return 1;
	}
	else
		printf("fopen() succeeded\n");
	fclose(fd);
/*FWRITE() CALLING*/
	FILE *fd1 = fopen("test.txt","r");
	char str[]="FWrite something in test.txt\n";
	printf("Calling the fwrite() function..\n");

	if(1*strlen(str) != fwrite(str,1,strlen(str),fd1))
    {
       printf("fwrite() failed\n");
	}
	else
		printf("fwrite() succeeded\n");

	fclose(fd1);
/*OPEN() CALLING */
	printf("Calling the open() function..\n");
	int fd_open = open("test2.txt", O_RDONLY);
	if (fd_open <0){
		printf("open() returned NULL\n");
		//return 1;
	}
	else
		printf("open() succeeded\n");
/*WRITE() CALLING-MODIFY FILE*/
	printf("Calling the write() function..\n");
	char str_write[] = "Write something in text.txt\n";
	if(write(fd_open,str_write,strlen(str_write)) != strlen(str_write))
		printf("write() failed\n");
	else
		printf("write() succeeded\n");



/*FWRITE() CALLING*/
	FILE *fd3 = fopen("test3.txt","r");
	char str2[]="Write something in test.txt\n";
	printf("Calling the fwrite() function..\n");

	if(1*strlen(str2) != fwrite(str2,1,strlen(str2),fd3))
    {
       printf("fwrite() failed\n");
	}
	else
		printf("fwrite() succeeded\n");
	fclose(fd3);


	return 0;

}
