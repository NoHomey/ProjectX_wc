#include <sys/types.h>		//		
#include <sys/stat.h>		// 	 	
#include <fcntl.h>		//		
#include <stdio.h>		//		
#include <unistd.h>		//		
#include <errno.h>		//

int words = 0;			//	
int bytes = 0;			//	
int lines = 0;			//		
int total_words = 0;		//			
int total_bytes = 0;		//			
int total_lines = 0;		
				//								
int error_flag = 0;		//			

int get_extention(int counter);								//
void write_counter(int counter);							//
void get_counters_from_file(char *file_pathname);					//
void get_counters_from_stdin(void);							//
void write_reference(char *reference, int stream);					//
void write_space(int stream);									//
void write_newline(int stream);								//
void write_dots(int stream);									//
void write_wc(int stream);									//	
void write_total(int stream);									//
void write_total_counters(int total_lines, int total_words, int total_bytes);		//
void wordcounter_file(char *file_pathname);						
void wordcounter_stdin(char *reference);						//
void write_error(int error_number);							
void write_error_massage(int error, char *reference);					

int main(int argc,char **argv)
{
	int i;
	int total_flag = 1;
	char stdin_name[1] = {'\0'};
	
	
	if(argc < 3)
		total_flag = 0;

	if(argc == 1)
		wordcounter_stdin(stdin_name);

	for(i = 1;i < argc;i++)
	{
		if(argv[i][0] == '-')
			wordcounter_stdin(argv[i]);

		else
			wordcounter_file(argv[i]);
	}

	if(total_flag == 1)
		write_total_counters(total_lines, total_words, total_bytes);

	return 0;
}

int get_extention(int counter)
{
	int i = 1;
	while(1)
	{

		if((counter / i)  < 1)
		{
			return (i / 10);
		}
		i = i*10;
		
	}
}

void write_counter(int counter)
{

	int digit_extention = get_extention(counter);
	char digit_to_char[1];
	char reference[] = {'\0'};
	if(digit_extention <= 0)
	{
		digit_to_char[0] = 48;
		int status_returned = write(STDOUT_FILENO, digit_to_char, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(errno, reference);	
					return;
				}
	}
	else if(digit_extention == 1)
	{
		while(counter > 0)
		{
			digit_to_char[0] = (counter % 10) + 48;
			counter = counter/10;
			if(digit_to_char[0] >= 48 && digit_to_char[0] <= 57)
			{
				int status_returned = write(STDOUT_FILENO, digit_to_char, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(errno, reference);	
					return;
				}
			}
		}
	}
	else
	{
		while(digit_extention > 0)
		{
			digit_to_char[0] = (counter/digit_extention) + 48;
			counter = counter%digit_extention;
			digit_extention = digit_extention/10;
			if(digit_to_char[0] >= 48 && digit_to_char[0] <= 57)
			{
				int status_returned = write(STDOUT_FILENO, digit_to_char, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(errno, reference);
					return;
				}
			}
		}
	}

	return;
}

void get_counters_from_file(char * file_pathname)
{	

	int file_discriptor = open(file_pathname, O_RDONLY);
	if((file_discriptor == -1) && (errno > 0))	
	{
		write_error_massage(errno,file_pathname);
		return;
	}	
		
	char buffer[1];
	int whitespace_flag = 0;
	int endoffile_flag = 0;
	words = 0;
	bytes = 0;
	lines = 0;

	while(!endoffile_flag)
	{
		int status_returned = read(file_discriptor, buffer, 1);
		if((status_returned != 1) && (errno > 0))
		{
			write_error_massage(errno,file_pathname);
			return;
		}
		else
		{
			if(status_returned == 0)
			{
				endoffile_flag = 1;
				break;
			}
			bytes++;
			if((buffer[0] == ' ') || (buffer[0] == '\t') || (buffer[0] == '\n') || (buffer[0] == '\r'))	
				whitespace_flag = 0;
			else if(whitespace_flag == 0)
			{
				words++;
				whitespace_flag = 1;
			}
			if(buffer[0] == '\n')
				lines++;
		}
	}

	total_words += words;
	total_bytes += bytes;
	total_lines += lines;

	int close_status = close(file_discriptor);
	if((close_status == -1) && (errno > 0))
	{
		write_error_massage(errno, file_pathname);
		return;
	}
		
	return;
}

void get_counters_from_stdin(void)
{
		
	char buffer[1];
	char reference[] = {'\0'};
	int whitespace_flag = 0;
	int endoffile_flag = 0;
	words = 0;
	bytes = 0;
	lines = 0;
	while(!endoffile_flag)
	{

		int status_returned = read(STDIN_FILENO, buffer, 1);
		if((status_returned != 1) && (errno > 0))
		{
			write_error_massage(errno, reference);
			return;
		}
		else
			if(status_returned == 0)
			{
				endoffile_flag = 1;
				break;
			}
			bytes++;
			if((buffer[0] == ' ') || (buffer[0] == '\t') || (buffer[0] == '\n') || (buffer[0] == '\r'))	
				whitespace_flag = 0;
			else if(whitespace_flag == 0)
			{
				words++;
				whitespace_flag = 1;
			}
			if(buffer[0] == '\n')
				lines++;
	}

	total_words += words;
	total_bytes += bytes;
	total_lines += lines;	
	
	return;
}

void write_reference(char *reference, int stream)
{
	int i =0;
	int status_returned = 1;
	while(reference[i] != '\0')
	{
		if(stream == 1)
			status_returned = write(STDERR_FILENO, reference + i, 1);
		
		else
			status_returned = write(STDOUT_FILENO, reference + i, 1);
		i++;
		if((status_returned != 1) && (errno > 0))
		{
			write_error_massage(errno, reference);
			return;
		}
		
	}

	return;
}

void write_space(int stream)
{
	char symbol[2];
	symbol[0] = ' ';
	symbol[1] = '\0';
	write_reference(symbol, stream);

	return;
}

void write_newline(int stream)
{
	char symbol[2];
	symbol[0] = '\n';
	symbol[1] = '\0';
	write_reference(symbol, stream);

	return;
}


void write_dots(int stream)
{
	char symbol[2];
	symbol[0] = ':';
	symbol[1] = '\0';
	write_reference(symbol, stream);

	return;
}

void write_wc(int stream)
{
	char symbols[3];
	symbols[0] = 'w';
	symbols[1] = 'c';
	symbols[2] = '\0';
	write_reference(symbols, stream);

	return;
}

void write_total(int stream)
{
	char symbols[] = {'t', 'o', 't', 'a', 'l', '\0'};
	write_reference(symbols, stream);
	
	return;
}

void write_total_counters(int total_lines, int  total_words, int total_bytes)
{
	int stream = 0;
	write_counter(total_lines);
	write_space(stream);
	write_counter(total_words);
	write_space(stream);
	write_counter(total_bytes);
	write_space(stream);
	write_total(stream);
	write_newline(stream);

	return;
}	

void wordcounter_file(char *file_pathname)
{
	int stream = 0;	
	get_counters_from_file(file_pathname);
	if(error_flag == 1)
		return;
	write_counter(lines);
	write_space(stream);	
	write_counter(words);
	write_space(stream);	
	write_counter(bytes);
	write_space(stream);	
	write_reference(file_pathname, stream);
	write_newline(stream);

	return;	
}

void wordcounter_stdin(char *reference)
{
	int stream = 0;	
	get_counters_from_stdin();
	write_counter(lines);
	write_space(stream);
	write_counter(words);
	write_space(stream);
	write_counter(bytes);
	write_space(stream);
	write_reference(reference, stream);
	write_newline(stream);

	return;
}

void write_error_massage(int error, char *reference)
{
	int stream = 1;	
	write_wc(stream);
	write_dots(stream);
	write_space(stream);		
	write_reference(reference, stream);		
	write_dots(stream);
	write_space(stream);	
	write_error(error);
	write_newline(stream);
	error_flag = 1;

	return;
}

void write_error(int error_number)
{
	char reference[] = {'\0'};
	int i;
	int stream = 1;
	switch(error_number)
	{

		case 1:
		{
			char error_massage[] = {'O', 'p', 'e', 'r', 'a', 't', 'i', 'o', 'n', ' ', 'n', 'o', 't', ' ', 'p', 'e', 'r', 'm', 'i', 't', 't', 'e', 'd', '\0'};
			write_reference(error_massage, stream);		
			break;
		}
		case 2 :
		{
			char error_massage[] = {'N', 'o', ' ', 's', 'u', 'c', 'h', ' ', 'f', 'i', 'l', 'e', ' ', 'o', 'r', ' ', 'd', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'y', '\0'};
			write_reference(error_massage, stream);		
			break;
		}
		case 4 :
		{
			char error_massage[] = {'I', 'n', 't', 'e', 'r', 'r', 'u', 'p', 't', 'e', 'd', ' ', 's', 'y', 's', 't', 'e', 'm', ' ', 'c', 'a', 'l', 'l'};
			for(i = 0;i < 23;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
			
				}
			
			
			}			
			break;
		}
		case 5 :
		{
			char error_massage[] = {'I', '/', 'O', ' ', 'e', 'r', 'r', 'o', 'r'};
			for(i = 0;i < 9;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 6 :
		{
			char error_massage[] = {'N', 'o', ' ', 's', 'u', 'c', 'h', ' ', 'd', 'e', 'v', 'a', 'i', 'c', 'e', ' ', 'o', 'r', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's'};
			for(i = 0;i < 27;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 9 :
		{
			char error_massage[] = {'B', 'a', 'd', ' ', 'f', 'i', 'l', 'e', ' ', 'n', 'u', 'm', 'b', 'e', 'r'};
			for(i = 0;i < 15;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 11 :
		{
			char error_massage[] = {'T', 'r', 'y', ' ', 'a', 'g', 'a', 'i', 'n'};
			for(i = 0;i < 9;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 12 :
		{
			char error_massage[] = {'O', 'u', 't', ' ', 'o', 'f', ' ', 'm', 'e', 'm', 'o', 'r', 'y'};
			for(i = 0;i < 13;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 13 :
		{
			char error_massage[] = {'P', 'e', 'r', 'm', 'i', 's', 's', 'i', 'o', 'n', ' ', 'd', 'e', 'n', 'i', 'e', 'd'};
			for(i = 0;i < 17;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 14 :
		{
			char error_massage[] = {'B', 'a', 'd', ' ', 'a', 'd', 'd', 'r', 'e', 's'};
			for(i = 0;i < 10;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 17 :
		{
			char error_massage[] = {'F', 'i', 'l', 'e', ' ', 'e', 'x', 'i', 's', 't'};
			for(i = 0;i < 10;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 19 :
		{
			char error_massage[] = {'N', 'o', ' ', 's', 'u', 'c', 'h', ' ', 'd', 'e', 'v', 'a', 'i', 'c', 'e'};
			for(i = 0;i < 15;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 20 :
		{
			char error_massage[] = {'N', 'o', 't', ' ', 'a', ' ', 'd', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'y'};
			for(i = 0;i < 15;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 21 :
		{
			char error_massage[] = {'I', 's', ' ', 'a', ' ', 'd', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'y'};
			for(i = 0;i < 14;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 23 :
		{
			char error_massage[] = {'F', 'i', 'l', 'e', ' ', 't', 'a', 'b', 'l', 'e', ' ', 'o', 'v', 'e', 'r', 'f', 'l', 'o', 'w'};
			for(i = 0;i < 19;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 26 :
		{
			char error_massage[] = {'T', 'e', 'x', 't', ' ', 'f', 'i', 'l', 'e', ' ', 'b', 'u', 's', 'y'};
			for(i = 0;i < 14;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 27 :
		{
			char error_massage[] = {'F', 'i', 'l', 'e', ' ', 't', 'o', 'o', ' ', 'l', 'a', 'r', 'g', 'e'};
			for(i = 0;i < 14;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 28 :
		{
			char error_massage[] = {'N', 'o', ' ', 's', 'p', 'a', 'c', 'e', ' ', 'l', 'e', 'f', 't', ' ', 'o', 'n', ' ', 'd', 'e', 'v', 'a', 'i', 'c', 'e'};
			for(i = 0;i < 24;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 30 :
		{
			char error_massage[] = {'R', 'e', 'a', 'd', ' ', 'o', 'n', 'l', 'y', ' ', 'f', 'i', 'l', 'e', ' ', 's', 'y', 's', 't', 'e', 'm'};
			for(i = 0;i < 21;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 32 :
		{
			char error_massage[] = {'B', 'r', 'o', 'k', 'e', 'n', ' ', 'p', 'i', 'p', 'e'};
			for(i = 0;i < 11;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 40 :
		{
			char error_massage[] = {'T', 'o', 'o', ' ', 'm', 'a', 'n', 'y', ' ', 's', 'y', 'm', 'b', 'o', 'l', 'i', 'c', ' ', 'l', 'i', 'n', 'k', 's', ' ', 'e', 'n', 'c', 'o', 'u', 'n', 't', 'a',  'r', 'e', 'd'};
			for(i = 0;i < 35;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 41 :
		{
			char error_massage[] = {'O', 'p', 'e', 'r', 't', 'i', 'o', 'n', ' ', 'w', 'o', 'u', 'l', 'd', 'e', ' ', 'b', 'l', 'o', 'c', 'k'};
			for(i = 0;i < 21;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 75 :
		{
			char error_massage[] = {'V', 'a', 'l', 'u', 'e', ' ', 't', 'o', 'o', ' ', 'l', 'a', 'r', 'g', 'e', ' ', 'f', 'o', 'r', ' ', 'd', 'e', 'f', 'a', 'i', 'n', 'e', 'd', ' ', 'd', 'a', 't', 'a', ' ', 't', 'y', 'p', 'e'};
			for(i = 0;i < 38;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 89 :
		{
			char error_massage[] = {'D', 'e', 's', 't', 'n', 'a', 't', 'i', 'o', 'n', ' ', 'a', 'd', 'd', 'r', 'e', 's', 's', ' ', 'r', 'e', 'q', 'u', 'i', 'r', 'e', 'd'};
			for(i = 0;i < 27;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		case 122 :
		{
			char error_massage[] = {'Q', 'o', 'a', 't', 'a', ' ', 'e', 'x', 'c', 'e', 'e', 'd', 'e', 'd'};
			for(i = 0;i < 14;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;
		}
		default :
		{
			char error_massage[] = {'U', 'n', 'k', 'n', 'o', 'w', 'n', ' ', 'e', 'r', 'r', 'o', 'r', ' ', 'o', 'c', 'u', 'r', 'r', 'e', 'd'};
			for(i = 0;i < 21;i++)
			{
				int status_returned = write(STDERR_FILENO, error_massage + i, 1);
				if((status_returned != 1) && (errno > 0))
				{
					write_error_massage(error_number, reference);
					error_flag = 1;
					return;
					
				}
			
			}	
			break;	
		}
	}
		
	return;
}
