//服务器server.c
/*
    实现功能：客户端发送一个字符串，服务器将之反转然后回复给客户端
    笔记：
    UDP服务器端
    socket() --> bind() --> recvfrom() --> sendto() --> close()

    UDP客户端
    socket() --> sendto() --> recvfrom() -->close()

    ssize_t recvfrom(int sockfd,void *buf,size_t len,int flags,struct sockaddr *from,size_t *addrlen)
    ssize_t sendto(int sockfd,const void *buf,size_t len,int flags,const struct sockaddr *to,int addrlen)
*/
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 1234
#define MAXDATASIZE 100

//字符反转函数
char *revstr(char *str, size_t len)
{

char *start = str;
char *end = str + len - 1;
char ch;

if (str != NULL)
{
while (start < end)
{
ch = *start;
*start++ = *end;
*end-- = ch;
}
}
return str;
}

int main(int argc,char *argv[]){
    int sockfd,numbytes;
    struct sockaddr_in server;//服务器地址信息
    struct sockaddr_in client;//客户端地址信息
    socklen_t len;
    char buff[BUFSIZ];
    
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1){
        perror("Creating socket failed.");
        exit(1);
    }
    int opt=SO_REUSEADDR;
	 setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    
    if(bind(sockfd,(struct sockaddr *)&server,sizeof(server))==-1){
        perror("Bind() error");
        exit(1);
    }
    
    len=sizeof(client);
   
        while(1){
			//清空缓存
			buff[MAXDATASIZE]='\0';
			numbytes=recvfrom(sockfd,buff,MAXDATASIZE,0,(struct sockaddr *)&client,&len);
			if(!strcmp(buff,"quit"))
             break;
        	printf("You got a message from %s:%d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));
        	//接收同一客户端的多次数据
        	buff[numbytes]='\0';
        	printf("recv message:%s\n",buff);
		
          //反转字符串,并发送给客户端
          revstr(buff,numbytes);
          sendto(sockfd,buff,numbytes,0,(struct sockaddr *)&client,len);

    	}
		printf("客户端退出。\n");
    	close(sockfd);
}


//客户端client.c

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 1234


int main(int argc,char *argv[]){
	int sockfd,numbytes;
	char buff[BUFSIZ];
	struct hostent *he;
	struct sockaddr_in server,peer;
    if(argc!=2){
    printf("Usage:%s<IP Address><message>\n",argv[0]);
    exit(1);
    }
    
    if((he=gethostbyname(argv[1]))==NULL){
    	printf("gethostbyname() error\n");
    	exit(1);
    }
    
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1){
    	printf("socket() error\n");
    	exit(1);
    }
    
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr=*((struct in_addr *)he->h_addr);
    /**/
    socklen_t len;
    len=sizeof(server);
    		

    while(1){
    //只要不接收到bye~,则一直循环输入
    	printf("Entersome thing:");
		//清空缓存
		memset(buff,'\0',sizeof(buff));
      	scanf("%s",buff);
		if(!strcmp(buff,"quit")){
			sendto(sockfd,buff,strlen(buff),0,(struct sockaddr *)&server,sizeof(server));
       	printf("bye~\n");
			break;
		}
      if(sendto(sockfd,buff,strlen(buff),0,(struct sockaddr *)&server,sizeof(server))==-1){
            printf("sendto() error\n");
            exit(1);
      }
    	if((numbytes=recvfrom(sockfd,buff,BUFSIZ,0,(struct sockaddr *)&peer,&len))==-1){
			printf("recvfrom() error\n");
          exit(1);
    	}
		printf("reversed message:%s\n",buff);
	}
    close(sockfd);
}