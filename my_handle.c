#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdarg.h>

void print_log(FILE *logfile,int uid, const char* path,struct tm* timeinfo, int open, int action_denied, unsigned char* hash ){
	fprintf(logfile,"%d\t%s\t%d-%d-%d\t%d:%d:%d\t%d\t%d\t",uid,path,1900+timeinfo->tm_year,timeinfo->tm_mon,timeinfo->tm_mday,timeinfo->tm_hour,
		   timeinfo->tm_min,timeinfo->tm_sec,open,action_denied);
	size_t i;
	for (i = 0; i < 20; i++) {
		fprintf(logfile,"%02x", hash[i]);
	}
	fprintf(logfile,"\n");
}

FILE *fopen(const char *path, const char *mode){
	printf("In our own fopen, opening %s\n",path);

	FILE *(*original_fopen)(const char*, const char*);
	original_fopen = dlsym(RTLD_NEXT,"fopen");


	FILE *logfile = original_fopen("/tmp/access_control_logfile.txt","a");
	if (logfile ==NULL){perror("error opening file.");}
	/*Create entries of logfile here*/
	/*UID*/
	int UID = getuid();
	/*TIME-DATE*/
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	/*ACTION DENIED */
	int action_denied = 0;
	FILE *buff_open = original_fopen(path,mode);
	if (!buff_open){action_denied = 1;}
	/*GET FULL PATH*/
	char proclnk[0xFFF];
	char filename[0xFFF];
	int MAXSIZE =0xFFF;
	int file_descriptor = fileno(buff_open);
	sprintf(proclnk, "/proc/self/fd/%d", file_descriptor);
	ssize_t r = readlink(proclnk,filename,MAXSIZE);
	 if (r < 0)
     {
		printf("failed to readlink\n");
		exit(1);
    }
    filename[r] = '\0';
	/*HASH VALUE*/
	long lSize; char *buffer;

	fseek(buff_open, 0L,SEEK_END);
	lSize = ftell(buff_open);
	rewind(buff_open);

	//allocate mem for content in buffer
	buffer = calloc(1,lSize+1);
	if (!buffer) fclose(buff_open),fputs("memory allocation failed\n",stderr),exit(1);
	//copy file content into buffer
	fread(buffer, lSize,1,buff_open);

	unsigned char hash[SHA_DIGEST_LENGTH];	//==20 bytes, the dimention of SHA1 output

	SHA1((const unsigned char*)buffer, sizeof(buffer) - 1, hash);

	print_log(logfile,UID,filename, timeinfo,0,action_denied,hash);

	fclose(logfile);
	fclose(buff_open);
	free(buffer);

	return (*original_fopen)(path,mode);
}

int open(const char *path, int flags, ... ){
	printf("In our own open, opening %s\n",path);

	mode_t mode = 0;
	va_list ap;
	int i;
	va_start(ap,flags);	//argument list exei 3 max stoixeia
	for(i = 0; i < 3; i++){
		if ((i ==  2)&&(va_arg(ap,mode_t)!=0)){
			mode = va_arg(ap,mode_t);
		}
	}
	va_end(ap);


	int (*original_open)(const char*, int, ...);
	original_open = dlsym(RTLD_NEXT,"open");
	/*find previous fopen to open logfile*/
	FILE *(*original_fopen)(const char*, const char*);
	original_fopen = dlsym(RTLD_NEXT,"fopen");

	//FILE *logfile = original_fopen("logfile.txt","a");
	FILE *logfile = original_fopen("/tmp/access_control_logfile.txt","a");
	if (logfile ==NULL){perror("error opening file.");}

	/*Create entries of logfile here*/
	/*UID*/
	int UID = getuid();
	/*TIME-DATE*/
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	/*ACTION DENIED */
	int action_denied = 0;
	int buff_open;
	buff_open = original_open(path,flags,mode);
	if (buff_open<0){action_denied = 1; printf("action denied!\n");}
	/*GET FULL PATH*/
	char proclnk[0xFFF];
	char filename[0xFFF];
	int MAXSIZE =0xFFF;
	sprintf(proclnk, "/proc/self/fd/%d", buff_open);
	ssize_t r = readlink(proclnk,filename,MAXSIZE);
	 if (r < 0)
     {
		printf("failed to readlink\n");
		exit(1);
    }
    filename[r] = '\0';
	/*HASH VALUE*/
	FILE *open_text = original_fopen(path,"r");
	long lSize; char *buffer;
	fseek(open_text,0L,SEEK_END);
	lSize = ftell(open_text);
	rewind(open_text);

	/*allocate memory for the content in buffer*/
	buffer = calloc(1,lSize+1);
	if (!buffer) fclose(open_text),fputs("memory allocation failed\n",stderr),exit(1);

	/*copy file content into buffer*/
	fread(buffer, lSize,1,open_text);

	unsigned char hash[SHA_DIGEST_LENGTH];	//==20 the dimention of SHA1 output

	SHA1((const unsigned char*)buffer, sizeof(buffer) - 1, hash);
	//printf("hash = %s\n",hash);
	//printf("buffer = %s\n",buffer);

	print_log(logfile,UID,filename, timeinfo,0,action_denied,hash);

	close(buff_open);
	fclose(logfile);
	fclose(open_text);
	free(buffer);

	return(*original_open)(path,flags,mode);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream ){
	printf("In our own fwrite, writing..\n");

	size_t(*original_fwrite)(const void*, size_t,size_t,FILE*);
	original_fwrite = dlsym(RTLD_NEXT,"fwrite");
	size_t ret = (*original_fwrite)(ptr,size,nmemb,stream);

	/*VRISKW tin previous fopen gia na anoiksw to logfile*/
	FILE *(*original_fopen)(const char*, const char*);
	original_fopen = dlsym(RTLD_NEXT,"fopen");


	//FILE *logfile = original_fopen("logfile.txt","a");
	FILE *logfile = original_fopen("/tmp/access_control_logfile.txt","a");
	if (logfile == NULL){perror("error opening file.");}
	/*Create entries of logfile here*/
	/*UID*/
	int UID = getuid();
	/*TIME-DATE*/
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	fflush(stream);
	/*HASH VALUE*/
	//original_fwrite(ptr,size,nmemb,stream);

	char proclnk[0xFFF];
	char filename[0xFFF];
	int MAXSIZE =0xFFF;
	int file_descriptor = fileno(stream);
	sprintf(proclnk, "/proc/self/fd/%d", file_descriptor);
	ssize_t r = readlink(proclnk,filename,MAXSIZE);
	 if (r < 0)
     {
		printf("failed to readlink\n");
		exit(1);
    }
    filename[r] = '\0';

	FILE *fwrite_text = original_fopen(filename,"r+");
	if (!fwrite_text){printf("den anoikse to file gia na dw to content gia hash\n");}

	long lSize; char *buffer;
	fseek(fwrite_text,0L,SEEK_END);
	lSize = ftell(fwrite_text);
	rewind(fwrite_text);

	/*allocate memory for the content in buffer*/
	buffer = calloc(1,lSize+1);
	if (!buffer) fclose(fwrite_text),fputs("memory allocation failed\n",stderr),exit(1);

	/*copy file content into buffer*/
	fread(buffer, lSize,1,fwrite_text);
	unsigned char hash[SHA_DIGEST_LENGTH];	//==20 the dimention of SHA1 output

	SHA1((const unsigned char*)buffer, sizeof(buffer) - 1, hash);
	//printf("hash = %s\n",hash);

	/*ACTION DENIED */
	ssize_t(*original_write)(int, const void*, size_t);
	original_write = dlsym(RTLD_NEXT,"write");
	int action_denied = 0;
	char test_write_valid[] = "";
	if(original_write(file_descriptor,test_write_valid,0) != 0){action_denied = 1;printf("action denied!\n");}

	print_log(logfile,UID,filename, timeinfo,1,action_denied,hash);


	free(buffer);
	fclose(logfile);
	fclose(fwrite_text);
	//return (*original_fwrite)(ptr,size,nmemb,stream);
	return ret;
}

ssize_t write(int fildes, const void *buf,size_t nbytes){
	printf("In our own write, writing..\n");

	ssize_t(*original_write)(int, const void*, size_t);
	original_write = dlsym(RTLD_NEXT,"write");
	ssize_t ret = (*original_write)(fildes, buf,nbytes);

	FILE *(*original_fopen)(const char*, const char*);
	original_fopen = dlsym(RTLD_NEXT,"fopen");

	//FILE *logfile = original_fopen("logfile.txt","a");
	FILE *logfile = original_fopen("/tmp/access_control_logfile.txt","a");
	if (logfile == NULL){perror("error opening file.");}
	/*Create entries of logfile here*/
	/*UID*/
	int UID = getuid();
	/*TIME-DATE*/
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	/*ACTION DENIED */
	int action_denied = 0;
	char test_write_valid[] = "";
	if(original_write(fildes,test_write_valid,0) != 0){action_denied = 1;printf("action denied!\n");}

	fsync(fildes);
	/*HASH VALUE*/
	char proclnk[0xFFF];
	char filename[0xFFF];
	int MAXSIZE =0xFFF;
	sprintf(proclnk, "/proc/self/fd/%d", fildes);
	ssize_t r = readlink(proclnk,filename,MAXSIZE);
	 if (r < 0)
     {
		printf("failed to readlink\n");
		exit(1);
    }
    filename[r] = '\0';

	FILE *fwrite_text = original_fopen(filename,"r+");
	if (!fwrite_text){printf("den anoikse to file gia na dw to content gia hash\n");}

	long lSize; char *buffer;
	fseek(fwrite_text,0L,SEEK_END);
	lSize = ftell(fwrite_text);
	rewind(fwrite_text);

	/*allocate memory for the content in buffer*/
	buffer = calloc(1,lSize+1);
	if (!buffer) fclose(fwrite_text),fputs("memory allocation failed\n",stderr),exit(1);

	/*copy file content into buffer*/
	fread(buffer, lSize,1,fwrite_text);
	unsigned char hash[SHA_DIGEST_LENGTH];	//==20 the dimention of SHA1 output

	SHA1((const unsigned char*)buffer, sizeof(buffer) - 1, hash);
	//printf("hash = %s\n",hash);

	print_log(logfile,UID,filename, timeinfo,1,action_denied,hash);

	free(buffer);
	fclose(logfile);
	fclose(fwrite_text);

	//return (*original_write)(fildes,buf,nbytes);
	return ret;
}
