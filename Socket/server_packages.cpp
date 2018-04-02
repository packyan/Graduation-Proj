#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string>
#include<vector>
#include<iostream>
using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
#define MAXLINE 4096
bool chuck_msg(char *buff);
int main(int argc, char** argv)
{
    int    listenfd, connfd;
    struct sockaddr_in   servaddr;
    char    buff[4096];
    int     n;
	vector<string> recv_data;
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
    n = recv(connfd, buff, MAXLINE, 0);
	cout << "the packages length is " << n+1 << " chars"<< endl; 
    //buff[n] = '\0';
    //printf("recv msg from client: %s\n", buff);
	//string buff_str = buff;
	chuck_msg(buff);
	//recv_data.push_back(buff_str);
	
//	cout<<"recv :"<< recv_data.size() << " packages" << endl;
//	//if(recv_data.size()==10) 
//	for(auto it = recv_data.begin(), it_end = recv_data.end(); it != it_end; ++it){
//		for(auto c : (*it)) cout << (int) c << ' ';
//		cout << endl;
//		cout << "recv msg : " << *it << endl;
//}
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

bool chuck_msg(char *buff){
	
	//check function
	string rev_msg = buff;
	size_t cnt = 0;
	string::iterator it = rev_msg.begin(), it_end = rev_msg.end();
	while(it != it_end){
	if((int)*it != -1 || (int)*(it+1) != -2){
		cout << "head chuck error" <<endl;
		return 0;
	}
	else {
		cout << "head checked"<<endl;
		string::iterator msg_end;
		for(string::iterator it_begin = it; it_begin != it_end; ++it_begin){
			if(it_begin == it_end - 1) {
				cout << "end chuck error" << endl;
				return 0;			
			}
			if(*it_begin == -66  && *(it_begin + 1) == -17){
				cout << "end checked" <<endl;
				msg_end = it_begin;
				break;
			}
			
		}// msg_end now is the begining of end-package


		cout << "do someting to unpack" <<endl;
		//for example
		string::iterator msg = it + 2;
		size_t package_length = msg_end - msg;
		for (; msg != msg_end; ++msg)
		{
			cout << *msg <<' ';
		}
		++cnt;
		cout << "package length : " << package_length <<endl;
		cout << "recived " << cnt << " packages"<<endl;
		it = it + (package_length + 2 + 2); // move package_length + head_length + end_length
		//memset(buff,0,MAXLINE);
		cout << endl;
				
			
	}
	}
	return 1;
}
