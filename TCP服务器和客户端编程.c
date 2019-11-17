//服务器端service.c
/*
    实现功能：客户端发送一个字符串，服务器将之反转然后回复给客户端
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>

#define PORT 8888  //定义服务器端口
#define BACKLOG 1  //最大连接数

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
    int listenfd,connectfd;
    int numbytes,i;
    
    struct sockaddr_in server;
    struct sockaddr_in client;
    char buff[BUFSIZ];
    int addrlen;
    
    //创建tcp套接字
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket() error");
        exit;
    }
    
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    addrlen=sizeof(server);
    
	int opt=SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    //綁定
    if(bind(listenfd,(struct sockaddr *)&server,sizeof(server))==-1){
        perror("bind() error");
        exit;
    }else{
        printf("bind success\n");
    }
    
    //监听
    if(listen(listenfd,BACKLOG)==-1){
        perror("listen() error");
        exit;
    }else{
        printf("listening.....\n");
    }
    
    if((connectfd=accept(listenfd,(struct sockaddr *)&client,&addrlen))==-1){
        perror("accept() error");
        exit;
    }else{
        printf("You got a connection from client's ip is %s,port is %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    }
    
    numbytes=send(connectfd,"welcome to my server\n",21,0);
    while((numbytes=recv(connectfd,buff,BUFSIZ,0))>0){
        buff[numbytes]='\0';
        printf("%s\n",buff);
        if(!strcmp(buff,"quit")){
        	//如果输入的是quit，则返回给客户端一个标志
        	if(send(connectfd,"bye~",4,0)<0){
            	perror("send() error");
            	return 1;
        	}
            break;//如果客户端输入的是quit，就退出循环
        }
        
        //反转字符串
        revstr(buff,numbytes);
        if(send(connectfd,buff,numbytes,0)<0){
            perror("send() error");
            return 1;
        }
    }
    
    //关闭套接字
    close(connectfd);
    close(listenfd);
    return 0;
}





客户端client.c


#include<stdio.h>
#include<stdlib.h>   //exit函数所在文件头
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

#define PORT 8888 //服务器端口

int main(int argc,char *argv[]){
    int sockfd,numbytes;
    char buf[BUFSIZ];
    char serv_ip[20],guest_ip[20];
    struct hostent *he;
    struct sockaddr_in server;
    
    if(argc!=2){
        printf("Usage:%s",argv[0]);
		exit;
    }
    
    if((he=gethostbyname(argv[1]))==NULL){
        printf("gethostbyname() error!\n");
		  exit;
    }
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        printf("socket() error!\n");
		  exit;
    }
    
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr=*((struct in_addr *)he->h_addr);
    if((connect(sockfd,(struct sockaddr *)&server,sizeof(server)))==-1){
        printf("connect() error!\n");
		exit;
    }

    //使用getsockname和getpeername
    /*
	返回与某个套接字关联的本地协议地址（getsockname），或者返回与某个套接字关联的外地协议地址即得到对方的地址（getpeername）
	int getsockname(int sockfd,struct sockaddr* localaddr,socklen_t *addrlen);
	int getpeername(int sockfd,struct sockaddr* peeraddr,socklen_t *addrlen);
    */
	
    struct sockaddr_in serv,guest;
    socklen_t serv_len=sizeof(serv);
    socklen_t guest_len=sizeof(guest);
    getsockname(sockfd,(struct sockaddr *)&guest,&guest_len);//得到本地地址
    getpeername(sockfd,(struct sockaddr *)&serv,&serv_len);//得到远程服务器的地址
    
    inet_ntop(AF_INET,&guest.sin_addr,guest_ip,sizeof(guest_ip));
    inet_ntop(AF_INET,&serv.sin_addr,serv_ip,sizeof(serv_ip));
    printf("host %s:%d/*******/guest %s:%d\n",serv_ip,ntohs(serv.sin_port),guest_ip,ntohs(guest.sin_port));


    //接收服务器端信息
    if((numbytes = recv(sockfd, buf, BUFSIZ,0))==-1){
	    printf("recv() error");
	    exit;

    }
  
    buf[numbytes]='\0';  
    printf("%s",buf);
    while(1)
    {
        if(!strcmp(buf,"bye~")){
            break;
        }
        printf("Entersome thing:");
        scanf("%s",buf);
        numbytes = send(sockfd, buf, strlen(buf), 0);
            numbytes=recv(sockfd,buf,BUFSIZ,0);  
        buf[numbytes]='\0'; 
        printf("received:%s\n",buf);  
    }
    close(sockfd);
    return 0;
}