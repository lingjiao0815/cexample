#include <stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#define PORT 1234
#define MAXSOCKFD 10

void *data_recv_thread(void *arg)
{
	char buffer[256] = {0};
	int *s = (int *)arg;

	while(1){

		bzero(buffer, sizeof(buffer));
		if(read(*s,buffer,sizeof(buffer)) > 0){
			printf("recv :%s\n", buffer);
		}
	}
}


int main()
{
	
	int sockfd = 0;
	int newsockfd = 0;
	int is_connected[MAXSOCKFD] = {0};
	int fd = 0;
	struct sockaddr_in addr = {0};
	
	int addr_len = sizeof(struct sockaddr_in);
	fd_set readfds = {0};
	char buffer[256] = {0};
	char msg[ ] = "Welcome to server!";
	
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}

	bzero(&addr,sizeof(addr));
	addr.sin_family =AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0){
		perror("connect");
		exit(1);
	}
	
	if(listen(sockfd,3)<0){
		perror("listen");
		exit(1);
	}
	
	
	if((newsockfd = accept (sockfd,(struct sockaddr *)&addr,&addr_len))<0){
		perror("accept");
	}

	
	pthread_t thread;
	pthread_create(&thread, NULL, data_recv_thread, (void *)&newsockfd);
	
	printf("connect from %s\n",inet_ntoa(addr.sin_addr));
	while(1){
		bzero(buffer, sizeof(buffer));
		gets(buffer);
		write(newsockfd,buffer,strlen(buffer));
	}
}

