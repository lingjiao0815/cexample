#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define PORT 1234
//#define SERVER_IP "192."

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("usage: ./client serverip\n");
		return 0;
	}

	int s = 0;
	struct sockaddr_in addr = {0};
	char buffer[256] = {0};
	
	if((s = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}
	
	/* 填写sockaddr_in结构*/
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	/* 尝试连线*/
	if(connect(s,(struct sockaddr *)&addr,sizeof(addr))<0){
		perror("connect");
		exit(1);
	}
	

	while(1){
		bzero(buffer,sizeof(buffer));
		/* 从标准输入设备取得字符串*/
		read(STDIN_FILENO,buffer,sizeof(buffer));
		/* 将字符串传给server端*/
		if(send(s,buffer,sizeof(buffer),0)<0){
			perror("send");
			exit(1);
		}

		bzero(buffer, sizeof(buffer));
		if(read(s,buffer,sizeof(buffer)) > 0){
			printf("recv :%s\n", buffer);
		}
		
	}
}

