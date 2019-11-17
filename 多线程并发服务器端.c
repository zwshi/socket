#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>

#define PORT 1234
#define BACKLOG 5
#define MAXDATASIZE 1000
void process_cli(int connfd,struct sockaddr_in client);
char key[]={2,0,1,7,1,2,2,1,0,9};
//线程的执行函数
void* function(void* arg);
struct ARG{
	int connfd;
	struct sockaddr_in client;
}

main(){
	int listenfd,connfd;
	struct ARG *arg;
	pthread_t tid;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t len;

	//创建监听套接字
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("creating socket error.");
		exit(1);
	}

	//设置套接字选项
	int opt=SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	//初始化服务器地址
	bzero(&server,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	//将套接字绑定到本地
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

	//循环接收
	while(1){
		if((connfd=accept(listenfd,(struct sockaddr *)&client,&len))==-1){
			perror("accept() error");
			exit(1);
		}

		//每次为新线程分配空间
		arg=(struct ARG*)malloc(sizeof(struct ARG));
		arg->connfd=connfd;
		memcpy((void *)&arg->client,&client,sizeof(client));//为结构体里的成员赋值

		//创建线程
		if(pthread_create(&tid,NULL,function,(void*)arg)){
			perror("pthread_create() error");
			exit(1);
		}
	}
	close(listenfd);
}


//客户请求处理函数
void process_cli(int connfd,struct sockaddr_in client){
	int num;
	char recvbuf[MAXDATASIZE],sendbuf[MAXDATASIZE],cli_name[MAXDATASIZE];

	printf("you got a connection from %s.\n",inet_ntoa(client.sin_addr));
	//接收客户端发送的名字,如果为空则关闭连接
	if(recv(connfd,cli_name,MAXDATASIZE,0)==0){
		close(connfd);
		printf("client disconnected");
		return;
	}
	cli_name[num]='\0';
	printf("client's name is %s\n",cli_name);

	while(num=recv(connfd,recvbuf,MAXDATASIZE,0)){
		recvbuf[num]='\0';
		printf("recevied client(%s) message:%s",cli_name,recvbuf);
		//清理接收缓冲区
		memset(&recvbuf,'\0',sizeof(recvbuf));
		
		//维吉尼亚加密
		int i,j=0;
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

		int lenth=strlen(sendbuf);
		sendbuf[lenth]='\0';
		send(connfd,sendbuf,strlen(sendbuf),0);
	}
	close(connfd);
}

void* function(void* arg){
	struct ARG *info;
	info=(struct ARG*)arg;
	process_cli(info->connfd,info->client);
	free(arg);
	pthread_exit(NULL);
}