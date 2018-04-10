#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include <iostream>
#define MAXLINE 4096
using std::cout;
using std::endl;
#pragma pack(push, 1) // 取消内存大小自动对齐
struct face_fit_msg{
unsigned char packages_head[2] ={0xff, 0xfe};
double face_fit_data[4];
unsigned char package_end[3] = {0xbe,0xef,'\0'};
};
#pragma pack(pop)

int main(int argc, char** argv)
{
    int    listenfd, connfd;
    struct sockaddr_in   servaddr;
    char    buff[4096];
    int     n;
	face_fit_msg face_fit_recv;
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
    printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
    printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }

    if( listen(listenfd, 10) == -1){
    printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }

    printf("======waiting for client's request======\n");
    while(1){
    if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
        continue;
    }
	memset(&face_fit_recv, 0x00, sizeof(face_fit_msg));
	recv(connfd, (char*)&face_fit_recv, sizeof(face_fit_msg), 0);
	cout << face_fit_recv.face_fit_data[0] << endl;
/*    n = recv(connfd, buff, MAXLINE, 0);*/
/*    buff[n] = '\0';*/
/*    printf("recv msg from client: %s\n", buff);*/

 	if(send(connfd, "Hello,you are connected!\n", 26,0) == -1)  
	{
		printf("send msg error: %s(errno: %d)",strerror(errno),errno);
		perror("send error"); 
		continue;
	}
    close(connfd);
    }

    close(listenfd);
}
