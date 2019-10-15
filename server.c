#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define IP "127.0.0.1"
#define PORT 10000

int main(void){
	int s_socket, c_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int rcvlen;
	int itemp;
	char buffer[100];
	char rcvbuf[100];
	char cmpbuf1[100];
	char cmpbuf2[100];
	char filebuf[100];
	char buff[255];
	FILE *fp;

	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = inet_addr(IP);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if(bind(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){
		printf("Cannot bind.\n");
		return -1;
	}

	if(listen(s_socket, 5)==-1){
		printf("Listen failed.\n");
		return -1;
	}
	while(1){
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		printf("Client is connected.\n");
		strcpy(buffer, " Server is connected.\n");
		write(c_socket, buffer, strlen(buffer));

		while(1){
			strcpy(buffer, "\0");
			rcvlen = read(c_socket, rcvbuf, sizeof(rcvbuf));
			rcvbuf[rcvlen] = '\0';
			printf("[%s] recieved.\n", rcvbuf);

			if(strcmp(rcvbuf, "안녕하세요")==0){
				strcpy(buffer, "안녕하세요. 만나서 반가워요");
				buffer[strlen(buffer)] = '\0';
				write(c_socket, buffer, strlen(buffer));
			}
			else if(strcmp(rcvbuf, "이름이 뭐야?")==0){
				strcpy(buffer, "내이름은 심지훈이야.");
				buffer[strlen(buffer)] = '\0';
				write(c_socket, buffer, strlen(buffer));
			}
			else if(strcmp(rcvbuf, "몇살이야?")==0){
				strcpy(buffer, "나는 23살이야.");
				buffer[strlen(buffer)] = '\0';
				write(c_socket, buffer, strlen(buffer));
			}
			else if(strncasecmp(rcvbuf, "strlen", 6)==0){
				printf(buffer, "문자열 길이=%d", strlen(rcvbuf)-7);
				write(c_socket, buffer, strlen(buffer));
			}
			else if(strncasecmp(rcvbuf,"readfile",8)==0){
				strtok(rcvbuf," ");
				strcpy(filebuf, strtok(NULL," "));
			fp= fopen(filebuf,"r");
			if(fp){
				while(fgets(buff, 255, (FILE *)fp)){
					printf("%s",buff);
				}
			}
			else if(strncasecmp(rcvbuf,"exec",4)==0){
				strtok(rcvbuf," ");
				strcpy(filebuf, strtok(NULL," "));
				system(filebuf)
				
			}
				
			}
			else if(strncasecmp(rcvbuf, "strcmp", 6)==0){
				strtok(rcvbuf, " ");
				strcpy(cmpbuf1, strtok(NULL, " "));
				strcpy(cmpbuf2, strtok(NULL, " "));
				if(strcmp(cmpbuf1, cmpbuf2)==0){
					strcpy(buffer, "문자열 일치(값 == 0)");
				}
				else{
					sprintf(buffer, "문자열 불일치(값 == %d)", strcmp(cmpbuf1, cmpbuf2));
				}
				write(c_socket, buffer, strlen(buffer));
			}
			else if(strncmp(rcvbuf, "quit", 4)==0 || strncmp(rcvbuf, "kill server", 11)==0)
				break;
			else{
				write(c_socket, "응답없음", 12);
			}
		}
		close(c_socket);
		if(strncmp(rcvbuf, "kill server", 11)==0)
			break;
	}
	close(s_socket);

	return 0;
}
