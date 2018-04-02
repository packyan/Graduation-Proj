#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<string>
#include<vector>
#define MAXLINE 4096

using std::string;
using std::cout;
using std::endl;

unsigned char packages_head[] ={0xff, 0xfe}, package_end[] = {0xbe,0xef,'\0'};

char* pack_msg(char *package, string msg)
{
	memset(package,0,MAXLINE);
    memcpy((char*)package, (char*)packages_head,2);
    strcat((char*)package, msg.c_str());
    strcat((char*)package, (char*)package_end);
    return (char*) package;
}
int main(int argc, char** argv)
{
    int    sockfd, n, rec_len;
    char    recvline[MAXLINE], buf[MAXLINE];//, sendline[4096]
    char package[MAXLINE];
	//string sendmsg = "is the data part";
    std::vector<string> sendmsg(5,"DATA");
    struct sockaddr_in   servaddr;

//    if( argc != 2){
//    printf("usage: ./client <ipaddress>\n");
//    exit(0);
//    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
    exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
//    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
//    printf("inet_pton error for %s\n",argv[1]);
//    exit(0);
//    }
 	if( inet_pton(AF_INET, "0.0.0.0", &servaddr.sin_addr) <= 0){
    printf("inet_pton error for %s\n",argv[1]);
    exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }

	int count = 0;
	while(count < 5)
	{
		
		//printf("send msg to server: ");
		//fgets(sendline, 4096, stdin);
		//const char *sendline = sendmsg[count].c_str();
		//std::cout<<sendmsg[count]<<std::endl;
		string temp = (char*)pack_msg(package, sendmsg[count]);
		const char *sendline = temp.c_str();
		for(int i = 0; i < temp.size(); ++i){
		cout << (int)*(sendline+i) << ' ';
}
		cout <<endl;
		if( send(sockfd, sendline, strlen(sendline), 0) < 0)
		{
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
		}
        count++;
//		if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {  
//		if(errno==EAGAIN) printf("Time out ");
//		printf("recv msg error: %s(errno: %d)\n", strerror(errno), errno);
//		//perror("recv error");  
//		exit(1);  
//		}  
//		buf[rec_len]  = '\0';
//		if(rec_len != 1 ) printf("Received : %s \n",buf); 
//		else printf("No reply\n");
		
	}
	close(sockfd);
    exit(0);
}
