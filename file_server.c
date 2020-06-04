// server code
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define IP_PROTOCOL 0 
#define PORT_NO 15050 
#define NET_BUF_SIZE 32
#define cipherKey 'S' 
#define sendrecvflag 0 
#define nofile "File Not Found!" 

// funtion to clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

// funtion to encrypt 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// funtion sending file 
int sendFile(FILE* fp, char* buf, int s) 
{ 
	int i, len; 
	if (fp == NULL) { 
		strcpy(buf, nofile); 
		len = strlen(nofile); 
		buf[len] = EOF; 
		for (i = 0; i <= len; i++) 
			buf[i] = Cipher(buf[i]); 
		return 1; 
	}
	// File Not Found Handled

	char ch, ch2; 
	for (i = 0; i < s; i++) { 
		ch = fgetc(fp); 
		ch2 = Cipher(ch); 
		buf[i] = ch2; 
		if (ch == EOF) 
			return 1; 
	} 
	// Encountered EOFs
	return 0; 
} 

int is_Regular(const char *path)
{
	struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
// checks if it is a regular file

// driver code 
int main() 
{
	// initializations
	int sockfd, nBytes; 
	struct sockaddr_in addr_con; 
	unsigned int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(PORT_NO); 
	addr_con.sin_addr.s_addr = INADDR_ANY; 
	char net_buf[NET_BUF_SIZE]; 
	FILE* fp;
	
	// socket creation
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL); 

	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd); 

	// binding created socket
	if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0) 
		printf("\nSuccessfully binded!\n"); 
	else
		printf("\nBinding Failed!\n"); 

	while (1)
	{
		// Terminal Loop
		printf("\nWaiting for Instruction...\n"); 

		// clear buffer
		clearBuf(net_buf); 

		// receive instruction
		nBytes = recvfrom(sockfd, net_buf, 
						NET_BUF_SIZE, sendrecvflag, 
						(struct sockaddr*)&addr_con, &addrlen);


		// check if listall instruction
		if(strcmp(net_buf, "listall") == 0)
		{
			struct dirent *de;
			// pointer for directory entry 
  
			// returns pointer to traverse through directory contents  
			DIR *dr = opendir("."); 
		  
			if(dr == NULL)
			// could not access directory 
			{ 
				printf("Could not access server\n"); 
				return 0; 
			}

			char **names = (char **)malloc(sizeof(char*) * 1000);
			int count = 0;

			FILE *temporary_file = fopen("random_file.txt", "w");
			// creates temporary file on server

			while ((de = readdir(dr)) != NULL) 
			{
				names[count++] = (char *)de->d_name;
				if(strcmp(de->d_name, "random_file.txt") == 0 || strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0 || !is_Regular(de->d_name))
					continue;
				fprintf(temporary_file, "%s\n", names[count - 1]);
			}
			// adds data to the temporary file

			fclose(temporary_file);
			closedir(dr);


			fp = fopen("random_file.txt", "r");
			
			while (1)
			{ 
				// send the temporary file to client
				if (sendFile(fp, net_buf, NET_BUF_SIZE)) { 
					sendto(sockfd, net_buf, NET_BUF_SIZE, 
						sendrecvflag, 
						(struct sockaddr*)&addr_con, addrlen); 
					break; 
				}
				// stop sending

				// keep sending
				sendto(sockfd, net_buf, NET_BUF_SIZE, 
					sendrecvflag, 
					(struct sockaddr*)&addr_con, addrlen); 
				clearBuf(net_buf); 
			}

			if(fp != NULL)
				fclose(fp);
			remove("random_file.txt");
			// removes temporary file from server
			continue;	 
		}

		// send a file
		fp = fopen(net_buf, "r"); 
		printf("\nFile Name Received: %s\n", net_buf); 
		if (fp == NULL) 
			printf("\nFile open failed!\n");
			// if file does not exist
		else
			printf("\nFile Successfully opened!\n"); 

		while (1) { 

			// send the file
			if (sendFile(fp, net_buf, NET_BUF_SIZE)) { 
				sendto(sockfd, net_buf, NET_BUF_SIZE, 
					sendrecvflag, 
					(struct sockaddr*)&addr_con, addrlen); 
				break;
			}
			// stop sending

			// keep sending
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, 
				(struct sockaddr*)&addr_con, addrlen); 
			clearBuf(net_buf); 
		} 
		if (fp != NULL) 
			fclose(fp); 
	} 
	return 0; 
} 
