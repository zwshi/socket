#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/utsname.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
	struct hostent *he;
	struct utsname myname;
	struct in_addr addr;
	char temp[100];

    //通过uname()函数返回主机名，放到myname结构里    
	if(uname(&myname)<0)
		return 0;
    if((he=gethostbyname(myname.nodename))==NULL)   //这里的myname.nodename是指主机的名称
	{
		perror("error");
		exit(1);
	}

	printf("OS name is %s\n",myname.sysname);
	printf("node name is %s\n",myname.nodename);
    printf("OS version release is %s\n",myname.release);
	printf("hardware version is %s\n",myname.version);
	printf("hardware type is %s\n",myname.machine);

	memcpy(&addr,he->h_addr,sizeof(addr));
	printf("host ip is %s\n",inet_ntoa(addr));
	return 0;
}


/**
 * gethostbyname(),查找主机名，执行成功返回一个指向结构hostent的指针，该结构包含了该主机的所有ipv4或ipv6地址，失败则返回空指针，在头文件<netdb.h>
 * struct hostent * gethostbyname(const char *hostname)
 * 参数hostname是主机的域名地址
 * 工作流程：gethostbyname()函数首先在/etc/hosts文件查找是否有匹配的主机名，如果没有，则根据在域名解析配置文件/etc/resolv.conf中指定的本地域名服务器的地址，向本地域名服务器发送地址解析请求，如果本地域名服务器能够解析，则返回UDP数据包说明结果，否则本地域名服务器将向上一层的域名服务器发送域名解析请求
 * 
 * uname(),返回当前主机的名字，经常与gethostbyname()一起使用来确定本地主机的ip地址，所在头文件<sys/utsname.h>
 * int uname(struct utsname *name);         返回非负值表示成功，返回-1则出错
 * utsname结构里的成员有：sysname   操作系统的名称；nodename    本机的名称；release     OS的release；version    操作系统版本；machine   硬件版本
 * gethostname(),返回当前主机的名字，但返回信息比uname()少，头文件<unistd.h>
 * 
 * 其他：
 * gethostbyaddr(),查询指定ip地址对应的主机域名地址，所在头文件<netdb.h>
 * getserbyname()、getserbyport();前者通过服务名得到端口号，后者通过端口号来获取服务名，头文件<netdb.h>
