// client code
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define IP_PROTOCOL 0 
#define IP_ADDRESS "127.0.0.1"
// localhost 
#define PORT_NO 15050 
#define NET_BUF_SIZE 32
#define cipherKey 'S' 
#define sendrecvflag 0

using namespace std;

FILE *fptr;

// funtion to clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

// function for decryption 
char Cipher(char ch)
{ 
	return ch ^ cipherKey; 
} 

// function to receive file 
int recvFile(char* buf, int s, int print_or_put) 
{ 
	// print_or_put 1 -> Print, 2 -> Put
	int i; 
	char ch; 
	for (i = 0; i < s; i++) { 
		ch = buf[i]; 
		ch = Cipher(ch); 
		if (ch == EOF) 
			return 1;
			// entire file received
		else
		{
			if(print_or_put == 1)
				printf("%c", ch);
				// if listall
			else if(print_or_put == 2)
				putc(ch, fptr);
				// if send
		}
	} 
	return 0; 
} 

// driver code 

int main() 
{
	// initializations
	int sockfd, nBytes; 
	struct sockaddr_in addr_con; 
	unsigned int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(PORT_NO); 
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
	char net_buf[NET_BUF_SIZE]; 
	FILE* fp; 

	// socket creation
	sockfd = socket(AF_INET, SOCK_DGRAM, 
					IP_PROTOCOL); 

	if(sockfd < 0)
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd);
	// socket creation being verified

	while (1)
	{
		clearBuf(net_buf);
		// buffer cleared
		printf(">>");
		// prompt displayed
		scanf("%[^\n]%*c", net_buf); 
		// command received

		if(strcmp("listall", net_buf) == 0)
		{
			// handles listall command
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
			sendrecvflag, (struct sockaddr*)&addr_con, 
			addrlen);
			
			while (1)
			{ 
				// receive data
				clearBuf(net_buf); 
				nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
								sendrecvflag, (struct sockaddr*)&addr_con, 
								&addrlen); 

				// take action based on command type
				if (recvFile(net_buf, NET_BUF_SIZE, 1)) { 
					break; 
				} 
			}
			continue;
		}

		// checks if invalid command

		char file_name[100];
		int i;
		for(i = 0; i < 100; ++i)
			file_name[i] = '\0';

		// preprocessing
		if(net_buf[0] == 's' && net_buf[1] == 'e' && net_buf[2] == 'n' && net_buf[3] == 'd' && net_buf[4] == ' ')
		{
			for(i = 5; net_buf[i] != '\0'; ++i)
				file_name[i - 5] = net_buf[i];

			int j;

			for(j = i - 5; j < 100; ++j)
				file_name[j] = '\0';

			for(j = 0; j < 100; ++j)
			{
				net_buf[j] = file_name[j];
			}
		}

		else
		{
			printf("Invalid Command\n");
			continue;
		}
		strcpy(file_name, net_buf);
		
		// all parameters initialized

		// send request for file
		sendto(sockfd, net_buf, NET_BUF_SIZE, 
			sendrecvflag, (struct sockaddr*)&addr_con, 
			addrlen);

		// pointer for file to be written onto
		fptr = fopen(net_buf, "ab+");

		while (1)
		{ 
			// receive data
			clearBuf(net_buf); 
			nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
							sendrecvflag, (struct sockaddr*)&addr_con, 
							&addrlen); 

			// process according to command
			if (recvFile(net_buf, NET_BUF_SIZE, 2)) { 
				break; 
			} 
		}

		fclose(fptr);

		// Check if File Not Found
		FILE *fileStream = fopen(file_name, "r");
		char fileText[100];
		fgets(fileText, 50, fileStream);
		if(strcmp(fileText, "File Not Found!") == 0)
		{
			printf("File Does Not Exist!\n");
			remove(file_name);
		}
		else
		{
			printf("File Downloaded Successfully\n");
		}
		fclose(fileStream);
	}
	return 0; 
} 
