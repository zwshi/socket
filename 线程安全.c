//线程安全
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>

#define PORT 1234
#define BACKLOG 5
#define MAXDATASIZE 1000
char mkey[]={2,0,1,7,1,2,2,1,0,9};

void process_cli(int connfd,struct sockaddr_in client);
void savedata_r(char* recvbuf,int len,char* cli_data);
void* function(void* arg);
struct ARG{
	int connfd;
	struct sockaddr_in client;
};

pthread_key_t key;
pthread_once_t once=PTHREAD_ONCE_INIT;
static void destructor(void *ptr){
	free(ptr);
}
static void creatkey_once(void){
	pthread_key_create(&key,destructor);	//创建线程
}

struct ST_DATA{
	int index;
};

int main(){
	int listenfd,connfd;
	pthread_t tid;
	struct ARG *arg;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t len;

	if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket() error!");
		exit(1);
	}

	int opt=SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	bzero(&server,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(listenfd,(struct sockaddr *)&server,sizeof(server))==-1){
		perror("bind() error!");
		exit(1);
	}

	if(listen(listenfd,BACKLOG)==-1){
		perror("listen() error!");
		exit(1);
	}

	len=sizeof(client);
	while(1){
		if((connfd=accept(listenfd,(struct sockaddr *)&client,&len))==-1){
			perror("accept() error!");
			exit(1);
		}
		arg=(struct ARG*)malloc(sizeof(struct ARG));
		arg->connfd=connfd;
		memcpy((void *)&arg->client,&client,sizeof(client));
		if(pthread_create(&tid,NULL,function,(void*)arg)){
			perror("pthread_create() error!");
			exit(1);
		}
	}
	close(listenfd);
}


void process_cli(int connfd,struct sockaddr_in client){
	int num;
	char cli_data[1000];
	char recvbuf[MAXDATASIZE],sendbuf[MAXDATASIZE],cli_name[MAXDATASIZE];
	printf("you got a connection from %s.\n",inet_ntoa(client.sin_addr));
	if((num=recv(connfd,cli_name,MAXDATASIZE,0))==0){
		close(connfd);
		printf("client disconnected\n");
		return;
	}

	cli_name[num]='\0';
	printf("client's name is %s,\n",cli_name);

	while(num=recv(connfd,recvbuf,MAXDATASIZE,0)){
		recvbuf[num]='\0';
		printf("Recvived client(%s) message:%s\n",cli_name,recvbuf);
		savedata_r(recvbuf,num,cli_data);

		if(!strcmp(recvbuf,"quit"))
			break;
		//处理数据
		
		//维吉尼亚加密
		int i,j=0;
		for(i=0;i<strlen(recvbuf)-1;i++){

		if(recvbuf[i]<='z'&&recvbuf[i]>='a'){
			recvbuf[i]=recvbuf[i]+mkey[j];
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
				sendbuf[len+i]+=mkey[n];
				n++;
				if(n==10){
					n=0;
				}
			}
		}

		int lenth=strlen(sendbuf);
		sendbuf[lenth]='\0';
		send(connfd,sendbuf,strlen(sendbuf),0);
		//清理接收缓冲区
		memset(&recvbuf,'\0',sizeof(recvbuf));
		//清理发送缓冲区
		memset(&sendbuf,'\0',sizeof(sendbuf));
		
	}
	close(connfd);
	printf("client(%s) closed connection.\nUser's data:%s\n",cli_name,cli_data);
}

void* function(void* arg){
	struct ARG *info;
	info=(struct ARG *)arg;
	process_cli(info->connfd,info->client);
	free(arg);
	pthread_exit(NULL);
}

void savedata_r(char* recvbuf,int len,char* cli_data){
	struct ST_DATA* data;
	pthread_once(&once,creatkey_once);
	if((data=(struct ST_DATA *)pthread_getspecific(key))==NULL){
		data=(struct ST_DATA *)malloc(sizeof(struct ST_DATA));
		pthread_setspecific(key,data);
		data->index=0;
	}
	int i=0;
	while(i<len){
		cli_data[data->index++]=recvbuf[i];
		i++;
	}
	cli_data[data->index]='\0';
}




#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 1234
#define MAXDATASIZE 100

int main(int argc,char *argv[]){
	int sockfd,num;
	char recvbuf[MAXDATASIZE],sendbuf[MAXDATASIZE],cli_name[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in server;

	if(argc!=2){
		printf("格式错误！");
		exit(1);
	}
	if((he=gethostbyname(argv[1]))==NULL){
		printf("gethostbyname() error!");
		exit(1);
	}

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
		printf("socket() error!");
		exit(1);
	}

	bzero(&server,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);
	server.sin_addr=*((struct in_addr *)he->h_addr);

	if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))==-1){
		printf("connect() error!");
		exit(1);
	}
	printf("input the client's name:");
	scanf("%s",cli_name);
	send(sockfd,cli_name,strlen(cli_name),0);
	while(1){
		printf("input the message:");
		scanf("%s",sendbuf);
		send(sockfd,sendbuf,strlen(sendbuf),0);
		if(!strcmp(sendbuf,"quit"))
			break;
		if((num=recv(sockfd,recvbuf,MAXDATASIZE,0))==-1){
			printf("recv() error!");
			exit(1);
		}
		recvbuf[num-1]='\0';
		printf("encrypted message:%s\n",recvbuf);	
	}
	
	close(sockfd);
}