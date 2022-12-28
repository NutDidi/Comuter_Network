#include<iostream>
#include<winsock.h>
#include<string.h>
#include<time.h> 
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;
#define max_buf 1062
const int PORT = 5001;//端口号
#define IP "127.0.0.1"//IP
const int maxlen = 10;
char username[10];
SOCKET my_socket;
void printTime();
char my_IP[15];
int my_Port;

void printTime()
{	
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
    printf( "[%4d/%02d/%02d %02d:%02d:%02d] ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond); 
}

void getTime(char* s)
{	
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
    sprintf( s," %4d/%02d/%02d %02d:%02d:%02d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond); 
}


void writebuf(char* sendbuf,char* username, char* msg, char* timemsg)
{
    if(string(msg) == "Esc") sendbuf[0]='1';//退出请求
    else sendbuf[0]='0';    //聊天消息
    if(string(msg).length() <= 0) 
    {
        sendbuf[0]='2'; //登陆消息
        msg[0]='[';
        msg[1]='L';
        msg[2]='o';
        msg[3]='g';
        msg[4]='i';
        msg[5]='n';
        msg[6]=' ';
        msg[7]='I';
        msg[8]='n';
        msg[9]=']';

    }
    sendbuf[1]='[';
    sendbuf[2]='U';
    sendbuf[3]='s';
    sendbuf[4]='e';
    sendbuf[5]='r';
    sendbuf[6]=':';
    sendbuf[7]=' ';
    int index=7;
    for(int i=0;i<strlen(username);i++)
    {
        index++;
        sendbuf[index]=username[i];
    }
    index++;
    sendbuf[index]=']';
    for(int i=0;i<strlen(msg);i++)
    {
        index++;
        sendbuf[index]=msg[i];
    }
    index++;
    sendbuf[index]='(';
    for(int i=0;i<strlen(timemsg);i++)
    {
        index++;
        sendbuf[index]=timemsg[i];
    }    
    index++;
    sendbuf[index]=')';   
    return; 
    
}

DWORD WINAPI recvmsg(LPVOID X)
{
    SOCKET socket =(SOCKET)X;//当前接收消息的客户
	char recvbuf[max_buf];//接收消息缓冲区
    memset(recvbuf, '\0', sizeof(recvbuf));
    int rec;
    while(1)
    {
        memset(recvbuf, '\0', sizeof(recvbuf));
        rec = recv(socket, recvbuf, sizeof(recvbuf), 0);
        // cout<<"recv"<<endl;
		if (rec == SOCKET_ERROR)//接受失败
		{
			//单纯失败
			printTime();
			cout<<"Failed to receive message"<<endl;
            cout<<"Application Interrupt"<<endl;

            closesocket(my_socket);
            WSACleanup(); 

            Sleep(10*1000);
            exit(-1);
 
		}
        else
        {
            if(recvbuf[0] == '0') //聊天消息
            {
                printTime();
                cout<<"[Message]";
                for( int i=1;i<strlen(recvbuf);i++) cout<<recvbuf[i];  
                cout<<endl;         
            }
            else if(recvbuf[0] == '2')
            //服务器消息
            {
                printTime();
                cout<<"[Server Message]";
                for( int i=1;i<strlen(recvbuf);i++) cout<<recvbuf[i];  
                cout<<endl;
            }
            else if(recvbuf[0] == '1')//允许退出
            {
                printTime();
                for( int i=1;i<strlen(recvbuf);i++) cout<<recvbuf[i]; 
                closesocket(my_socket);
                WSACleanup(); 
                Sleep(1*1000);
                exit(-1);              
            }
        }
    }
}

int main()
{
    cout<<"Please input your IP:";
    cin>>my_IP;
    cout<<"Please input your Port:";
    cin>>my_Port;
    cout<<"Please input your username :";
    cin>>username;
  

    cout<<"====================================================================================="<<endl;
    cout<<"==================================Chat  Room========================================="<<endl;
    cout<<"====================================================================================="<<endl;
    cout<<"<<< IP : "<<my_IP<<endl;
    cout<<"<<< Port : "<<my_Port<<endl;
    cout<<endl;

    //检查连接
    WSADATA wsad;
	if (WSAStartup(MAKEWORD(2, 2), &wsad) != 0)
	{
		printTime();
		cout<<"Failed to load Winsock"<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Success to load Winsock"<<endl;
	}

	//创建socket
	my_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (my_socket == INVALID_SOCKET)
	{
		printTime();
		cout<<"Failed to creat socket : "<<WSAGetLastError()<<endl;
        closesocket(my_socket);
        WSACleanup();
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Success to creat socket"<<endl;
	} 

	SOCKADDR_IN my_addr;
	my_addr.sin_family = AF_INET;
    my_addr.sin_addr.S_un.S_addr = inet_addr(my_IP);
	my_addr.sin_port = htons(my_Port);	 

    //连接
    if( connect(my_socket,(sockaddr*)&my_addr,sizeof(my_addr)) == SOCKET_ERROR)
	{
		printTime();
		cout<<"Failed to connect : "<<WSAGetLastError();
        closesocket(my_socket);
        WSACleanup();
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Success to connect to server"<<endl;
	}

    char sendbuf[1062];
    char msg[1024];
    char timemsg[19];

    HANDLE hThread = CreateThread(NULL, NULL, &recvmsg, LPVOID(my_socket), 0, NULL);

    while(1)
    {
        memset(sendbuf,'\0',sizeof(sendbuf));
        memset(msg,'\0',sizeof(msg));
        memset(timemsg,'\0',sizeof(timemsg));
        char m[1024];
        cin.getline(m,1024);
        for(int i=0;i<strlen(m);i++)
        {
            msg[i]=m[i];
        }
        getTime(timemsg);
        writebuf(sendbuf,username, msg, timemsg);
        int s=send(my_socket, sendbuf, sizeof(sendbuf),0);
        if(s == SOCKET_ERROR)
        {
            printTime();
            cout<<"Failed to send message : "<<WSAGetLastError() <<endl;
            closesocket(my_socket);
            WSACleanup();
            system("pause");
            return -1;
        }
    }
    closesocket(my_socket);
	WSACleanup();
    system("pause");
	return 0;
}