#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv){
/*
RUNS:
$ ./Task_B /path/logs.txt 1
$ ./Task_B /path/logs.txt 2 /path/example.txt
$ ./Task_B /path/logs.txt 3

path = argv[1]
number(1-3) = argv[2]
*/
	struct tm t;
	const char* path = argv[1];
	int functionality = atoi(argv[2]);


	char ** compute_log_buffer(char* buffer,size_t size){
		char** array_all =malloc(sizeof(size));
		int BUF = 1024;
    	FILE  * fp;
    	char * line = NULL;
    	size_t len = 0;
    	fp = fopen("/tmp/access_control_logfile.txt", "r") ;
    	ssize_t read;

    	char n[BUF] ;
    	int offset,index = 0,index_all=0 ;
    	while ((read = getline(&line, &len, fp)) != -1) {
        	char* array[7] ;
        	index = 0 ;
        	while ( 1 == sscanf(line, "%s%n[^\n]", n, &offset) ) {
        	    // printf("%s\n", n) ;
        	    array[index] = strdup(n) ;
        	    line += offset ;
        	    index++ ;
        	}
	        int i = 0;
	        while(i < 7) {
				//printf("index_all=%d\n",index_all);
				array_all[index_all] =calloc(1,sizeof(size));
				array_all[index_all] = array[i] ;
				printf("array_all[%d]=%s\n",index_all,array_all[index_all]);
				//printf("%s\n", array[i]) ;

				i++ ;
				index_all++;
	        }
	    }
		return array_all;
	}

	long int get_epoch(int year,int month,int day, int hour, int min, int sec){

		time_t t_of_day;
		t.tm_year = year;
		t.tm_mon = month;
		t.tm_mday = day;
		t.tm_hour =hour;
		t.tm_min = min;
		t.tm_sec = sec;
		t.tm_isdst = 0;			//DST on? 1 = yes, 0 = no, -1 = unknown
		t_of_day = mktime(&t);
		return (long) t_of_day;
	}

	/*split date string with '-' */
	int split_string_date(const char* date,int number){
    	const char * start = date;
    	char * end;
    	while ( ( end = strchr( start, '-' ) ) != NULL )
    	{
			if (number == 1)
				return atoi(start);
    	    start = end + 1;
			if (number == 2)
				return atoi(start);
    	}
    	if(number==3)
			return atoi(start);
	}

	/*split time string with ':' */
	int split_string_time(const char* date,int number){
    	const char * start = date;
    	char * end;
    	while ( ( end = strchr( start, ':' ) ) != NULL )
    	{
			if (number == 1)
				return atoi(start);
    	    start = end + 1;
			if (number == 2)
				return atoi(start);
    	}
    	if(number==3)
			return atoi(start);
	}



	const char *line_array[7]; //pinakas me string ka8e mia apo tis 7 stiles tou log
	char *line = NULL;
	size_t len=0;
	ssize_t read;
	int i = 0;
/*FUNCTIONALITY 1 == Detect users with no permisision*/
	if (functionality == 1){
		FILE *fd = fopen(path,"r");
		if (fd ==NULL)
			exit(EXIT_FAILURE);


		while((read = getline(&line, &len,fd)) != -1){
			char *tb = "\t";
			char *token;
			token = strtok(line,tb);
			i =0;
			while(i<7){
				line_array[i] = token;
				//printf("%s\n",token);
				token = strtok(NULL,tb);
				//printf("%s\n",line_array[i]);
				i++;
			}
			//printf("line_array[5]=%s\n",line_array[5]);
			if(strcmp(line_array[5], "1")==0){
				//printf("Unauthorized access to file:%s from User: %s\n",line_array[1],line_array[0]);
				printf("%s\n",line_array[0]);
			}


		}

		fclose(fd);
	}
/*FUNCTIONALITY 2 == Find all users that accesses given file
	and who changed its content*/
	if(functionality == 2){
		const char *given_path = argv[3];

		//printf("given path = %s\n",given_path);
		FILE *fd = fopen(path,"r");
		if (fd ==NULL)
			exit(EXIT_FAILURE);
		char line_hashes[20];
		const char *prev_hash =NULL;
		while((read = getline(&line, &len,fd)) != -1){
			char *tb = "\t";
			char *token;
			token = strtok(line,tb);
			i =0;
			while(i<7){
				line_array[i] = token;
				token = strtok(NULL,tb);
				i++;
			}
			if((strcmp(line_array[1], given_path)==0)){
				if((strcmp(line_array[4],"0")==0) && (strcmp(line_array[5],"0")==0))
							printf("%s A\n",line_array[0]);
				if(prev_hash!=NULL){
					if((strcmp(prev_hash,line_array[6])!=0)){
						printf("%s M\n",line_array[0]);
					}
					else if((strcmp(line_array[4],"1")==0) && (strcmp(line_array[5],"0")==0)){
					printf("%s A\n",line_array[0]);
					}
				}
				prev_hash = line_array[6];
				char *line2 = NULL;
				size_t len2=0;
				ssize_t read2;
				const char *line_array2[7];
				//printf("line_array= %s\n",line_array[1]);

				while((read2 = getline(&line2, &len2,fd)) != -1){
					char *tb2 = "\t";
					char *token2;
					token2 = strtok(line2,tb2);
					int k =0;
					while(k<7){
						line_array2[k] = token2;
						token2 = strtok(NULL,tb2);
						k++;
					}
					if((strcmp(line_array2[1], given_path)==0) && (strcmp(prev_hash,line_array2[6])!=0)){
						//printf("User: %s MODIFIED the given file: %s\n",line_array[0],given_path);
						if((strcmp(line_array2[4],"1")==0) && (strcmp(line_array2[5],"0")==0))
							printf("%s M\n",line_array2[0]);
						prev_hash =line_array2[6];
						break;
					}
					else if((strcmp(line_array2[1], given_path)==0) && strcmp(prev_hash,line_array2[6])==0 && (strcmp(line_array2[5],"0")==0)){
						printf("%s A\n",line_array2[0]);
					}
				}
			}
			int i;
		}
		fclose(fd);
	}

/*FUNCTIONALITY 3 == Find all users that have unsuccessfully tried
to access more than 10 different files in less than 24 hours.*/
	if(functionality == 3){
		int denied_counter = 0;
		FILE *fd = fopen(path,"r");
		if (fd ==NULL)
			exit(EXIT_FAILURE);
			/*anoigw to logfile mou kai pernw to content tou se ena buffer gia na doulepsw*/
			FILE *open_text = fopen("/tmp/access_control_logfile.txt","r");
			long lSize; char *buffer;
			fseek(open_text,0L,SEEK_END);
			lSize = ftell(open_text);
			rewind(open_text);

			/*allocate memory for the content in buffer*/
			buffer = calloc(1,lSize+1);
			if (!buffer) fclose(open_text),fputs("memory allocation failed\n",stderr),exit(1);

			/*copy file content into buffer*/
			fread(buffer, lSize,1,open_text);

			//char ** log_buffer = compute_log_buffer(buffer,sizeof(buffer));
			//printf("log_buffer=\n%s\n",log_buffer[7]);

		while((read = getline(&line, &len,fd)) != -1){

			denied_counter=0;
			char *tb = "\t";
			char *token;
			token = strtok(line,tb);
			i =0;
			while(i<7){
				line_array[i] = token;
				token = strtok(NULL,tb);
				i++;
			}
			/*an dw mia unsuccessfull access*/
			if((strcmp(line_array[5],"1")==0)){
				denied_counter++;
				//const char *buffer_imerominia = line_array[2];
				int year = split_string_date(line_array[2],1);
				int month = split_string_date(line_array[2],2);
				int day = split_string_date(line_array[2],3);
				int hour = split_string_time(line_array[3],1);
				int min = split_string_time(line_array[3],2);
				int sec = split_string_time(line_array[3],3);
				int first_epoch = get_epoch(year,month,day,hour,min,sec);
				char *line2 = NULL;
				size_t len2=0;
				ssize_t read2;
				const char *line_array2[7];
				/*twra koitaw gia ton idio UID an eixe alla 10 unsuccessfull accesses to epomeno 24wro*/
				while((read2 = getline(&line2, &len2,fd)) != -1){
					char *tb2 = "\t";
					char *token2;
					token2 = strtok(line2,tb2);
					int k =0;
					while(k<7){
						line_array2[k] = token2;
						token2 = strtok(NULL,tb2);
						k++;
					}
					if((strcmp(line_array[0],line_array2[0])==0) && (strcmp(line_array2[5],"1")==0)
					   && strcmp(line_array[1],line_array2[1])!=0){
						/*KNOW I MUST CHECK IF TIME EXCEEDS 24 HOURS FROM THE FIRST FAILED ATTEMPT*/
						//printf("line_array2d[ate]=%s\n",line_array2[2]);
						int year = split_string_date(line_array2[2],1);
						int month = split_string_date(line_array2[2],2);
						int day = split_string_date(line_array2[2],3);
						int hour = split_string_time(line_array2[3],1);
						int min = split_string_time(line_array2[3],2);
						int sec = split_string_time(line_array2[3],3);
						int current_epoch = get_epoch(year,month,day,hour,min,sec);
						if(current_epoch <= first_epoch+86400){
							denied_counter++;
						}
					}
					if(denied_counter == 3){
						printf("User %s tried unsuccesfully to open 3 different files on 24 hours\n",line_array[0]);
						//printf("%s\n",line_array[0]);
						break;
					}
				}

			}
		}
		fclose(fd);

	}


}
