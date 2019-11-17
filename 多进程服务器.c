第一部分，这部分代码运行有问题
#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define BACKLOG 5
#define PORT 1234
#define MAXDATASIZE 1000
#define N 100 //加密函数要用到的参数
void process_cli(int connfd,struct sockaddr_in client);
char *encry(char recvbuf[],int key[]);
int key[]={2,0,1,7,1,2,2,1,0,9};
int main(){
	int listenfd,connfd;
	pid_t pid;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int len;
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Creating socket error");
		exit(1);
	}

	//设置套接字选项
	int opt=SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	//初始化服务器信息
	bzero(&server,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	//绑定
	if(bind(listenfd,(struct sockaddr *)&server,sizeof(server))==-1){
		perror("bind() error");
		exit(1);
	}

	//监听
	if(listen(listenfd,BACKLOG)==-1){
		perror("listen() error");
		exit(1);
	}

	len=sizeof(client);
	while(1){
		if((connfd=accept(listenfd,(struct sockaddr *)&client,&len))==-1){
			perror("accept() error");
			exit(1)
		}

		//进程创建
		if((pid=fork())>0){
			//父进程
			close(connfd);
			continue;
		}
		else if(pid==0){
			//子进程
			close(listenfd);
			process_cli(connfd,client);
			exit(0);
		}
		else{
			printf("fork() error");
			exit(0);
		}
	}
	close(listenfd);
}


//客户端请求处理函数
void process_cli(int connfd,struct sockaddr_in client){
	int num;
	char recvbuf[MAXDATASIZE],sendbuf[MAXDATASIZE],cli_name[MAXDATASIZE];
	printf("you got a connection from %s\n",inet_ntoa(client.sin_addr));
	//接收客户端传来的值,如果未收到，就关闭已连接套接字
	if((num=recv(connfd,cli_name,MAXDATASIZE,0))==0){
		close(connfd);
		printf("Client disconnected\n");
		return;
	}

	cli_name[num-1]='\0';
	printf("Client's name is %s",cli_name);
	while(num=recv(connfd,recvbuf,MAXDATASIZE,0)){
		recvbuf[num]='\0';
		printf("Received client(%s) message:%s\n",cli_name,recvbuf);
		if(!strcmp(recvbuf,"quit"))
			return;
		//这里添加服务器应该实现的功能
		//加密
		//密钥2017122109
		sendbuf=encry(recvbuf,key);
		send(connfd,sendbuf,strlen(sendbuf),0);
	}
	close(connfd);
}

//加密函数
void jiami(char m[],char key[],char s[])//加密函数 
{
	int i,j;//i,j控制循环，j控制key的轮回， 
	int len_k=strlen(key);//密钥长度
	char t[N]; 
	for(i=0,j=0;m[i]!='\0';i++){ 
		if(m[i]==32){//判断空格 
			s[i]=' ';
		}
		else if(m[i]>='A'&&m[i]<='Z'){//if 是大写 
		
			s[i]=((int)(m[i]-'A')+(int)(key[j%len_k]-'A'))%26+65; 
			j++;
		}
		else if(m[i]>='a'&&m[i]<='z'){//如果是小写 
			s[i]=((int)(m[i]-'a')+(int)(key[j%len_k]-'A'))%26+97;
			j++; 
		}
			//printf("%c",s[i]); //输出密文 
	}	
	//printf("\n");//打出回车

}


char* encry(char *recvbuf,int *key){
	int i;
	for(i=0;i<strlen(recvbuf)-1;i++){

		 if(recvbuf[i]<='z'&&recvbuf[i]>='a'){
			recvbuf[i]=recvbuf[i]+key[j];
		}
		if(recvbuf[i]>'z'||recvbuf[i]>'Z'){
			recvbuf[i]-=26;
		}
		if(recvbuf[i]<'A'||recvbuf[i]<'a'){
			recvbuf[i]+=26;
		}
		j++;
		if(j==10){
			j=0;
		}
	sendbuf[i]=recvbuf[i];
	}
	int len=strlen(recvbuf)-1;
	int n=10-len%10;
	if(len%10!=0){
		for(i=0;i<(10-len%10);i++){
			sendbuf[len+i]=0;
			sendbuf[len+i]+=48;
			sendbuf[len+i]+=key[n];
			n++;
			if(n==10){
				n=0;
			}
		}
	}
	return sendbuf;
}








第二部分：

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define BACKLOG 5
#define MAXDATASIZE 100
void process_cli(int connfd,struct sockaddr_in client);
char key[]={2,0,1,7,1,2,2,1,0,9};
int main()
{
	int listenfd,connfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int len;
	pid_t pid;
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket()erro.");
		exit(1);
	 } 
	 
	 int opt=SO_REUSEADDR;
	 setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	 
	 bzero(&server,sizeof(server));
	 server.sin_family=AF_INET;
	 server.sin_port=htons(PORT);
	 server.sin_addr.s_addr=(INADDR_ANY);
	 if(bind(listenfd,(struct sockaddr*)&server,sizeof(server))==-1)
	 {
	 	perror("bind()error");
	 	exit(1);
	 }
	 if(listen(listenfd,BACKLOG)==-1)
	 {
	 	perror("listen error");
	 	exit(1);
	 }
	 
	 len=sizeof(client);
	 while(1){
	 
		 if((connfd=accept(listenfd,(struct sockaddr*)&client,&len))==-1)
		 {
		 	perror("accept()error\n");
		 	exit(1);
		 }
		 if((pid=fork())>0){
		 	close(connfd);
		 	continue;
		 }
		 else if(pid==0){
		 	close(listenfd);
			 process_cli(connfd,client);
			 exit(0);
			
		 }
		 else{
		 	printf("fork() error\n");
		 	exit(0);
		 }
		 close(listenfd);
	 }
}
void process_cli(int connfd,struct sockaddr_in client){
	int num;
	char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE],
	cli_name[MAXDATASIZE];
	int i=0;
	int j=0;
	printf("You got a connection from %s.\n",inet_ntoa(client.sin_addr));
	num=recv(connfd,cli_name,MAXDATASIZE,0);
	if(num==0){
		close(connfd);
		printf("Client disconnected");
		return;
	}
	cli_name[num-1]='\0';
	printf("Client's name is %s.\n",cli_name);
	while(num=recv(connfd,recvbuf,MAXDATASIZE,0))
	{	
		recvbuf[num]='\0';
		printf("Received cient(%s) message:%s",cli_name,recvbuf);
	for(i=0;i<strlen(recvbuf)-1;i++){

		if(recvbuf[i]<='z'&&recvbuf[i]>='a'){
			recvbuf[i]=recvbuf[i]+key[j];
		}
		if(recvbuf[i]>'z'||recvbuf[i]>'Z'){
			recvbuf[i]-=26;
		}
		if(recvbuf[i]<'A'||recvbuf[i]<'a'){
			recvbuf[i]+=26;
		}
		j++;
		if(j==10){
			j=0;
		}
	sendbuf[i]=recvbuf[i];
	}
	int len=strlen(recvbuf)-1;
	int n=10-len%10;
	if(len%10!=0){
		for(i=0;i<(10-len%10);i++){
			sendbuf[len+i]=0;
			sendbuf[len+i]+=48;
			sendbuf[len+i]+=key[n];
			n++;
			if(n==10){
				n=0;
			}
		}
	}
	memset(&recvbuf,'\0',sizeof(recvbuf));
	int lenth=strlen(sendbuf);
	sendbuf[lenth]='\0';
	send(connfd,sendbuf,strlen(sendbuf),0);
	memset(&sendbuf,'\0',sizeof(sendbuf));
	//printf("%s",sendbuf);
		
	}
	close(connfd);
}
